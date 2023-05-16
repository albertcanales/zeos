#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  int* shared[1024];

  int i = 0;
  while(i < 512) {
    shared[i] = shmat(2, (void*)0);
    *shared[i] = 1234;
    i++;
  }
  print_num(i);
  write(1, "\n", 1);

  char error[] = "Se ha machacado!!";
  char done[] = "He acabat";
  char fill[] = "Soc el fill";
  
  int ret = fork();
  if(ret > 0) {
    for(int j = 0; j < i; j++) {
      if(*shared[j] != 1234) {
        write(1, error, strlen(error));
        print_num(j);
        write(1, " ", strlen(" "));
        print_num(*shared[j]);
        write(1, "\n", strlen("\n"));
      }
    }
    write(1, done, strlen(done));
  }
  else {
    write(1, fill, strlen(fill));
  }

  while(1);
}
