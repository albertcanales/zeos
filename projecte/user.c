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

// 0 = up, 1 = left, 2 = down, 3 = right
int* direction;

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

void game() {
  // Initialize snake
  // Initialize screen
  int ended = 0;
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
