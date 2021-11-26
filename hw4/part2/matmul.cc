#include <mpi.h>
#include <iostream>


void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr, int **a_mat_ptr, int **b_mat_ptr){
    int rank;
    int size;
    int n,m,l;
    int *a_m;
    int *b_m;
    int size_info[3]; //n,m,l

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    std::ios_base::sync_with_stdio(false);

    if(rank==0){
        std::cin >> size_info[0] >> size_info[1] >> size_info[2];
    }

    //brocast the value of n,m,l
    MPI_Bcast(size_info,3,MPI_INT,0,MPI_COMM_WORLD);
    n = *n_ptr = size_info[0];
    m = *m_ptr = size_info[1];
    l = *l_ptr = size_info[2];
    int batch = n/size;
    //deal with corner case
    if(rank == size-1){
        batch = n - batch*(size-1);
    }
    
    if(rank == 0){
        int a_len = n*m;
        int b_len = m*l;
        a_m = *a_mat_ptr = (int *)malloc(a_len*sizeof(int));
        b_m = *b_mat_ptr = (int *)malloc(b_len*sizeof(int));

        for(int i=0;i<a_len;++i){
            std::cin>>a_m[i];
        }
        //record the transpose form for cache optimization
        for(int i=0;i<m;++i){
            for(int j=0;j<l;++j){
                std::cin >> b_m[j*m+i];
            }
        }
    }else{
        *a_mat_ptr = (int *)malloc(batch*m*sizeof(int));
        *b_mat_ptr = (int *)malloc(m*l*sizeof(int));
    }

    //brocast the whole matrix b(transposed)
    MPI_Bcast(*b_mat_ptr,m*l,MPI_INT,0,MPI_COMM_WORLD);
    

    if(rank == 0){
        for(int i=1;i<size-1;++i){
            MPI_Send(a_m + i*batch*m ,batch*m,MPI_INT,i,0,MPI_COMM_WORLD);
        }
        MPI_Send(a_m + (size-1)*batch*m ,(n-batch*(size-1))*m,MPI_INT,size-1,0,MPI_COMM_WORLD);
    }else{
        MPI_Recv(*a_mat_ptr,batch*m,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
}


void matrix_multiply(const int n, const int m, const int l, const int *a_mat, const int *b_mat){
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int batch = n/size;
    //deal with corner case
    if(rank == size-1){
        batch = n - batch*(size-1);
    }

    int *sol;
    if(rank == 0){
        sol = (int *)calloc(n*l,sizeof(int));
    }else{
        sol = (int *)calloc(l*batch,sizeof(int));
    }
    int tsize = 8;
    for(int i=0;i<batch;i+=tsize){
        for(int j=0;j<l;j+=tsize){
            for(int k=0;k<m;k+=tsize){
                for(int t_i=0;t_i<tsize&&i+t_i<batch;++t_i){
                    const int t1_row = t_i+i;
                    const int a_base = t1_row*m + k;
                    for(int t_j=0;t_j<tsize&&j+t_j<l;++t_j){
                        const int t2_row = t_j+j;
                        const int b_base = t2_row*m + k;
                        const int sol_index = t1_row*l + t2_row;
                        for(int t_k=0;t_k<tsize&&k+t_k<m;++t_k){
                            sol[sol_index]+=a_mat[a_base+t_k]*b_mat[b_base+t_k];
                        }
                    }
                }
            }
        }
    }

    /*
    for(int i=0;i<batch;++i){//row index of ma
        const int sol_base = i*l;
        const int a_base = i*m;
        for(int j=0;j<l;++j){//row index of mb
            const int b_base = j*m;
            const int sol_i = sol_base + j;
            for(int k=0;k<m;++k){//col index for both
                sol[sol_i] += a_mat[a_base+k]*b_mat[b_base+k];
            }
        }
    }*/

    if(rank == 0){
        for(int i=1;i<size-1;++i){
            MPI_Recv(sol+batch*i*l,batch*l,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
        MPI_Recv(sol+(size-1)*batch*l,(n-(size-1)*batch)*l,MPI_INT,size-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }else{
        MPI_Send(sol,batch*l,MPI_INT,0,0,MPI_COMM_WORLD);
    }
    

    if(rank == 0){
        for(int i=0 ; i<n ; ++i){
            for(int j=0 ; j<l ; ++j){
                std::cout << sol[i*l+j] << " ";
            }
            std::cout<<std::endl;
        }
    }
    free(sol);
}

void destruct_matrices(int *a_mat, int *b_mat){
    free(a_mat);
    free(b_mat);
}

