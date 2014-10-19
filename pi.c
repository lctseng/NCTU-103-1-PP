#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include <semaphore.h>
#ifndef MAX_THREAD
#define MAX_THREAD 15
#endif
#define MAX_FREE_THREAD (MAX_THREAD-1)
#define RAND_UPBOUND 25000
#define HALF_BOUND (RAND_UPBOUND/2)
#define randGen(n) ((double)((double)((n%RAND_UPBOUND)) / HALF_BOUND - 1.0))
/*
inline double randGen(int n){
    int m = n%RAND_UPBOUND;
    printf("M:%d\n",m);
    
    double result =  m;
    printf("R:%lf\n",(double)result);
    return result;
}*/

long part_in[MAX_THREAD];  
long total_tosses;
struct table_entry{
    int v1,v2;
};
typedef struct table_entry ENTRY;
ENTRY** table;
long* table_indexes;
sem_t* sem_ptr;
pthread_mutex_t mutex;
long number_of_tosses;

inline int Rand(){
    int val;
    //pthread_mutex_lock(&mutex);
    val = rand();
    //pthread_mutex_unlock(&mutex);
    return val;
}

void* gen_rnd(){
    long i,j;
    for(i=0;i<number_of_tosses;i++){
        for(j=0;j<MAX_FREE_THREAD;j++){
            table[j][i].v1 = Rand();
            table[j][i].v2 = Rand();
            //printf("Rank %d OK!\n",j);
            sem_post(&sem_ptr[j]);
       }
    }


    return NULL;
}

void* go_rand(void* p_rank){
    long rank = (long)p_rank - 1;
    //printf("Thread %d\n",(long)p_rank);
    long number_in_circle = 0;
    long toss;
    double distance_squared,x,y;
    double result = 0.0;   
    ENTRY* e_ptr;
    for ( toss = 0; toss < number_of_tosses ; toss ++) {
        //printf("Thread %d Wait for...\n",rank+1);
        sem_wait(&sem_ptr[rank]);
        e_ptr = &table[rank][table_indexes[rank]++];
        //printf("v1:%d,v2:%d\n",e_ptr->v1,e_ptr->v2);
        x = randGen(e_ptr->v1);
        y = randGen(e_ptr->v2);
        //printf("Thread %d Got %lf , %llf !\n",rank+1,x,y);
        distance_squared = x*x + y*y;
        if ( distance_squared <= 1)
            number_in_circle ++;
    }
    part_in[rank] = number_in_circle; 
    return NULL;    
}

double calculatePI(long total_tosses){
    long sum_in = 0;
    long i;
    for(i=0;i<MAX_THREAD;i++){
        sum_in += part_in[i];
    } 
    

    return 4.0*(double)sum_in / total_tosses;
    
}

void other_init(){    
    sem_ptr = malloc(MAX_FREE_THREAD*sizeof(sem_t));
    long i;
    table = malloc(MAX_FREE_THREAD*sizeof(ENTRY*));
    table_indexes = malloc(MAX_FREE_THREAD*sizeof(long));
    for(i=0;i<MAX_FREE_THREAD;i++){
        table[i] = malloc(number_of_tosses*sizeof(ENTRY));
        sem_init(&sem_ptr[i], 0, 0);
        table_indexes[i] = 0;
    }
}

void other_end(){
    long i;
    free(sem_ptr);   
    for(i=0;i<MAX_FREE_THREAD;i++){
        sem_destroy(&sem_ptr[i]);
        free(table[i]);
    }
    free(table_indexes);
    free(table);
}


int main(int argc,char** argv){
    srand(time(0));
    if(MAX_THREAD <= 1)
        return -1;
    total_tosses = atoi(argv[1]); 
    number_of_tosses = total_tosses / MAX_FREE_THREAD ;
    const long thread_count = MAX_THREAD;
    pthread_t* threads_ptr = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
 
    long i = 0;
    pthread_mutex_init(&mutex, NULL); 
    other_init();
    pthread_create(&threads_ptr[0],NULL,gen_rnd,(void*) i);
    for(i=1;i<thread_count;i++){
        pthread_create(&threads_ptr[i],NULL,go_rand,(void*) i);

    }
    


    for(i=0;i<thread_count;i++){

        pthread_join(threads_ptr[i],NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    free(threads_ptr);
    other_end();
    printf("%lf\n",calculatePI(total_tosses)); 

    return 0;

}

