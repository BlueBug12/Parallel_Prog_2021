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

    // TODO: init MPI
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
    if (world_rank > 0)//send the message to rank 0 process
    {
        MPI_Send(&sum,1,MPI_LONG,0,0,MPI_COMM_WORLD);
    }
    else if (world_rank == 0)//receive message from other processes
    {
        for(int source=1;source<world_size;++source){
            MPI_Recv(&sum,1,MPI_LONG,source,0,MPI_COMM_WORLD,&status);
            total_sum += sum;
        }
    }

    if (world_rank == 0)
    {
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
