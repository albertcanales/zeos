#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  char b[100];
  int num_read;
  char numstr[10];

  gotoxy(2, 3);

  while(1) {
    num_read = read(b, 4);
    write(1, b, num_read);
    itoa(num_read, numstr);
    write(1, numstr, strlen(numstr));

    for(int i = 0; i < 1000000000; i++);
  }
}
