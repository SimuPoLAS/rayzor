#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)

typedef struct __attribute__ ((packed)) _Vec3 {
    float x, y, z;
} Vec3;

typedef struct __attribute__ ((packed)) _Index3 {
    int x, y ,z;
} Index3;

typedef struct __attribute__ ((packed)) _Ray {
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

    // load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("collision.cl", "r");
    if (!fp) {
        fprintf(stderr, "failed to load kernel.\n");
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
    cl_mem rays_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
            numRays * sizeof(Ray), NULL, &ret);
    cl_mem points_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
            numPoints * sizeof(Vec3), NULL, &ret);
    cl_mem triangles_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
            numTriangles * sizeof(Index3), NULL, &ret);
    cl_mem collisions_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
            numRays * sizeof(int), NULL, &ret);

    // Copy the lists rays, points and triangles to their respective memory buffers
    ret = clEnqueueWriteBuffer(command_queue, rays_mem_obj, CL_TRUE, 0,
            numRays * sizeof(Ray), rays, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, points_mem_obj, CL_TRUE, 0,
            numPoints * sizeof(Vec3), points, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, triangles_mem_obj, CL_TRUE, 0,
            numTriangles * sizeof(Index3), triangles, 0, NULL, NULL);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1,
            (const char **)&source_str, (const size_t *)&source_size, &ret);

    printf("%i AAAA kernel\n", ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    if (ret != CL_SUCCESS) {
        printf("%i AAAA build\n", ret);
        size_t len;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, NULL, NULL, &len);
        char *message = (char*)malloc(sizeof(char) * len);
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, len, message, NULL);
        fprintf(log, "CL kernel build log output:\n%s\n", message);
    }

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "collision", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&rays_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&points_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&triangles_mem_obj);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&collisions_mem_obj);
    
    // Execute the OpenCL kernel on the list
    size_t global_item_size[] = { numRays, 0, 0 }; // Process the entire lists
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
            global_item_size, NULL, 0, NULL, NULL);

    // Read the memory buffer C on the device to the local variable C
    int *collisions = (int*)malloc(sizeof(int) * numRays);
    ret = clEnqueueReadBuffer(command_queue, collisions_mem_obj, CL_TRUE, 0, 
            numRays * sizeof(int), collisions, 0, NULL, NULL);

    if (ret != CL_SUCCESS) {
        printf("ERROR AAAAAAAA %i\n", ret);
        exit(ret);
    }

    // Display the result to the screen
    for(int i = 0; i < numRays; i++)
        printf("ray %i has following collision: %d\n", i, collisions[i]);

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(rays_mem_obj);
    ret = clReleaseMemObject(points_mem_obj);
    ret = clReleaseMemObject(triangles_mem_obj);
    ret = clReleaseMemObject(collisions_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    free(rays);
    free(triangles);
    free(points);
    free(collisions);

    fprintf(log, "ending session\n");
    fclose(log);
    return 0;
}
