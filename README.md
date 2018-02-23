# rayzor

The purpose of this program consists of letting rays collide with triangles. The specialty is about the apporach. The calculation is done paralell on the GPU.

Please keep in mind, that this is still under developmend and that the things documented in this readme may not function properly yet.

## Usage

The application takes a list of points, triangles and rays. Normals may be added to also evaluate the next direction the rays would take. The format looks like this:

```
3 
1 1 1
2 2 2
3 3 3
1
0 1 2
1
0 0 0 1 1 1
```

The frist number indicates the count of the following points. So it reads 3 points. Next, it reads one triangle. The triangle itself indexes the prevoiusly defined points. At last, the number of rays is communicated and with them, origin and direction of each ray would follow.

The output would just be a key value pair. The key indexes a ray and the value indexes a trangle the ray collided with. For example:

```
1
0 0
```

In this case, ray 0 collides with triangle 0

## GPGPU

To evaluate the trangles the rays collided with, we use the paralell principle of general purpose graphics processing units. To accesse the capabilities of the GPU, we use the framework OpenCL.

## Authors

- UgiNyaa
- satorialist

This project is being developed in the scope of a school project, which is refered to as SimuPoLAS
