#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
	FILE* log = fopen("/var/log/rayzor.log", "a");
	setlinebuf(log);

	{
		time_t current;
		time(&current);
		fprintf(log, "Rayzor session started at %s\n", ctime(&current));
	}

	int numPoints = 0;
	scanf("%i\n", &numPoints);
	fprintf(log, "Number of Points read: %i\n", numPoints);

	Vec3* points = malloc(sizeof(Vec3) * numPoints);
	for (int i = 0; i < numPoints; i++) {
		if (scanf("%f %f %f\n", &points[i].x, &points[i].y, &points[i].z) != 3) {
			fprintf(stderr, "bad input, closing\n");
			fprintf(log, "bad input, closing\n");
			return 1;
		}
		fprintf(log, "Point read: %f %f %f\n", points[i].x, points[i].y, points[i].z);
	}

	int numTriangles = 0;
	scanf("%i\n", &numTriangles);
	fprintf(log, "Number of Triangles read: %i\n", numTriangles);

	Index3* triangles = malloc(sizeof(Index3) * numTriangles);
	for (int i = 0; i < numTriangles; i++) {
		if (scanf("%i %i %i\n", &triangles[i].x, &triangles[i].y, &triangles[i].z) != 3) {
			fprintf(stderr, "bad input, closing\n");
			fprintf(log, "bad input, closing\n");
			return 1;
		}
		fprintf(log, "Triangle read: %i %i %i\n", triangles[i].x, triangles[i].y, triangles[i].z);
	}

	int numRays = 0;
	scanf("%i\n", &numRays);
	fprintf(log, "Number of Rays read: %i\n", numRays);

	Ray* rays = malloc(sizeof(Ray) * numRays);
	for (int i = 0; i < numRays; i++) {
		if (scanf("%f %f %f %f %f %f\n", &rays[i].o.x, &rays[i].o.y, &rays[i].o.z, &rays[i].d.x, &rays[i].o.y, &rays[i].d.z) != 6) {
			fprintf(stderr, "bad input, closing\n");
			fprintf(log, "bad input, closing\n");
			return 1;
		}
		fprintf(log, "Ray read: %f %f %f %f %f %f\n", rays[i].o.x, rays[i].o.y, rays[i].o.z, rays[i].d.x, rays[i].d.y, rays[i].d.z);
	}

	free(rays);
	free(triangles);
	free(points);
	
	fprintf(log, "Ending session");
	return 0;
}
