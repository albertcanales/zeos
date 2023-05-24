#include <libc.h>

char buff[24];

int pid;

int frames = 0;

void update_fps() {
  char fpsstr[10];
  float fps = getfps(frames);
  ftoa(fps*100, fpsstr);
  gotoxy(0,0);
  write(1, fpsstr, strlen(fpsstr));
  write(1, "\n", 1);
  frames++;
}

int ended;

// 0 = up, 1 = left, 2 = down, 3 = right
int *direction;

#define MAX_SIZE 100

int snake_x[MAX_SIZE];
int snake_y[MAX_SIZE];
int head, size;

int apple_x, apple_y;

int dx[] = {0,-1,0,1};
int dy[] = {-1,0,1,0};


void reader() {
  *direction = 0;
  char buff[2];
  while(1) {
    if(read(buff, 1)) {
      switch(*buff) {
        case 'w':
          if(*direction != 2)
            *direction = 0;
          break;
        case 'a':
          if(*direction != 3)
            *direction = 1;
          break;
        case 's':
          if(*direction != 0)
            *direction = 2;
          break;
        case 'd':
          if(*direction != 1)
            *direction = 3;
          break;
      }
    }
  }
}

void paint_apple() {
  apple_x = ((apple_x + 126731) % 80);
  apple_y = ((apple_y + 237891) % 25);
  gotoxy(apple_x, apple_y);
  write(1, "X", 1);
}

void paint_snake_pixel(int x, int y) {
  gotoxy(x, y);
  write(1, "O", 1);
}

void clear_pixel(int x, int y) {
  gotoxy(x, y);
  write(1, " ", 1);
}

void init() {
  ended = 0;
  size = 4;
  head = 0;

  // Inciar snake
  for (int i = 0; i < MAX_SIZE; i++) {
    if(i < size) {
      snake_x[i] = 40;
      snake_y[i] = 15+i;
    }
    else {
      snake_x[i] = 0;
      snake_y[i] = 0;
    }
  }

  // Clear screen
  for(int i = 0; i < 80*25; i++) {
    write(1, " ", 1);
  }
  // Print snake
  for(int i = 0; i < size; i++) {
    paint_snake_pixel(snake_x[i], snake_y[i]);
  }
  // Print apple
  paint_apple();
}

void move() {
  // Move snake
  int old_head_x = snake_x[head];
  int old_head_y = snake_y[head];
  int new_head_x = (old_head_x + dx[*direction]);
  int new_head_y = (old_head_y + dy[*direction]);

  // Check collisions with wall
  if (new_head_x < 0 || new_head_x >= 80 || new_head_y < 0 || new_head_y >= 25)
    ended = 1;

  // Check collisions with itself
  for (int i = 0; i < MAX_SIZE; i++)
    if (i != head && snake_x[i] == new_head_x && snake_y[i] == new_head_y)
      ended = 1;

  // Check apples
  if (new_head_x == apple_x && new_head_y == apple_y) {

    for (int i = size - 1; i > head; i--) {
      snake_x[i + 1] = snake_x[i];
      snake_y[i + 1] = snake_y[i];
    }
    snake_x[head + 1] = apple_x;
    snake_y[head + 1] = apple_y;

    size++;
    paint_apple();
  }

  // Apply movement
  if (ended == 0) {
    head = (head + 1) % size;
    if (snake_x[head] != apple_x || snake_y[head] != apple_y)
      clear_pixel(snake_x[head], snake_y[head]);
    snake_x[head] = new_head_x;
    snake_y[head] = new_head_y;
    paint_snake_pixel(snake_x[head], snake_y[head]);
  }

  // Check win
  if (size > MAX_SIZE)
    ended = 1;
}

void game() {
  // Initialize game
  init();

  // Gaming loop
  while(!ended) {
    move();
    sleep(10000);
    update_fps();
  }
  // Show score screen
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  direction = shmat(1, (void*)0);
  if(fork() > 0) {
    game();
  }
  else {
    reader();
  }
  while(1);
}
