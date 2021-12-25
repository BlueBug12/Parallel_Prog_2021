#include <stdio.h>
#include <stdlib.h>
#include "hostFE.h"
#include "helper.h"

void hostFE(int filterWidth, float *filter, int imageHeight, int imageWidth,
            float *inputImage, float *outputImage, cl_device_id *device,
            cl_context *context, cl_program *program)
{
    cl_int status;
    size_t img_size = sizeof(float)*imageHeight*imageWidth;
    size_t filter_size = sizeof(float)*filterWidth*filterWidth;

    cl_command_queue cq = clCreateCommandQueue(*context,*device,0,&status);
    
    cl_mem c_input = clCreateBuffer(*context,CL_MEM_READ_ONLY,img_size,NULL,&status);
    cl_mem c_output = clCreateBuffer(*context,CL_MEM_WRITE_ONLY,img_size,NULL,&status);
    cl_mem c_filter = clCreateBuffer(*context,CL_MEM_READ_ONLY,filter_size,NULL,&status);
    
    status = clEnqueueWriteBuffer(cq,c_input,CL_TRUE,0,img_size,(void *)inputImage,0,NULL,NULL);
    status = clEnqueueWriteBuffer(cq,c_filter,CL_TRUE,0,filter_size,(void *)filter,0,NULL,NULL);

    //status = clBuildProgram(*program,1,*device,NULL,NULL,NULL);
    cl_kernel c_kernel = clCreateKernel(*program,"convolution",&status);
    clSetKernelArg(c_kernel,0,sizeof(cl_mem),(void *)&c_input);
    clSetKernelArg(c_kernel,1,sizeof(cl_mem),(void *)&c_output);
    clSetKernelArg(c_kernel,2,sizeof(cl_mem),(void *)&c_filter);
    clSetKernelArg(c_kernel,3,sizeof(int),(void *)&imageWidth);
    clSetKernelArg(c_kernel,4,sizeof(int),(void *)&imageHeight);
    clSetKernelArg(c_kernel,5,sizeof(int),(void *)&filterWidth);

    size_t globalws[2] = {imageWidth,imageHeight};
    status = clEnqueueNDRangeKernel(cq,c_kernel,2,0,globalws,NULL,0,NULL,NULL);
    clFinish(cq);
    status = clEnqueueReadBuffer(cq,c_output,CL_TRUE,0,img_size,(void *)outputImage,0,NULL,NULL);
}
