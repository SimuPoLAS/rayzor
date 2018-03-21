typedef struct __attribute__ ((packed)) _Vec3 {
    float x, y, z;
} Vec3;

typedef struct __attribute__ ((packed)) _Index3 {
    int x, y ,z;
} Index3;

typedef struct __attribute__ ((packed)) _Ray {
    Vec3 o, d;
} Ray;

__kernel void collision(__global Ray *rays, __global Vec3 *points, __global Index3 *triangles, __global int *collisions) {
    // Get the index of the current element
    int i = get_global_id(0);

    // Do the operation
    collisions[i] = i;
}
