#include <libc.h>

char buff[24];

int pid;

int frames = 0;

void update_fps() {
  char fpsstr[10];
  float fps = getfps(frames);
  ftoa(fps*100, fpsstr);
  write(1, fpsstr, strlen(fpsstr));
  write(1, "\n", 1);
  frames++;
}

int ended;

int head_x, head_y;
// 0 = up, 1 = left, 2 = down, 3 = right
int* direction;

#define MAX_SIZE 100

int snake_x[MAX_SIZE];
int snake_y[MAX_SIZE];
int head, size;

int apple_x, apple_y;

int dx[] = {0,-1,0,1};
int dy[] = {1,0,-1,0};


void reader() {
  *direction = 0;
  char buff[2];
  while(1) {
    if(read(buff, 1)) {
      switch(*buff) {
        case 'w':
          *direction = 0;
          break;
        case 'a':
          *direction = 1;
          break;
        case 's':
          *direction = 2;
          break;
        case 'd':
          *direction = 3;
          break;
      }
    }
  }
}

void init() {
  ended = 0;
  head_y = 15;
  head_x = 40;
  size = 1;
  head = 0;

  // Reestablir snake
  for (int i = 0; i < MAX_SIZE; i++)
    snake_x[i] = 0;
  for (int i = 0; i < MAX_SIZE; i++)
    snake_y[i] = 0;
  snake_x[0] = head_x;
  snake_y[0] = head_y;
}

void game() {
  // Initialize snake
  init();

  // Initialize screen
  char buff[2];
  while(!ended) {
    itoa(*direction, buff);
    write(1, buff, strlen(buff));
    // Move snake
    // Check collisions (itself, screen)
    // Check apples
    // Wait for tick
    // Update fps
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
