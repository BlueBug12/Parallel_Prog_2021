#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
   int thread_id;
   int iteration;
} Arg;

//long long int circle[16];
long long int total_sum = 0;
pthread_mutex_t mutex;

void *countPi(void *arg){
    Arg *data = (Arg *)arg;
    int iter = data->iteration;
    int id = data->thread_id;
    unsigned int seed = id;
    long long int sum = 0;
    while(iter>0){
        double x = (double)rand_r(&seed)/RAND_MAX ;//[0,1]
        double y = (double)rand_r(&seed)/RAND_MAX ;//[0,1]
        if(x*x + y*y <= 1.0){
            ++sum;
        }
        iter--;
    }

    pthread_mutex_lock(&mutex);
    total_sum += sum;
    pthread_mutex_unlock(&mutex);

    //std::cout<<"Thread "<<id<<" finish. circle = "<<sum<<std::endl;
    pthread_exit((void *)0);
}

int main(int argc, char *argv[]){
   if(argc != 3){
      std::cerr<<"Error: the program requires 3 arguments."<<std::endl;
      exit(1);
   }

   long long int total_iter = std::stoi(argv[2]);
   int thread_num = std::stoi(argv[1]);
   long long int counter = total_iter/thread_num;

   Arg arg[thread_num];

   pthread_t all_threads[thread_num];

   pthread_attr_t attr;
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   pthread_mutex_init(&mutex, NULL);

   for(int i=0;i<thread_num;++i){
       arg[i].thread_id = i;
       arg[i].iteration = counter;
       pthread_create(&all_threads[i],&attr,countPi,(void *)&arg[i]);
   }
   pthread_attr_destroy(&attr);

   void *status;
   for(int i=0;i<thread_num;++i){
      pthread_join(all_threads[i],&status);
   }
   std::cout<<"Final result: pi = "<<4*(long double)total_sum/total_iter<<std::endl;
   pthread_exit(NULL);
}