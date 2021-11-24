mpicxx ./hello.cc -o mpi_hello
mpirun -np 8 --hostfile hosts mpi_hello
