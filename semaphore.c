#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "spinlock.h"
#define NSEMS 32 //number of entries for array of semaphores

typedef struct semaphore_t {
   int value;
   int active; 
   struct spinlock lock;
} t_sem;

t_sem sem[32]; 

void sem_init1 (void) {
	for (int i=0; i<NSEMS ; i++) {
		initlock(&sem[i].lock, "semaphore");
		sem[i].active = 0;
		sem[i].value = 0; 
	}
}

int sem_init2 (int index, int max) {
	if (index < 0 || index > NSEMS) {//check if it's within range of the semaphore array
		return -1;
	}
	acquire(&sem[index].lock);
	if (sem[index].active != 0 ){//check if it's active or not
		return -1;
	}
	sem[index].value = max;
	sem[index].active = 1;
	release(&sem[index].lock);
	return 0;  
}

int sem_wait (int index, int count) {
	if (index < 0 || index > NSEMS) {//check if it's within range of the semaphore array
		return -1;
	}
	acquire(&sem[index].lock);
	if (sem[index].active == 0 ){//check if it's active or not
		return -1;
	}
	while(sem[index].value <= (count-1)){
		sleep(&sem[index], &sem[index].lock); 
	}
	//enters critical section
	sem[index].value -= count;
	release(&sem[index].lock);
	return 0;
}

int sem_signal (int index, int count) {
	if (index < 0 || index > NSEMS) {//check if it's within range of the semaphore array
		return -1;
	}
	acquire(&sem[index].lock);
	if (sem[index].active == 0 ){//check if it's active or not
		return -1;
	}
	//enters critical section
	sem[index].value += count;
	if (sem[index].value > 0) { //if lock is available, wakeup semaphore 
		wakeup(&sem[index]);
	}
	release(&sem[index].lock);
	return 0;
}
