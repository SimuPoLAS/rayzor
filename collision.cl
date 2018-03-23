typedef struct __attribute__ ((packed)) _Vec3 {
    float x, y, z;
} Vec3;

typedef struct __attribute__ ((packed)) _Index3 {
    int x, y ,z;
} Index3;

typedef struct __attribute__ ((packed)) _Ray {
    Vec3 o, d;
} Ray;

Vec3 my_diff(Vec3 a, Vec3 b) {
    Vec3 c;
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    return c;
}

Vec3 my_cross(Vec3 a, Vec3 b) {
    Vec3 c;
    c.x = (a.y * b.z) - (a.z * b.y);
    c.y = (a.z * b.x) - (a.x * b.z);
    c.z = (a.x * b.y) - (a.y * b.x);
    return c;
}

float my_dot(Vec3 a, Vec3 b) {
    return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

__kernel void collision(__global Ray *rays, __global Vec3 *points, __global Index3 *triangles, __global int *collisions) {
    const float EPSILON = 0.0000001;
    int i = get_global_id(0);

    Vec3 tv0, tv1, tv2;
    float a, f, u, v, x;
    Vec3 e1, e2, h, s, q;

    for (int t = 0; t < 1; t++) {
        tv0 = points[triangles[t].x];
        tv1 = points[triangles[t].y];
        tv2 = points[triangles[t].z];

        // edge detection
        e1 = my_diff(tv1, tv0);
        e2 = my_diff(tv2, tv0);

        // my_cross product
        h = my_cross(rays[i].d, e2);

        // my_dot product
        a = my_dot(e1, h);

        // DIVERGENCE DETECTED!
        if (a > -EPSILON && a < EPSILON) {
            collisions[i] = -5;
            return;
        }

        f = 1/a;
        s = my_diff(rays[i].o, tv0);
        u = f * my_dot(s, h);

        if (u < 0.0 || u > 1.0) {
            collisions[i] = -2;
            return;
        }

        q = my_cross(s, e1);
        v = f * my_dot(rays[i].d, q);

        if (v < 0.0 || (u + v) > 1.0) {
            collisions[i] = -3;
            return;
        }

        x = f * my_dot(e2, q);
        if (x > EPSILON) {
            collisions[i] = t;
            return;
        }

        collisions[i] = -4;
    }
}
