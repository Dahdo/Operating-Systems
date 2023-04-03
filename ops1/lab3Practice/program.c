#include <math.h>
#include <pthread.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_THREADCOUNT 9
#define DEFAULT_ARRAY_SIZE 13

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

void readArguments(int argc, char **argv, int *threadCount, int *arraysize);
void *argFunction(void *args);

typedef struct squareThread
{
    pthread_t tid;
    int arraySize;
} squareThread_t;

typedef struct arrayArg
{
    double *array;
    pthread_mutex_t mxArray;
    int arraySize;
} array_arg_t;

int main(int argc, char **argv)
{
    int threadCount, arraySize;
    array_arg_t arrayArg;
    readArguments(argc, argv, &threadCount, &arraySize);
    double *array = (double *)malloc(sizeof(double) * arraySize);
    if (array == NULL)
        ERR("null array");
    arrayArg.array = array;
    arrayArg.arraySize = arraySize;

    squareThread_t *threads = (squareThread_t *)malloc(sizeof(squareThread_t) * threadCount);
    if (threads == NULL)
        ERR("erro allocating threads");

    pthread_mutex_t mxArray = PTHREAD_MUTEX_INITIALIZER;
    arrayArg.mxArray = mxArray;

    for (int i = 0; i < arraySize; i++)
    {
        int a = rand() % 61 + 1;
        array[i] = (double)rand() / (double)(RAND_MAX / a);
    }
    int count = 0;
    while (count < arraySize)
    {
        printf("%f\n", array[count]);
        count++;
    }

    for (int i = 0; i < threadCount; i++)
    {
        int err = pthread_create(&(threads[i].tid), NULL, argFunction, &arrayArg);
        if (err != 0)
            ERR("erro creating");
    }

    for (int i = 0; i < threadCount; i++)
    {
        int err = pthread_join(threads[i].tid, NULL);
        if (err != 0)
            ERR("erro joining");
    }

    for(int i = 0; i < a)
    {
        printf("%f\n", array[count2]);
        count2++;
    }

    free(threads);
}

void readArguments(int argc, char **argv, int *threadCount, int *arraysize)
{
    *threadCount = DEFAULT_THREADCOUNT;
    *arraysize = DEFAULT_ARRAY_SIZE;

    if (argc >= 2)
    {
        *threadCount = atoi(argv[1]);
        if (*threadCount <= 0)
        {
            printf("Invalid value for 'threadCount'");
            exit(EXIT_FAILURE);
        }
    }
    if (argc >= 3)
    {
        *arraysize = atoi(argv[2]);
        if (*arraysize <= 0)
        {
            printf("Invalid value for 'samplesCount'");
            exit(EXIT_FAILURE);
        }
    }
}

void *argFunction(void *args)
{
    array_arg_t *arg = args;

    pthread_mutex_lock(&(arg->mxArray));
    srand(time(NULL));
    int index = rand() % arg->arraySize;
    printf("[%d] ", index);
    double result = arg->array[index] * arg->array[index];
    arg->array[index] = result;
    printf("%f\n", result);
    pthread_mutex_unlock(&(arg->mxArray));
    exit(0);
}