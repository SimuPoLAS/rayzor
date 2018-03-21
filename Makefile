OPTIONS:=
OPTIONS+= -l OpenCL

rayzor: rayzor.c
	gcc -Wall -g rayzor.c -o rayzor $(OPTIONS)

clean:
	rm -rf rayzor
