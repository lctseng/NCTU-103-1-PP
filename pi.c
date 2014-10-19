#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#ifndef MAX_THREAD
#define MAX_THREAD 15
#endif
#define RAND_UPBOUND 25000
#define HALF_BOUND (RAND_UPBOUND/2)
#define randGen() ((long double)(Rand()%RAND_UPBOUND) / HALF_BOUND - 1.0)
#define SHOW_TIME

long part_in[MAX_THREAD];  
long total_in;

pthread_mutex_t mutex;
long number_of_tosses;

inline int Rand(){
    int val;
    //pthread_mutex_lock(&mutex);
    val = 5656*4145;//rand();
    //pthread_mutex_unlock(&mutex);
    return val;
}

void* go_rand(void* p_rank){
    long rank = (long)p_rank;
    //clock_t start_time = clock();
    //printf("Thread %d , count :%d\n",rank,number_of_tosses);
    long number_in_circle = 0;
    long toss;
    long double distance_squared,x,y;
    long double result = 0.0;   
    for ( toss = 0; toss < number_of_tosses ; toss ++) {
        //pthread_mutex_lock(&mutex);
        x = randGen();
        y = randGen();
        //pthread_mutex_unlock(&mutex);
        distance_squared = x*x + y*y;
        if ( distance_squared <= 1)
            number_in_circle ++;
    }
    //part_in[rank] = number_in_circle; 
    //clock_t end_time = clock();
    //printf("Thread %d Done in %lf\n",rank,(double)(end_time - start_time) / CLOCKS_PER_SEC);
    pthread_mutex_lock(&mutex);
    total_in += number_in_circle;
    pthread_mutex_unlock(&mutex);
    return NULL;    
}

long double calculatePI(long total_tosses){
    long sum_in = total_in;
    /*long i;
    for(i=0;i<MAX_THREAD;i++){
        sum_in += part_in[i];
    } 
    */

    return 4.0*(long double)sum_in / total_tosses;
    
}

int main(int argc,char** argv){
    long total_in = 0;
    //clock_t start_time = clock();
    long total_tosses = atoi(argv[1]); 
    number_of_tosses = total_tosses / MAX_THREAD + 1;
    const long thread_count = MAX_THREAD;
    pthread_t* threads_ptr = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
 
    long i = 0;
    pthread_mutex_init(&mutex, NULL); 
    for(i=0;i<thread_count;i++){
        pthread_create(&threads_ptr[i],NULL,go_rand,(void*) i);

    }


    for(i=0;i<thread_count;i++){

        pthread_join(threads_ptr[i],NULL);
    }
    pthread_mutex_destroy(&mutex);
    free(threads_ptr);

    printf("%llf\n",calculatePI(total_tosses)); 

    //clock_t end_time = clock();
#ifdef SHOW_TIME

    //printf("Execution Time: %lf s\n",(double)(end_time - start_time) / CLOCKS_PER_SEC);
#endif
    return 0;

}

