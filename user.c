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

  char *buffer2 = "Aixo es un text molt molt molt llarg\n";
  ret = write(1, buffer2, strlen(buffer2));

  char buffer3[13000];
  for(int i = 0; i < 12999; i++)
    buffer3[i] = 'a' + i/4096;
  buffer3[12999] = 0;

  ret = write(1, buffer3, 13000);
  if(ret < 0)
    perror();

  while(1) { }
}
