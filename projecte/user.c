#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  char* addr = shmat(2, (void*)0);

  if(addr < 0) {
    char error[] = "ERROR!!!!";
    write(1, error, strlen(error));
  }

  *addr = 'x';
  write(1, addr, 1);

  int ret = fork();
  print_us("He fet el fork\n");

  if(ret > 0)
    write(1, addr, 1);

  while(1);
}
