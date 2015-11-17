#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "spinlock.h"

typedef struct semaphore_t {
   int value;
   struct spinlock lock;
} t_sem;

t_sem sem[32]; 

void sem_init1 (void) {
	for (int i=0; i<32 ; i++) {
		initlock(&sem[i].lock, "semaphore");
		sem[i].value = 0; 
	}
}

int sem_init2 (int index, int max_val) {
	if (index < 0 || index > 32) {//check if it's within range of the semaphore array
		return -1;
	}
	acquire(&sem[index].lock);
	sem[index].value = max_val;
	release(&sem[index].lock);
	return 0;  
}

int sem_wait (int index, int c) {
	if (index < 0 || index > 32) {//check if it's within range of the semaphore array
		return -1;
	}
	acquire(&sem[index].lock);
	while(sem[index].value <= (c-1)){//semaphore not available (value less than c) 
		sleep(&sem[index], &sem[index].lock); 
	}
	//enters critical section
	sem[index].value -= c;
	release(&sem[index].lock);
	return 0;
}

int sem_signal (int index, int count) {
	if (index < 0 || index > NSEMS) {//check if it's within range of the semaphore array
		return -1;
	}
	acquire(&sem[index].lock);
	//enters critical section
	sem[index].value += count;
	if (sem[index].value > 0) { //if lock is available, wakeup semaphore 
		wakeup(&sem[index]);
	}
	release(&sem[index].lock);
	return 0;
}
