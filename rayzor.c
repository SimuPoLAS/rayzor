#include <stdio.h>
#include <stdlib.h>

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
	int numPoints = 0;
	scanf("%i\n", &numPoints);

	Vec3* points = malloc(sizeof(Vec3) * numPoints);
	for (int i = 0; i < numPoints; i++) {
		if (scanf("%f %f %f\n", &points[i].x, &points[i].y, &points[i].z) != 3) {
			fprintf(stderr, "bad input\n");
			return 1;
		}
	}

	int numTriangles = 0;
	scanf("%i\n", &numTriangles);

	Index3* triangles = malloc(sizeof(Index3) * numTriangles);
	for (int i = 0; i < numTriangles; i++) {
		if (scanf("%i %i %i\n", &triangles[i].x, &triangles[i].y, &triangles[i].z) != 3) {
			fprintf(stderr, "bad input\n");
			return 1;
		}
	}

	int numRays = 0;
	scanf("%i\n", &numRays);

	Ray* rays = malloc(sizeof(Ray) * numRays);
	for (int i = 0; i < numRays; i++) {
		if (scanf("%f %f %f %f %f %f\n", &rays[i].o.x, &rays[i].o.y, &rays[i].o.z, &rays[i].d.x, &rays[i].o.y, &rays[i].d.z) != 6) {
			fprintf(stderr, "bad input\n");
			return 1;
		}
	}

	free(rays);
	free(triangles);
	free(points);
	
	return 0;
}
