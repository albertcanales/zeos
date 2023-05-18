/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <p_stats.h>

#include <errno.h>

#include<buffer.h>

#include <interrupt.h>

#define LECTURA 0
#define ESCRIPTURA 1

void * get_ebp();

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF; 
  if (permissions!=ESCRIPTURA) return -EACCES; 
  return 0;
}

void user_to_system(void)
{
  update_stats(&(current()->p_stats.user_ticks), &(current()->p_stats.elapsed_total_ticks));
}

void system_to_user(void)
{
  update_stats(&(current()->p_stats.system_ticks), &(current()->p_stats.elapsed_total_ticks));
}

int sys_ni_syscall()
{
	return -ENOSYS; 
}

int sys_getpid()
{
	return current()->PID;
}

int global_PID=1000;

int ret_from_fork()
{
  return 0;
}

int sys_fork(void)
{
  struct list_head *lhcurrent = NULL;
  union task_union *uchild;
  
  /* Any free task_struct? */
  if (list_empty(&freequeue)) return -ENOMEM;

  lhcurrent=list_first(&freequeue);
  
  list_del(lhcurrent);
  
  uchild=(union task_union*)list_head_to_task_struct(lhcurrent);
  
  /* Copy the parent's task struct to child's */
  copy_data(current(), uchild, sizeof(union task_union));
  
  /* new pages dir */
  allocate_DIR((struct task_struct*)uchild);

  /* Allocate pages for DATA+STACK */
  int new_ph_pag, pag, i;
  page_table_entry *process_PT = get_PT(&uchild->task);
  for (pag=0; pag<NUM_PAG_DATA; pag++)
  {
    new_ph_pag=alloc_frame();
    if (new_ph_pag!=-1) /* One page allocated */
    {
      set_ss_pag(process_PT, PAG_LOG_INIT_DATA+pag, new_ph_pag);
    }
    else /* No more free pages left. Deallocate everything */
    {
      /* Deallocate allocated pages. Up to pag. */
      for (i=0; i<pag; i++)
      {
        free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
        del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
      }
      /* Deallocate task_struct */
      list_add_tail(lhcurrent, &freequeue);
      
      /* Return error */
      return -EAGAIN; 
    }
  }

  /* Copy parent's SYSTEM and CODE to child. */
  page_table_entry *parent_PT = get_PT(current());
  for (pag=0; pag<NUM_PAG_KERNEL; pag++)
  {
    set_ss_pag(process_PT, pag, get_frame(parent_PT, pag));
  }
  for (pag=0; pag<NUM_PAG_CODE; pag++)
  {
    set_ss_pag(process_PT, PAG_LOG_INIT_CODE+pag, get_frame(parent_PT, PAG_LOG_INIT_CODE+pag));
  }

  /* Copy parent's DATA to child. We will use TOTAL_PAGES-1 as a temp logical page to map to */
  for (pag=NUM_PAG_KERNEL+NUM_PAG_CODE; pag<NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA; pag++)
  {
    /* Map one child page to parent's address space. */
    set_ss_pag(parent_PT, TOTAL_PAGES-1, get_frame(process_PT, pag));
    copy_data((void*)(pag<<12), (void*)((TOTAL_PAGES-1)<<12), PAGE_SIZE);
    del_ss_pag(parent_PT, TOTAL_PAGES-1);
    
    /* Deny access to the child's memory space */
    set_cr3(get_DIR(current()));
  }

  /* Copy parent's SHARED to child. */
  for (pag=NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA; pag < TOTAL_PAGES-1; pag++)
  {
    if(get_frame(parent_PT, pag)) {
      set_ss_pag(process_PT, pag, get_frame(parent_PT, pag));

      // Increment number of references
      for(int i = 0; i < SHARED_FRAMES; i++)
        if(shared_frames[i].id == get_frame(parent_PT, pag))
          shared_frames[i].ref++;
    }
  }

  uchild->task.PID=++global_PID;
  uchild->task.state=ST_READY;

  int register_ebp;		/* frame pointer */
  /* Map Parent's ebp to child's stack */
  register_ebp = (int) get_ebp();
  register_ebp=(register_ebp - (int)current()) + (int)(uchild);

  uchild->task.register_esp=register_ebp + sizeof(DWord);

  DWord temp_ebp=*(DWord*)register_ebp;
  /* Prepare child stack for context switch */
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=(DWord)&ret_from_fork;
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=temp_ebp;

  /* Set stats to 0 */
  init_stats(&(uchild->task.p_stats));

  /* Queue child process into readyqueue */
  uchild->task.state=ST_READY;
  list_add_tail(&(uchild->task.list), &readyqueue);
  
  return uchild->task.PID;
}

#define TAM_BUFFER 512

int sys_write(int fd, char *buffer, int nbytes) {
char localbuffer [TAM_BUFFER];
int bytes_left;
int ret;

	if ((ret = check_fd(fd, ESCRIPTURA)))
		return ret;
	if (nbytes < 0)
		return -EINVAL;
	if (!access_ok(VERIFY_READ, buffer, nbytes))
		return -EFAULT;
	
	bytes_left = nbytes;
	while (bytes_left > TAM_BUFFER) {
		copy_from_user(buffer, localbuffer, TAM_BUFFER);
		ret = sys_write_console(localbuffer, TAM_BUFFER);
		bytes_left-=ret;
		buffer+=ret;
	}
	if (bytes_left > 0) {
		copy_from_user(buffer, localbuffer,bytes_left);
		ret = sys_write_console(localbuffer, bytes_left);
		bytes_left-=ret;
	}
	return (nbytes-bytes_left);
}

int sys_read(char* b, int maxchars) {
  char localbuffer [TAM_BUFFER];
  int num_read;

  if (maxchars < 0)
    return -EINVAL;
  if (!access_ok(VERIFY_WRITE, b, maxchars))
    return -EFAULT;

  num_read = 0;
  while(!buffer_empty(&keyboard_buffer) && num_read < maxchars) {
    localbuffer[num_read % TAM_BUFFER] = buffer_pop(&keyboard_buffer);
    num_read++;
    if(num_read % TAM_BUFFER == 0)
      copy_to_user(localbuffer, b + num_read, TAM_BUFFER);
  }

  if(num_read % TAM_BUFFER != 0)
    copy_to_user(localbuffer, b + num_read - (num_read%TAM_BUFFER), num_read%TAM_BUFFER);

  return num_read;
}


extern int zeos_ticks;

int sys_gettime()
{
  return zeos_ticks;
}

/* System call to force a task switch */
int sys_yield()
{
  force_task_switch();
  return 0;
}

extern int remaining_quantum;

int sys_get_stats(int pid, struct stats *st)
{
  int i;
  
  if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) return -EFAULT; 
  
  if (pid<0) return -EINVAL;
  for (i=0; i<NR_TASKS; i++)
  {
    if (task[i].task.PID==pid)
    {
      task[i].task.p_stats.remaining_ticks=remaining_quantum;
      copy_to_user(&(task[i].task.p_stats), st, sizeof(struct stats));
      return 0;
    }
  }
  return -ESRCH; /*ESRCH */
}

int sys_gotoxy(int x, int y) {
  if (setxy(x,y) < 0)
    return -EINVAL;
  return 0;
}

int sys_set_color(int fg, int bg) {
  if (set_col(fg,bg) < 0)
    return -EINVAL;
  return 0;
}

void* sys_shmat(int id, void* addr) {
  if (id < 0 || id >= SHARED_FRAMES || (int)addr % PAGE_SIZE != 0)
    return (void*)-EINVAL;

  if(addr == NULL || get_frame(get_PT(current()), PH_PAGE((int)addr))) {
    
    int page = get_first_free_page(get_PT(current()));
    if(page < 0)
      return (void*)-ENOMEM;
    addr = (void*) (page * PAGE_SIZE);

  }

  set_ss_pag(get_PT(current()), PH_PAGE((int)addr), shared_frames[id].id);
  shared_frames[id].ref++;

  return addr;
}

int sys_shmdt(void* addr) {
  if((int)addr % PAGE_SIZE > 0)
    return -EINVAL;
  if(PH_PAGE((int)addr) < PAG_LOG_INIT_DATA+NUM_PAG_DATA)
    return -EFAULT;
  int frame = get_frame(get_PT(current()), PH_PAGE((int)addr));
  if(!frame)
    return -EFAULT;

  for(int i = 0; i < SHARED_FRAMES; i++)
  if(shared_frames[i].id == frame) {
    shared_frames[i].ref--;

    if(shared_frames[i].ref == 0 && shared_frames[i].del) {
      for(int i = 0; i < PAGE_SIZE / sizeof(int); i++) {
        ((int*)addr)[i] = 0;
      }
    }
  }
  del_ss_pag(get_PT(current()), PH_PAGE((int)addr));
  return 0;
}

int sys_shmrm(int id) {
  if (id < 0 || id >= SHARED_FRAMES)
    return -EINVAL;
  shared_frames[id].del = 1;
  return 0;
}

void sys_exit()
{  
  int i;

  page_table_entry *process_PT = get_PT(current());

  // Deallocate all the propietary physical pages
  for (i=0; i<NUM_PAG_DATA; i++)
  {
    free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
    del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
  }

  for(int page = PAG_LOG_INIT_DATA+NUM_PAG_DATA; page < TOTAL_PAGES; page++) {
    int frame = get_frame(process_PT, page);
    if(frame) {
      sys_shmdt((void*)(page * PAGE_SIZE));
    }
  }
  
  /* Free task_struct */
  list_add_tail(&(current()->list), &freequeue);
  
  current()->PID=-1;
  
  /* Restarts execution of the next process */
  sched_next_rr();
}