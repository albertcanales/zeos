#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

    char funciona[] = "Funciona!!!";
  char nofunciona[] = "No funciona :(";
  if(fork() == 0) {
    int* val = shmat(2, (void*)0);
    *val = 25;
    shmrm(2);
    
    int ret = fork();
    print_num(ret);
    if(ret > 0) {
      print_us("Soc el pare\n");
      shmdt(val);
    }
    else {
      shmdt(val);
      int* val2 = shmat(2, (void*) val);
      if(*val2 == 0) {
        write(1, funciona, strlen(funciona));
      }
      else {
        write(1, nofunciona, strlen(nofunciona));
        print_num(*val2);
      }
    }
  }

  while(1);
}
