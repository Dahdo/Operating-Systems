#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))



void readArguments(int argc, char **argv, int *threadCount);
void *pigWork(void *args);
void wolfWork(int *args);
void wallHeight(int *args, int index) ;

typedef unsigned int UINT;
typedef struct pigHouses{
	pthread_t tid;
	UINT seed;
} pigHouses_t;

typedef struct arrayProps {
    int * array;
    pthread_mutex_t *mxArray;
} arrayProps_t;

typedef struct timespec timespec_t;

int main(int argc, char ** argv) {
    int threadCount;
    readArguments(argc, argv, &threadCount);
     pigHouses_t *pigs = (pigHouses_t*)malloc(sizeof(pigHouses_t) * threadCount);\
     if(pigs == NULL){
        ERR("error allocating pigs");
        exit(0);
     }

     int randArray[threadCount];
     srand(time(NULL));
     for(int i = 0; i < threadCount; i++){
        randArray[i] = rand() % 101 + 1;
        printf("%d\n", randArray[i]);
     } 
     arrayProps_t *arrayProps;
     arrayProps->array = randArray;
     for(int i = 0; i < threadCount; i++){
        //arrayProps->mxArray[i] = PTHREAD_MUTEX_INITIALIZER;
     }
    
    for (int i = 0; i < threadCount; i++) {
		int err = pthread_create(&(pigs->tid), NULL, pigWork, randArray);
		if (err != 0)
			ERR("Couldn't create thread");
	}

    srand(time(NULL));
	time_t start, current;
    double diff;
    time(&start);
	do {
        wolfWork(randArray);
        time(&current);
        diff = difftime(start, current);
        diff = abs(diff);
        printf("start: %d, current: %d, dif: %f\n", start, current, diff);
        
	} while (diff < 6);


    // for (int i = 0; i < threadCount; i++) {
	// 	int err = pthread_join(pigs[i].tid, NULL);
	// 	if (err != 0)
	// 		ERR("Can't join with a thread");
	// }

    for (int i = 0; i < threadCount; i++) {
		int err = pthread_cancel(pigs[i].tid);
		if (err != 0)
			ERR("thread can't be canceled!");
	}
    free(pigs);
    exit(EXIT_SUCCESS);

}

void readArguments(int argc, char **argv, int *threadCount)
{
	if (argc >= 2) {
		*threadCount = atoi(argv[1]);
		if (*threadCount <= 0) {
			printf("Invalid value for N");
			exit(EXIT_FAILURE);
		}
	}
}
void msleep(UINT milisec)
{
	time_t sec = (int)(milisec / 1000);
	milisec = milisec - (sec * 1000);
	timespec_t req = { 0 };
	req.tv_sec = sec;
	req.tv_nsec = milisec * 1000000L;
	if (nanosleep(&req, &req))
		ERR("nanosleep");
}

void *pigWork(void *args){
    // int * arg = args;
    // wallHeight(args, pthread_self());
}

int getMax(int *args){
    int length = sizeof(args) / sizeof(int);
    int max = args[0];
    for(int i = 1; i < length; i++){
        if(args[i] > max)
            max = args[i];
    }
    return max;
}

void wolfWork(int *args){
    int b = rand() % getMax(args) + 1;
    int length = sizeof(args) / sizeof(int);
    
    msleep(rand() % 500 + 100);
    for(int i = 0; i < length; i++) {
        if(args[i] <= b)
            args[i] = 0;
    }
}

void wallHeight(int *args, int index) {
    msleep(rand() % 51 + 10);
    args[index]++;
}

