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
	struct task_struct * child = list_head_to_task_struct(list_head_fork);
	list_del(list_head_fork);

	union task_union * parent_task_union = ((union task_union*)current());
	union task_union * child_task_union = ((union task_union*)child);

	copy_data(current(), child, KERNEL_STACK_SIZE);

	allocate_DIR(child);

	// Get frames for child
	int child_pages[NUM_PAG_DATA];
	for(int i = 0; i < NUM_PAG_DATA; i++) {
		child_pages[i] = alloc_frame();
		if (child_pages[i] == -1) {
			// Free previous resources
			for(int j = 0; j < i; j++)
				free_frame(child_pages[j]);
			list_add_tail(&freequeue, list_head_fork);
			return ENOMEM;
		}
	}

	page_table_entry * parent_table_page = get_PT(current());
	page_table_entry * child_table_page = get_PT(child);
	
	// Assign logical pages of child
	for(int i = 0; i < NUM_PAG_KERNEL; i++)
		set_ss_pag(child_table_page, i, get_frame(parent_table_page, i));

	for(int i = 0; i < NUM_PAG_CODE; i++)
		set_ss_pag(child_table_page, PAG_LOG_INIT_CODE+i, 
			get_frame(parent_table_page, PAG_LOG_INIT_CODE+i));

	for(int i = 0; i < NUM_PAG_DATA; i++)
		set_ss_pag(child_table_page, PAG_LOG_INIT_DATA+i, 
			child_pages[i]);

	// TODO Copy the Data+Stack from parent to child

	set_cr3(get_DIR(current()));

	// TODO Sections g and h

	list_add_tail(&child->list, &readyqueue);

  	return child->PID;
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
