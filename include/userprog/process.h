#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"




// load segment에 사용되는 구조체
struct file_info {
	struct file *file;
	off_t ofs;
	size_t read_bytes;
};


tid_t process_create_initd (const char *file_name);
tid_t process_fork (const char *name, struct intr_frame *if_);
int process_exec (void *f_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (struct thread *next);

bool lazy_load_segment (struct page *page, void *aux);

#endif /* userprog/process.h */
