#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    // TODO: MPI init
    MPI_Status status;
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
    long long int total_sum = sum;

    
    int n = log2(world_size);
    int pos = 1;
    // TODO: binary tree redunction
    for(int i= 1 ;i <= n;++i){
        pos = pos << 1;
        int pre_pos = pos >> 1;
        if(world_rank % pos == pos >> 1){
            MPI_Send(&sum,1,MPI_LONG,world_rank-pre_pos,0,MPI_COMM_WORLD);
            //printf("rank %d send sum to rank %d\n",world_rank,world_rank-pre_pos);
        }else if(world_rank % pos == 0){
            MPI_Recv(&sum,1,MPI_LONG,world_rank+pre_pos,0,MPI_COMM_WORLD,&status);
            //printf("rank %d receive sum from rank %d\n",world_rank,world_rank+pre_pos);
            total_sum += sum;
        }
        sum = total_sum;
    }

    if (world_rank == 0)
    {
        // TODO: PI result
        pi_result = 4*(double)total_sum/tosses;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
