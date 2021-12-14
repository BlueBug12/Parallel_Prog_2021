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

__global__ void mandelKernel(float stepX, float stepY, float lowerX, float lowerY, int* output_device, int resX, int resY, int maxIterations) {
    // To avoid error caused by the floating number, use the following pseudo code
    //
    // float x = lowerX + thisX * stepX;
    // float y = lowerY + thisY * stepY;
    int i = blockIdx.x*blockDim.x+threadIdx.x;
    int j = blockIdx.y*blockDim.y+threadIdx.y;
    if(i>=resX||j>=resY)
        return;
    float x = lowerX + i*stepX;
    float y = lowerY + j*stepY;
    int index = j*resX + i;
    output_device[index] = mandel(x,y,maxIterations);
}

// Host front-end function that allocates the memory and launches the GPU kernel
void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{
    float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;
    int size = resX*resY*sizeof(int);
    int *output_host = (int *)malloc(size);
    int *output_device;
    cudaMalloc(&output_device,size);
    float block_dim_x = 16;
    float block_dim_y = 16;
    dim3 threadsPerBlock(block_dim_x,block_dim_y);
    dim3 numBlocks((int)ceil(resX/block_dim_x),(int)ceil(resY/block_dim_y));
    mandelKernel<<<numBlocks,threadsPerBlock>>>(stepX,stepY,lowerX,lowerY,output_device,resX,resY,maxIterations);

    cudaMemcpy(output_host,output_device,size,cudaMemcpyDeviceToHost);
    memcpy(img,output_host,size);
    free(output_host);
    cudaFree(output_device);
}
