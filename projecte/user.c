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

char a[] = "a";

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  if(fork() > 0) {
    *a = 'b';
    write(1, a, 1);
  }
  else {
    *a = 'c';
    write(1, a, 1);
  }
  while(1);
}
