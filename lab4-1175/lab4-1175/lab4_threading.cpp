#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/syscall.h>
#define _GNU_SOURCE

// Define the mutex as global variable
pthread_mutex_t lock;
pthread_mutexattr_t attributes;

// slow down loops
#define LOOP_LARGE_NUMBER (1000000000)
#define LOOP_10PERCENT_NUMBER (LOOP_LARGE_NUMBER / 10)

int policy = SCHED_FIFO;

int get_real_time_priority() {
    char str[50];
    sprintf(str, "/proc/%ld/stat", syscall(SYS_gettid));
    FILE *fp = fopen(str, "r");
    for(int i = 0; i < 17; i++) {
        fscanf(fp, "%s", str);
    }
    int rt_priority = 0;
    fscanf(fp, "%d", &rt_priority);
    fclose(fp);
    return -rt_priority-1;
}

void *thread1handler(void *arg) {
	printf("Thread 1 start\n");
	fflush(stdout);

    printf("Thread 1 requests the lock\n");
	fflush(stdout);

	pthread_mutex_lock(&lock);
    printf("Thread 1 had the lock \n");
	fflush(stdout);

    int counter = 0;
	for(int i = 0; i < LOOP_LARGE_NUMBER; i++)
	{
		int a = 4*4;
        if ((i > 0) && (i % LOOP_10PERCENT_NUMBER == 0)) {
            counter += 1;
            printf("Thread 1 running, priority %d, process %d0%%...\n", get_real_time_priority(), counter);
            fflush(stdout);
        }
	}

	printf("Thread 1 released the lock\n");
	fflush(stdout);
    
	pthread_mutex_unlock(&lock);

	printf("Thread 1 complete\n");
	fflush(stdout);

}

void *thread2handler(void *arg) {
	printf("Thread 2 start\n");
	fflush(stdout);

    printf("Thread 2 requests the lock\n");
	fflush(stdout);

	pthread_mutex_lock(&lock);
    printf("Thread 2 had the lock \n");
	fflush(stdout);

    int counter = 0;
	for(int i = 0; i < LOOP_LARGE_NUMBER; i++)
	{
		int a = 4*4;
        if ((i > 0) && (i % LOOP_10PERCENT_NUMBER == 0)) {
            counter += 1;
            printf("Thread 2 running, priority %d, process %d0%%...\n", get_real_time_priority(), counter);
            fflush(stdout);
        }
	}

	printf("Thread 2 released the lock\n");
	fflush(stdout);
    
	pthread_mutex_unlock(&lock);

	printf("Thread 2 complete\n");
	fflush(stdout);

}

void *thread3handler(void *arg) {
	printf("Thread 3 start\n");
	fflush(stdout);

    printf("Thread 3 requests the lock\n");
	fflush(stdout);

	pthread_mutex_lock(&lock);
    printf("Thread 3 had the lock \n");
	fflush(stdout);

    int counter = 0;
	for(int i = 0; i < LOOP_LARGE_NUMBER; i++)
	{
		int a = 4*4;
        if ((i > 0) && (i % LOOP_10PERCENT_NUMBER == 0)) {
            counter += 1;
            printf("Thread 3 running, priority %d, process %d0%%...\n", get_real_time_priority(), counter);
            fflush(stdout);
        }
	}

	printf("Thread 3 released the lock\n");
	fflush(stdout);
    
	pthread_mutex_unlock(&lock);

	printf("Thread 3 complete\n");
	fflush(stdout);

}

int main() {
	//initialize mutex
	pthread_mutexattr_setprotocol(&attributes, PTHREAD_PRIO_INHERIT);
	pthread_mutex_init(&lock, &attributes);

    //Check priority
    int maxPriority = sched_get_priority_max(policy); 
    int minPriority = sched_get_priority_min(policy); 
    printf("Priority range: [%d, %d]\n", minPriority, maxPriority);

    // define a schedule parameter
    struct sched_param param_main = { 0 };
    int priority_mainthread = 99; 
    param_main.sched_priority = priority_mainthread;
    pthread_setschedparam(pthread_self(), policy, &param_main);
    printf("Priority of main thread: %d\n", get_real_time_priority());

    //bind the schedule parameter to a thread
    int priority1 = 10; 
    pthread_t thread1 = { 0 }; 
    struct sched_param param1 = { 0 };
	param1.sched_priority = priority1;
    pthread_attr_t attr1;
    pthread_attr_init(&attr1);
    pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr1, policy);
    pthread_attr_setschedparam(&attr1, &param1);
    
    //Initiate thread 2
    int priority2 = 5; 
    pthread_t thread2 = { 0 }; 
    struct sched_param param2 = { 0 };
	param1.sched_priority = priority2;
    pthread_attr_t attr2;
    pthread_attr_init(&attr2);
    pthread_attr_setinheritsched(&attr2, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr2, policy);
    pthread_attr_setschedparam(&attr2, &param2);
    
    //Initiate thread 3
    int priority3 = 2; 
    pthread_t thread3 = { 0 }; 
    struct sched_param param3 = { 0 };
	param1.sched_priority = priority3;
    pthread_attr_t attr3;
    pthread_attr_init(&attr3);
    pthread_attr_setinheritsched(&attr3, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr3, policy);
    pthread_attr_setschedparam(&attr3, &param3);

    // create thread 3 
    printf("Creating thread3...\n");
    fflush(stdout);
	pthread_create(&thread3, &attr3, thread3handler, NULL);
    sleep(1);

    // create thread 1
	printf("Creating thread1...\n");
    fflush(stdout);
	pthread_create(&thread1, &attr1, thread1handler, NULL);
    sleep(1);
    
    // create thread 2
    printf("Creating thread2...\n");
    fflush(stdout);
	pthread_create(&thread2, &attr2, thread2handler, NULL);
    sleep(1);

    
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	pthread_join(thread3,NULL);

	return 0;
}
