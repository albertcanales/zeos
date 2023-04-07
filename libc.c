/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

#include <errno.h>

int errno;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

// Very unsafe memory access, but not for perror
void strcpy(char* orig, char* dest) {
  while(*orig != 0)
    *(dest++) = *(orig++);
  *dest = 0;
}

void perror() {
  char buff[512];
  switch(errno) {
    case EINVAL:
      strcpy("Invalid argument.\n", buff);
      break;
    case EACCES:
      strcpy("Permission denied.\n", buff);
      break;
    case ENOSYS:
      strcpy("Invalid system call number.\n", buff);
      break;
    case EBADF:
      strcpy("File descriptor in bad state.\n", buff);
      break;
    case EFAULT:
      strcpy("Bad address.\n", buff);
      break;
    case ENOSPC:
      strcpy("No space left on device.\n", buff);
      break;
    case EAGAIN:
      strcpy("Try again.\n", buff);
      break;
    case ENOMEM:
      strcpy("Out of memory.\n", buff);
      break;
    case ESRCH:
      strcpy("No such process.\n", buff);
      break;
    default:
      strcpy("Unrecognised error.\n", buff);
  }
  write(1, buff, strlen(buff));
}

