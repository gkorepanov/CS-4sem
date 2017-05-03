#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "../tools/alerts1.h"
#include <unistd.h>
#include <errno.h>

#include <sched.h>


pthread_t* threads;
long double a, b, h, h2;
unsigned N, steps;

// field 'a' is used as thread rvalue
typedef struct SimpsonData {
	long double a;
	int core_id;
} SimpsonData;
SimpsonData* data;


// core_id = 0, 1, ... n-1, where n is the system's number of cores
int stick_this_thread_to_core(int core_id) {
   int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
   if (core_id < 0 || core_id >= num_cores)
      return EINVAL;

   cpu_set_t cpuset;
   CPU_ZERO(&cpuset);
   CPU_SET(core_id, &cpuset);

   pthread_t current_thread = pthread_self();
   return pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
}

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
#define DEFAULT_SPLIT 4194304

void* simpson(void* args) {
	int core_id = ((SimpsonData*)args)->core_id;
	if(stick_this_thread_to_core(core_id))
		ERROR("Sticking thread to specific core failed.");

	long double a = ((SimpsonData*)args)->a,
				b = a + h2,
				c = a + h,
				func_a = FUNC(a),
				func_c,
				sum = 0;

	for (unsigned i = 0; i < steps; ++i) {
		func_c = FUNC(c);
		sum += func_a + 4 * FUNC(b) + func_c;
		func_a = func_c;
		a = c;
		b += h;
		c += h;
	}

	long double* rvalue = &((SimpsonData*)args)->a;
	*rvalue = sum * h/6;
    pthread_exit(rvalue);
}


int main(int argc, char** argv) {
	arg_process(argc, argv);

	if (!(threads = malloc(N * sizeof(pthread_t))) || !(data = malloc(N * sizeof(SimpsonData))))
		ERROR("Memory allocation failed");

	// get number of online cores
	long cores_num = sysconf(_SC_NPROCESSORS_ONLN);


	steps = DEFAULT_SPLIT/N;
	h = (b - a) / DEFAULT_SPLIT;
	h2 = h/2;
	interval_len = (b - a)/N;
	
	// A time to take stones away...
	unsigned int i;
	long double interval_start;
	for (i = 0, interval_start = a;
	i < N;
	++i, interval_start += interval_len) {

		data[i] = (SimpsonData) {
			interval_start,
			i % cores_num
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
