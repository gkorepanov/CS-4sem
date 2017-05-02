#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "../tools/alerts1.h"


pthread_t* threads;
double a, b;
unsigned N;

// field 'a' is used as thread rvalue
typedef struct SimpsonData {
	double a, b;
} SimpsonData;
SimpsonData* data;


void arg_process(int argc, char** argv) {
	if (argc != 4)
		ERROR("Usage: integral [lower bound] [upper bound] [NUMBER OF THREADS]");

	if (!sscanf(argv[1], "%lf", &a) || !sscanf(argv[2], "%lf", &b))
		ERROR("Invalid argument (type <double>)");
	if (!sscanf(argv[3], "%u", &N))
		ERROR("Invalid argument (type <unsigned>)");

	if (!(N==1 || N==2 || N==4 || N==8 || N==16 || N==32 || N==64))
		ERROR("Invalid argument (type <unsigned>)\n\
              NUMBER OF THREADS: expected 1, 2, 4, 8, 16, 32 or 64");
}


// Simpson integration
#define FUNC(x) (((x) == 0) ? 1 : (sin(x)/(x)))
#define DEFAULT_SPLIT 33554432  // =2**25

void* simpson(void* args) {
	unsigned steps = DEFAULT_SPLIT/N;

	double  a = ((SimpsonData*)args)->a,
			h = (((SimpsonData*)args)->b - a)/steps,
			h2 = h/2,
			acc = 0;

	for (unsigned i = 0; i < steps; ++i) {
		acc += FUNC(a) + 4 * FUNC(a + h2) + FUNC(a + h);
		a += h;
	}

	double* rvalue = &((SimpsonData*)args)->a;
	*rvalue = acc * h/6;
    pthread_exit(rvalue);
}


int main(int argc, char** argv) {
	arg_process(argc, argv);

	if (!(threads = malloc(N * sizeof(pthread_t))) || !(data = malloc(N * sizeof(SimpsonData))))
		ERROR("Memory allocation failed");

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
	double S = 0, *r;
	for (unsigned i = 0; i < N; ++i) {
		if (pthread_join(threads[i], (void**) &r))
			ERROR("Thread joining failed");
		S += *r;
	}
	printf("%lf\n", S);

	free(threads);
	return 0;
}
