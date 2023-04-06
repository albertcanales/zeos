#include <libc.h>

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  int ret;

  char *buffer = "\nHeyyy que tal!\n";

  // Hauria de funcionar bé
  
  ret = write(1, buffer, strlen(buffer));
  if(ret < 0)
    perror();

  // Hauria de petar pel fd
  ret = write(0, buffer, strlen(buffer));
  if(ret < 0)
    perror();

  // Hauria de petar pel buffer
  ret = write(1, 0x0, strlen(buffer));
  if(ret < 0)
    perror();

  // Hauria de petar per la size
  ret = write(1, buffer, -1);
  if(ret < 0)
    perror();

  char *buffer2 = "Aixo es un text molt molt molt llarg\n\n\n";
  ret = write(1, buffer2, strlen(buffer2));

  char *buffer4 = "The time is: ";
  char timestr[10];
  itoa(gettime(), timestr);
  write(1, buffer4, strlen(buffer4));
  write(1, timestr, strlen(timestr));
  write(1, "\n", 1);

  char *buffer5 = "The PID is: ";
  write(1, buffer5, strlen(buffer5));
  char pidstr[10];
  itoa(getpid(), pidstr);
  write(1, pidstr, strlen(pidstr));
  write(1, "\n", 1);

  //fork();

  write(1, buffer5, strlen(buffer5));
  itoa(getpid(), pidstr);
  write(1, pidstr, strlen(pidstr));
  write(1, "\n", 1);

  int i = 0;
  char *buffer6 = "Timer: ";
  char istr[10];
  while(1) {
    write(1, buffer5, strlen(buffer5));  
    itoa(i, istr);
    write(1, istr, strlen(istr));
    write(1, "\n", 1);
    i++;
    for(int j = 0; j < 1000000; j++);
  }
}
