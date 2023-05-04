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
  char prova[] = "hola!!!!";
  write(1, prova, strlen(prova));

  char falla[] = "tot falla";
  if (gotoxy(100, 20) && gotoxy(20, 100) && gotoxy(-2, 10) && gotoxy(20, -1))
    write(1, falla, strlen(falla));

  char provacol[] = "aixo hauria d'estar canviat";
  set_color(15, 1);
  write(1, provacol, strlen(provacol));
  if (set_color(17, 1) && set_color(1, 9) && set_color(-1, 5) && set_color(3, -2))
    write(1, falla, strlen(falla));

  char* addr = shmat(2, (void*)0); 

  *addr = 'a';

  int ret = fork();

  if(ret == 0) {
    char* addr2 = shmat(2, (void*)0x1000000);
    *addr2 = 'b';
    while(1);
  }
  else {
    while(1) {
      write(1, addr, 1);
    }
  }

  while(1) {
    num_read = read(b, 4);
    write(1, b, num_read);
    itoa(num_read, numstr);
    write(1, numstr, strlen(numstr));

    for(int i = 0; i < 1000000000; i++);
  }
}
