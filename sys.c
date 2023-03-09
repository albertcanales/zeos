/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1

char buff[4096];

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}

int sys_write(int fd, void *buffer, int size) {
	// Check errors
	int ret;
	ret = check_fd(fd, ESCRIPTURA);
	if(ret < 0) return ret;
	if(!buffer) return 14; /*EFAULT*/
	if(size <= 0 || size > 4096) return 22; /*EINVAL*/

	// Copy data
	if(copy_from_user(buffer, buff, size) < 0) return 28; /*ENOSPC*/

	// Implement service
	return sys_write_console(buff, size);
}
