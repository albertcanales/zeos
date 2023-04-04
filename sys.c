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

int ret_from_fork() {
	return 0;
}

int sys_fork()
{
	if (list_empty(&freequeue)) return EAGAIN; // No more PIDs available

	struct list_head * list_head_fork = freequeue.next;
	struct task_struct * child = list_head_to_task_struct(list_head_fork);
	list_del(list_head_fork);

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

	page_table_entry * parent_page_table = get_PT(current());
	page_table_entry * child_page_table = get_PT(child);
	
	// Assign logical pages of child
	for(int i = 0; i < NUM_PAG_KERNEL; i++)
		set_ss_pag(child_page_table, i, get_frame(parent_page_table, i));

	for(int i = 0; i < NUM_PAG_CODE; i++)
		set_ss_pag(child_page_table, PAG_LOG_INIT_CODE+i, 
			get_frame(parent_page_table, PAG_LOG_INIT_CODE+i));

	for(int i = 0; i < NUM_PAG_DATA; i++)
		set_ss_pag(child_page_table, PAG_LOG_INIT_DATA+i, 
			child_pages[i]);

	// TODO Copy the Data+Stack from parent to child
	int SPACE_START = NUM_PAG_KERNEL;
	int SPACE_END = NUM_PAG_KERNEL+NUM_PAG_DATA;

	for(int i = SPACE_START; i < SPACE_END; i++) {
		set_ss_pag(parent_page_table, i+NUM_PAG_DATA, get_frame(child_page_table, i));
		copy_data((void *)(i<<12), (void *)((i+NUM_PAG_DATA)<<12), PAGE_SIZE);
		del_ss_pag(parent_page_table, i+NUM_PAG_DATA);
	}

	set_cr3(get_DIR(current()));

	// Modify child parameters
	child->PID = next_pid++;
	
	// Emulate task_switch
	DWord * child_kernel_esp = (DWord *)KERNEL_ESP(child_task_union);
	child_kernel_esp[-19] = 0;
	child_kernel_esp[-18] = (DWord)ret_from_fork;
	child->kernel_esp = (DWord)&child_kernel_esp[-19];

	// End fork
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
