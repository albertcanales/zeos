/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

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

void ftoa(int a, char *b)
{
  // a = 25.02f;
  itoa(a/100, b);
  int len = strlen(b);
  b[len] = '.';
  itoa(a%100, b+len+1);
  b[len+3] = '0';
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

void perror()
{
  char buffer[256];

  itoa(errno, buffer);

  write(1, buffer, strlen(buffer));
}

int print_us(char* str) {
  return write(1, str, strlen(str));
}

int print_num(int num) {
  char buff[10];
  itoa(num, buff);
  return write(1, buff, strlen(buff));
}

float getfps(int frames) {
  return 18*frames/gettime();
}