/*
 * sys.c - Syscalls implementation
 */
#include "list.h"
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
#include <errno.h>
#include <interrupt.h>

#define LECTURA 0
#define ESCRIPTURA 1

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

# define BUFFER_SIZE 4096
char buff[BUFFER_SIZE];

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF;
  if (permissions!=ESCRIPTURA) return -EACCES;
  return 0;
}

int sys_ni_syscall()
{
	return -ENOSYS;
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
	if (list_empty(&freequeue)) return EAGAIN; // No more PIDs available

	struct list_head * list_head_fork = freequeue.next;
	struct task_struct * new_task = list_head_to_task_struct(list_head_fork); 

	union task_union * parent_task_union = ((union task_union*)current());
	union task_union * child_task_union = ((union task_union*)new_task);

	copy_data(current(), new_task, KERNEL_STACK_SIZE);

	allocate_DIR(new_task);
	int frame = alloc_frame();
	if (frame == -1) return ENOMEM; // No memory available

	//inherit user 
	//page_table_entry * = get_PT(new_task);

  // creates the child process
  int PID = 0;
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
	if(size < 0) return -EINVAL;
	ret = access_ok(VERIFY_READ, buffer, size);
	if(ret == 0) return -EFAULT;

	// Copy data and implement service
	int written = 0;
	while(written < size) {
		int to_read = MIN(size-written, BUFFER_SIZE);
		copy_from_user(buffer+written, buff, to_read);
		int bytes = sys_write_console(buff, to_read);
		written += bytes;
		if(bytes < to_read) return written;
	}
	return written;
}

int sys_gettime() {
	return zeos_ticks;
}
