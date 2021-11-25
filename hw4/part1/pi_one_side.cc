#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    MPI_Win win;

    // TODO: MPI init
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);    
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);    
    long long int iteration = tosses/world_size;

    double x,y;
    long long int sum = 0;
    unsigned int seed = world_rank;
    for(;iteration>0;--iteration){
        x = (double)rand_r(&seed)/RAND_MAX ;
        y = (double)rand_r(&seed)/RAND_MAX;
        if(x*x + y*y <= 1.0)
            ++sum;
    }

    long long int *s;
    if (world_rank == 0)
    {
        // Master
        s = (long long int *)malloc(sizeof(long long int)*world_size);
        MPI_Alloc_mem(world_size*sizeof(long long int),MPI_INFO_NULL,&s);
        memset(s,0,world_size);
        s[0] = sum;
        MPI_Win_create(s,world_size*sizeof(long long int),sizeof(long long int),MPI_INFO_NULL,MPI_COMM_WORLD, &win);
    }
    else
    {
        // Workers
        MPI_Win_create(nullptr,0,1,MPI_INFO_NULL, MPI_COMM_WORLD,&win);
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0,0,win);
        MPI_Put(&sum,1,MPI_LONG_LONG,0,world_rank,1,MPI_LONG_LONG,win);
        MPI_Win_unlock(0,win);
    }

    MPI_Win_free(&win);

    if (world_rank == 0)
    {
        // TODO: handle PI result

        long long int total_sum;
        for(int i=0;i<world_size;++i){
            total_sum+=s[i];
        }
        
        pi_result = 4*(double)total_sum/tosses;
        MPI_Free_mem(s);
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    MPI_Finalize();
    return 0;
}
