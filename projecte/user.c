#include <libc.h>

char buff[24];

int pid;

int frames = 0;

void update_fps() {
  char fpsstr[10];
  float fps = getfps(frames);
  ftoa(fps*100, fpsstr);
  set_color(15, 3);
  gotoxy(70,0);
  for(int i = 0; i < 10; i++)
    write(1, " ", 1);
  gotoxy(80-strlen(fpsstr),0);
  write(1, fpsstr, strlen(fpsstr));
  frames++;
}

int ended;

// 0 = up, 1 = left, 2 = down, 3 = right
int *direction;

#define MAX_SIZE 13

int snake_x[MAX_SIZE];
int snake_y[MAX_SIZE];
int head, size;

int apple_x, apple_y;

int dx[] = {0,-2,0,2};
int dy[] = {-1,0,1,0};


void reader() {
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
  apple_x = ((apple_x + 126731) % 76 + 2) >> 1 << 1;
  apple_y = ((apple_y + 237891) % 23 + 1) >> 1 << 1;
  gotoxy(apple_x, apple_y);
  set_color(0, 4);
  write(1, "  ", 2);
}

void paint_snake_pixel(int x, int y) {
  gotoxy(x, y);
  set_color(0, 2);
  write(1, "  ", 2);
}

void clear_pixel(int x, int y) {
  gotoxy(x, y);
  set_color(0, 0);
  write(1, "  ", 2);
}

void update_score() {
  char scorestr[4];
  itoa(size-4, scorestr);
  set_color(15,3);
  gotoxy(7, 0);
  write(1, scorestr, strlen(scorestr));
}

void clear_screen() {
  set_color(2,0);
  for(int i = 0; i < 80*25; i++) {
    write(1, " ", 1);
  }
}

void move() {
  // Move snake
  int old_head_x = snake_x[head];
  int old_head_y = snake_y[head];
  int new_head_x = (old_head_x + dx[*direction]);
  int new_head_y = (old_head_y + dy[*direction]);

  // Check collisions with wall
  if (new_head_x < 2 || new_head_x >= 78 || new_head_y < 1 || new_head_y >= 24)
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

    // Check that apple not in snake body
    int colliding = 0;
    do {
      for (int i = 0; i < MAX_SIZE; i++)
        if (snake_x[i] == apple_x && snake_y[i] == apple_y)
          colliding = 1;
    } while(colliding);


    update_score();
  }

  // Apply movement
  if (ended == 0) {
    head = (head + 1) % size;
    clear_pixel(snake_x[head], snake_y[head]);
    snake_x[head] = new_head_x;
    snake_y[head] = new_head_y;
    paint_snake_pixel(snake_x[head], snake_y[head]);
  }

  // Check win
  if (size > MAX_SIZE)
    ended = 1;
}

void show_score() {
  char lost[] = "Has perdido, paquete";
  char segmentation[] = "Segmentation fault";
  char won[] = "Nah es broma :P, has ganado!";
  char score[] = "Score: ";
  char scorestr[10];
  clear_screen();
  set_color(4, 0);
  if(size > MAX_SIZE) {
    gotoxy((80-strlen(segmentation))/2, 12);
    write(1, segmentation, strlen(segmentation));
    sleep(1000);
    gotoxy((80-strlen(won))/2, 13);
    write(1, won, strlen(won));
  }
  else {
    itoa(size-4, scorestr);
    gotoxy((80-strlen(lost))/2, 12);
    write(1, lost, strlen(lost));
    gotoxy((80-strlen(score)-strlen(scorestr))/2, 13);
    write(1, score, strlen(score));
    write(1, scorestr, strlen(scorestr));
  }
}

void init() {
  ended = 0;
  size = 4;
  head = 0;

  // Inciar snake
  for (int i = 0; i < MAX_SIZE; i++) {
    if(i < size) {
      snake_x[i] = 40;
      snake_y[i] = 19+i;
    }
    else {
      snake_x[i] = 0;
      snake_y[i] = 0;
    }
  }

  // Clear screen
  clear_screen();

  // Draw border
  set_color(15, 3);
  gotoxy(0,0);
  for(int x = 0; x < 80; x++)
    write(1, " ", 1);
  gotoxy(0,24);
  for(int x = 0; x < 80; x++)
    write(1, " ", 1);
  gotoxy(0,0);
  for(int y = 0; y < 25; y++) {
    write(1, "  \n", 3);
  }
  for(int y = 0; y < 25; y++) {
    gotoxy(78,y);
    write(1, "  ", 2);
  }

  // Draw fps and score
  gotoxy(0,0);
  write(1, "Score: ", 7);
  update_score();
  gotoxy(65,0);
  write(1, "FPS: ", 5);

  // Print snake
  for(int i = 0; i < size; i++) {
    paint_snake_pixel(snake_x[i], snake_y[i]);
  }

  // Print apple
  paint_apple();

  // Dirty hack
  for(int i = 0; i < 4; i++)
    move();
}

void game() {
  // Initialize game
  init();

  // Gaming loop
  while(!ended) {
    move();
    if(*direction % 2 == 0)
      sleep(200);
    else
      sleep(250);
    update_fps();
    // ended = 1;
  }
  // Show score screen
  show_score();
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  direction = shmat(1, (void*)0);
  if(fork() > 0) {
    reader();
  }
  else {
    game();
  }
  while(1);
}
