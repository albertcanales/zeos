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
  char pidstr[10];
  write(1, buffer5, strlen(buffer5));
  itoa(getpid(), pidstr);
  write(1, pidstr, strlen(pidstr));
  write(1, "\n", 1);

  int fork_res = 0; //fork();

  char *buffer7 = "Process X got Y: ";
  char forkstr[10];
  write(1, buffer7, strlen(buffer7));
  itoa(getpid(), pidstr);
  write(1, pidstr, strlen(pidstr));
  write(1, " ", 1);
  itoa(fork_res, forkstr);
  write(1, forkstr, strlen(forkstr));
  write(1, "\n", 1);

  // Trying to force a EAGAIN
  int val_fork = 0;
  int i = 0;
  while(0 && val_fork >= 0) {
    val_fork = fork();
    if(val_fork == -1) {
      perror();
    }
    i++;
  }

  char *buffer8 = "Fork error at iteration ";
  char itstr[10];

  if(1) {
    write(1, buffer8, strlen(buffer8));
    itoa(i, itstr);
    write(1, itstr, strlen(itstr));
    write(1, "\n", 1);
  }

  struct stats st;

  // Hauria de petar per pid invàlid
  ret = get_stats(-1, &st);
  if(ret < 0)
    perror();
  
  // Hauria de petar per invàlid st
  ret = get_stats(getpid(), 0x0);
  if(ret < 0)
    perror();

  // Hauria de petar per pid no existent
  ret = get_stats(3000, &st);
  if(ret < 0)
    perror();

  char *buffer9 = "Stats are: ";
  char statstr[20];
  while(1) {
    //Hauria d'anar bé
    ret = get_stats(getpid(), &st);
    if(ret < 0)
      perror();

    write(1, buffer9, strlen(buffer9));
    itoa(st.user_ticks, statstr);
    write(1, statstr, strlen(statstr));
    write(1, " ", 1);
    itoa(st.system_ticks, statstr);
    write(1, statstr, strlen(statstr));
    write(1, " ", 1);
    itoa(st.blocked_ticks, statstr);
    write(1, statstr, strlen(statstr));
    write(1, " ", 1);
    itoa(st.ready_ticks, statstr);
    write(1, statstr, strlen(statstr));
    write(1, " ", 1);
    itoa(st.elapsed_total_ticks, statstr);
    write(1, statstr, strlen(statstr));
    write(1, " ", 1);
    itoa(st.total_trans, statstr);
    write(1, statstr, strlen(statstr));
    write(1, " ", 1);
    itoa(st.remaining_ticks, statstr);
    write(1, statstr, strlen(statstr));
    write(1, "\n", 1);

    for(int i = 0; i < 100000000; i++);
  }
}
