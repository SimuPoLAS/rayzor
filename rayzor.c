#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    int x, y ,z;
} Index3;

typedef struct {
    Vec3 o, d;
} Ray;

int main() {
    FILE* log = fopen("rayzor.log", "a");

    {
        time_t current;
        time(&current);
        fprintf(log, "rayzor session started at %s\n", ctime(&current));
    }

    int numPoints = 0;
    scanf("%i\n", &numPoints);
    fprintf(log, "number of points read: %i\n", numPoints);

    Vec3* points = malloc(sizeof(Vec3) * numPoints);
    for (int i = 0; i < numPoints; i++) {
        if (scanf("%f %f %f\n", &points[i].x, &points[i].y, &points[i].z) != 3) {
            fprintf(stderr, "bad input, closing\n");
            fprintf(log, "bad input, closing\n");
            return 1;
        }
        fprintf(log, "point read: %f %f %f\n", points[i].x, points[i].y, points[i].z);
    }

    int numTriangles = 0;
    scanf("%i\n", &numTriangles);
    fprintf(log, "number of Triangles read: %i\n", numTriangles);

    Index3* triangles = malloc(sizeof(Index3) * numTriangles);
    for (int i = 0; i < numTriangles; i++) {
        if (scanf("%i %i %i\n", &triangles[i].x, &triangles[i].y, &triangles[i].z) != 3) {
            fprintf(stderr, "bad input, closing\n");
            fprintf(log, "bad input, closing\n");
            return 1;
        }
        fprintf(log, "triangle read: %i %i %i\n", triangles[i].x, triangles[i].y, triangles[i].z);
    }

    int numRays = 0;
    scanf("%i\n", &numRays);
    fprintf(log, "number of rays read: %i\n", numRays);

    Ray* rays = malloc(sizeof(Ray) * numRays);
    for (int i = 0; i < numRays; i++) {
        if (scanf("%f %f %f %f %f %f\n", &rays[i].o.x, &rays[i].o.y, &rays[i].o.z, &rays[i].d.x, &rays[i].d.y, &rays[i].d.z) != 6) {
            fprintf(stderr, "bad input, closing\n");
            fprintf(log, "bad input, closing\n");
            return 1;
        }
        fprintf(log, "ray read: %f %f %f %f %f %f\n", rays[i].o.x, rays[i].o.y, rays[i].o.z, rays[i].d.x, rays[i].d.y, rays[i].d.z);
    }

    // Create the two input vectors
    int i;
    const int LIST_SIZE = 1024;
    int *A = (int*)malloc(sizeof(int)*LIST_SIZE);
    int *B = (int*)malloc(sizeof(int)*LIST_SIZE);
    for(i = 0; i < LIST_SIZE; i++) {
        A[i] = i;
        B[i] = LIST_SIZE - i;
    }

    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("vector_add_kernel.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;   
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_ALL, 1, 
            &device_id, &ret_num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Create memory buffers on the device for each vector 
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
            LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
            LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
            LIST_SIZE * sizeof(int), NULL, &ret);

    // Copy the lists A and B to their respective memory buffers
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
            LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, 
            LIST_SIZE * sizeof(int), B, 0, NULL, NULL);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, 
            (const char **)&source_str, (const size_t *)&source_size, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);
    
    // Execute the OpenCL kernel on the list
    size_t global_item_size = LIST_SIZE; // Process the entire lists
    size_t local_item_size = 64; // Process in groups of 64
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
            &global_item_size, &local_item_size, 0, NULL, NULL);

    // Read the memory buffer C on the device to the local variable C
    int *C = (int*)malloc(sizeof(int)*LIST_SIZE);
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, 
            LIST_SIZE * sizeof(int), C, 0, NULL, NULL);

    // Display the result to the screen
    for(i = 0; i < LIST_SIZE; i++)
        printf("%d + %d = %d\n", A[i], B[i], C[i]);

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseMemObject(b_mem_obj);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(A);
    free(B);
    free(C);

    free(rays);
    free(triangles);
    free(points);

    fprintf(log, "ending session\n");
    fclose(log);
    return 0;
}
