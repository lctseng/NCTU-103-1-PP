#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<limits.h>
#include<time.h>
// for test, specific number of threads
#ifndef MAX_THREAD
#define MAX_THREAD 15
#endif
// for random function
#define RAND_UPBOUND 32768
#define HALF_BOUND (RAND_UPBOUND/2)
// convert integer random value to floating type
#define randGen(seed) ((double)(Rand(seed)%RAND_UPBOUND) / HALF_BOUND - 1.0)

// define large integer type to store number of tosses
typedef long long int toss_t;
// record total toss-in number
toss_t total_in;


// random seeds for each threads
int seed_table[MAX_THREAD];

// mutex 
pthread_mutex_t mutex;
toss_t number_of_tosses;

// random number generator, need a seed number pass by pointer
inline int Rand(int* seed_ptr){
   *seed_ptr = *seed_ptr * 1103515245 + 12345;
   return (unsigned int)(*seed_ptr/65536) % RAND_UPBOUND ;
}

// each thread will run this function
void* go_rand(void* p_rank){
    // initialize
    long rank = (long)p_rank;
    toss_t number_in_circle = 0;
    toss_t toss;
    double distance_squared,x,y;
    double result = 0.0;   
    // fetch the seed and create pointer
    int seed = seed_table[rank];
    int* seed_ptr = &seed;
    

    for ( toss = 0; toss < number_of_tosses ; toss ++) {
        // generate random coordinate
        x = randGen(seed_ptr);
        y = randGen(seed_ptr);
        distance_squared = x*x + y*y;
        if ( distance_squared <= 1)
            number_in_circle ++;
    }
    // after toss, accumulate into sum, use a mutex 
    pthread_mutex_lock(&mutex);
    total_in += number_in_circle;
    pthread_mutex_unlock(&mutex);
    return NULL;    
}

// calculate PI by total_in and total_tosses
double calculatePI(toss_t total_tosses){
    toss_t sum_in = total_in;

    return 4.0*(double)sum_in / total_tosses;
    
}

int main(int argc,char** argv){
    toss_t total_in = 0;
    // random seed
    srand(time(0));
    // arrange workload for each thread
    toss_t total_tosses = atoll(argv[1]); 
    number_of_tosses = total_tosses / MAX_THREAD + 1;
    const long thread_count = MAX_THREAD;
    pthread_t* threads_ptr = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
 
    long i = 0;
    pthread_mutex_init(&mutex, NULL); 
    for(i=0;i<thread_count;i++){
        // generate seeds for each thread
        seed_table[i] = ((unsigned)rand() * i)%INT_MAX;
        pthread_create(&threads_ptr[i],NULL,go_rand,(void*) i);
    }


    for(i=0;i<thread_count;i++){

        pthread_join(threads_ptr[i],NULL);
    }
    pthread_mutex_destroy(&mutex);
    free(threads_ptr);

    printf("%lf\n",calculatePI(total_tosses)); 

    return 0;

}

