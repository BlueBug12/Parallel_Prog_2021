#include <mpi.h>
#include <iostream>


void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr, int **a_mat_ptr, int **b_mat_ptr){
    int rank;
    int size;
    int n,m,l;
    int size_info[3]; //n,m,l

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    std::ios_base::sync_with_stdio(false);

    if(rank==0){
        std::cin >> size_info[0] >> size_info[1] >> size_info[2];
    }

    MPI_Bcast(size_info,3,MPI_INT,0,MPI_COMM_WORLD);
    n = *n_ptr = size_info[0];
    m = *m_ptr = size_info[1];
    l = *l_ptr = size_info[2];
    

    if(rank==0){
        a_mat_ptr = (int **)malloc(n*sizeof(int *));
        for(int i=0;i<n;++i){
            a_mat_ptr[i] = (int *)malloc(m*sizeof(int));
            for(int j=0;j<m;++j){
                std::cin>>a_mat_ptr[i][j];
            }
        }
        //record the transpose form for cache optimization
        b_mat_ptr = (int **)malloc(l*sizeof(int *));
        for(int i=0;i<l;++i){
            b_mat_ptr[i] = (int *)malloc(m*sizeof(int));
        }
        for(int i=0;i<m;++i){
            for(int j=0;j<l;++j){
                std::cin >> b_mat_ptr[j][i];
            }
        }
        for(int i=0;i<n;++i){
            for(int j=0;j<m;++j){
                std::cout<<a_mat_ptr[i][j]<<" ";  
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;

        for(int i=0;i<l;++i){
            for(int j=0;j<m;++j){
                std::cout<<b_mat_ptr[i][j]<<" ";
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
    }
    
    

    
    


    

    
}


void matrix_multiply(const int n, const int m, const int l, const int *a_mat, const int *b_mat){
    
}

void destruct_matrices(int *a_mat, int *b_mat){

}

