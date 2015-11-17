#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

#include "memlayout.h"

#include <stddef.h>

/*
 * This is where you'll need to implement the user-level functions
 */

void lock_init(lock_t *lock) {
  lock->locked = 0;
  }

void lock_acquire(lock_t *lock) {
 
  while(xchg(&lock->locked, 1) != 0);
}

void lock_release(lock_t *lock) {

  xchg(&lock->locked, 0);

}

typedef struct {
    int p_pid;
    void* stack;
} tTuple;

tTuple List[64]; //globabl variable to keep track of thread stacks
int stacknum =0;

int thread_join(int pid) {
	int newpid;
	void* stack;
	int j;
	if (pid ==-1){
	  newpid= join(-1);
	  if (newpid ==-1){ return -1;}
	  for (j=0;j<64;j++){
	    if (List[j].p_pid==newpid) //matches with the newpid which has been joined
	  	  {stack=List[j].stack;} //assign its stack to be freed later
	  }
	}
        else{
	  for (j=0;j<64;j++){
	    if (List[j].p_pid==pid){
		stack=List[j].stack;
		newpid =join(pid);
		}
		
            }
	}
        //int 
	free(stack);
	return newpid;
}

int thread_create(void (*start_routine)(void *), void *arg) {
  
  void *stack = malloc(PGSIZE*2); //allocate stack
  
  if((uint)stack % PGSIZE) 
     stack = stack + (PGSIZE - (uint)stack % PGSIZE);

  if (stack == NULL ) {//if stack allocation fails
    printf (1, "malloc failure \n");
    return -1;
  }
  int tid = 0;
  tid = clone (start_routine, arg, stack); 
  if (tid ==-1) {
    printf(1, "clone failure \n");
    return -1;
  }
  //update the global thread stack tracker 
  List[stacknum].p_pid=tid;
  List[stacknum].stack=stack;
  stacknum++;
  return tid;
}



