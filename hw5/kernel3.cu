#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

__device__ int mandel(float c_re, float c_im, int count){
    float z_re = c_re, z_im = c_im;
    int i;
    for (i = 0; i < count; ++i)
    {
        if (z_re * z_re + z_im * z_im > 4.f)
          break;
        float new_re = z_re * z_re - z_im * z_im;
        float new_im = 2.f * z_re * z_im;
        z_re = c_re + new_re;
        z_im = c_im + new_im;
    }
    return i;
}

__global__ void mandelKernel(float stepX, float stepY, float lowerX, float lowerY, int* output_device, int resX, int resY, int maxIterations, size_t pitch, int shift_x) {
    // To avoid error caused by the floating number, use the following pseudo code
    //
    // float x = lowerX + thisX * stepX;
    // float y = lowerY + thisY * stepY;
    int i = blockIdx.x*blockDim.x+threadIdx.x;
    int j = blockIdx.y*blockDim.y+threadIdx.y;
    if(j>=resY)
        return;
    float y = lowerY + j*stepY;
    
    for(int k=0;k<shift_x;++k){
        int s_i = i*shift_x+k;
        if(s_i>=resX)
            return;
        float x = lowerX + s_i*stepX;
        int index = j*pitch + s_i;
        output_device[index] = mandel(x,y,maxIterations);
    }
}

// Host front-end function that allocates the memory and launches the GPU kernel
void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{
    float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;
    int *output_host; 
    int *output_device;
    size_t pitch;
    
    cudaMallocPitch(&output_device,&pitch,sizeof(int)*resX,resY);
    cudaHostAlloc(&output_host,resY*pitch,cudaHostAllocMapped);
    
    int shift_x = 2;
    float block_dim_x = 16;
    float block_dim_y = 16;

    dim3 threadsPerBlock(block_dim_x,block_dim_y);
    dim3 numBlocks((int)(ceil(resX/block_dim_x/shift_x)),(int)ceil(resY/block_dim_y));
    mandelKernel<<<numBlocks,threadsPerBlock>>>(stepX,stepY,lowerX,lowerY,output_device,resX,resY,maxIterations,pitch/(sizeof(int)),shift_x);

    cudaMemcpy(output_host,output_device,pitch*resY,cudaMemcpyDeviceToHost);
    pitch/=sizeof(int);
    
    for(int i=0;i<resY;++i){
        for(int j=0;j<resX;++j){
            img[i*resX+j] = output_host[i*pitch+j];
        }
    }
    cudaFreeHost(output_host);
    cudaFree(output_device);
}
