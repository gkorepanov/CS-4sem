#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "../tools/alerts1.h"
#include <unistd.h>


pthread_t* threads;
long double a, b;
unsigned N;

// field 'a' is used as thread rvalue
typedef struct SimpsonData {
	long double a, b;
} SimpsonData;
SimpsonData* data;


void arg_process(int argc, char** argv) {
	if (argc != 4)
		ERROR("Usage: integral [lower bound] [upper bound] [NUMBER OF THREADS]");

	if (!sscanf(argv[1], "%Lf", &a) || !sscanf(argv[2], "%Lf", &b))
		ERROR("Invalid argument (type <long double>)");
	if (!sscanf(argv[3], "%u", &N))
		ERROR("Invalid argument (type <unsigned>)");
}


// Simpson integration
#define FUNC(x) (((x) == 0) ? 1 : (sin(x)/(x)))
#define DEFAULT_SPLIT 33554432  // =2**25

void* simpson(void* args) {
	unsigned steps = DEFAULT_SPLIT/N;

	long double  a = ((SimpsonData*)args)->a,
			     h = (((SimpsonData*)args)->b - a)/steps,
			     h2 = h/2,
			     acc = 0;

	for (unsigned i = 0; i < steps; ++i) {
		acc += FUNC(a) + 4 * FUNC(a + h2) + FUNC(a + h);
		a += h;
	}

	long double* rvalue = &((SimpsonData*)args)->a;
	*rvalue = acc * h/6;
    pthread_exit(rvalue);
}


int main(int argc, char** argv) {
	arg_process(argc, argv);

	if (!(threads = malloc(N * sizeof(pthread_t))) || !(data = malloc(N * sizeof(SimpsonData))))
		ERROR("Memory allocation failed");

	// get number of online cores
	long cores_num = sysconf(_SC_NPROCESSORS_ONLN);
	
	// A time to take stones away...
	for (unsigned i = 0; i < N; ++i) {
		data[i] = (SimpsonData) {
			a + (b - a)/N * i,
			a + (b - a)/N * (i+1)
		};
		if (pthread_create(&threads[i], NULL, &simpson, &data[i]))
			ERROR("Thread creation failed");
	}

	// ...and a time to get stones together
	long double S = 0, *r;
	for (unsigned i = 0; i < N; ++i) {
		if (pthread_join(threads[i], (void**) &r))
			ERROR("Thread joining failed");
		S += *r;
	}
	printf("%.18Lf\n", S);

	free(threads);
	return 0;
}
