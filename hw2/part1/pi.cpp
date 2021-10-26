#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
   int thread_id;
   int iteration;
} Arg;

long long int circle[16];

void *countPi(void *arg){
    Arg *data = (Arg *)arg;
    int iter = data->iteration;
    int id = data->thread_id;
    unsigned int seed = id;
    while(iter>0){
        double x = (double)rand_r(&seed)/RAND_MAX ;//[0,1]
        double y = (double)rand_r(&seed)/RAND_MAX ;//[0,1]
        if(x*x + y*y <= 1.0){
            ++circle[id];
        }
        iter--;
    }
    //std::cout<<"Thread "<<id<<" finish. circle = "<<circle[id]<<std::endl;
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

   for(int i=0;i<thread_num;++i){
       arg[i].thread_id = i;
       arg[i].iteration = counter;
       circle[i]=0;
       pthread_create(&all_threads[i],&attr,countPi,(void *)&arg[i]);
   }
   pthread_attr_destroy(&attr);

   void *status;
   long long int circle_sum = 0;
   for(int i=0;i<thread_num;++i){
      pthread_join(all_threads[i],&status);
      circle_sum += circle[i];
   }
   std::cout<<"Final result: pi = "<<4*(long double)circle_sum/total_iter<<std::endl;
   pthread_exit(NULL);
}