#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "../tools/alerts.h"
#include <unistd.h>
#include <errno.h>

#include <sched.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



pthread_t* threads;
long double a, b, h, h2;
unsigned N, steps;

// field 'a' is used as thread rvalue
typedef struct SimpsonData {
	long double a;
	cpu_set_t cpuset;
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
#define DEFAULT_SPLIT 4194304

void* simpson(void* args) {
	cpu_set_t cpuset = ((SimpsonData*)args)->cpuset;
	pthread_t current_thread = pthread_self();
   	if(pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset))
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

// hope there are no more than 256 ones
#define MAX_CPUS 256

#define MAX_SYS_FILE_LENGTH 256

int main(int argc, char** argv) {

	// acquire left and right bounds (a, b) and number of lines N
	arg_process(argc, argv);

	/*****************************************************************************
	 *                 ACQUIRING LIST OF ONLINE VIRTUAL CORES                    *
	 *****************************************************************************/

	// flags showing whether some virtual cpu is online or not
	int is_virtual_cpu_online[MAX_CPUS];
	memset(&is_virtual_cpu_online, 0, sizeof(is_virtual_cpu_online));

	// read file
	int fd;
	ERRTEST(fd = open("/sys/devices/system/cpu/online", O_RDONLY));

	char online_str[MAX_SYS_FILE_LENGTH];
	ERRTEST(read(fd, online_str, MAX_SYS_FILE_LENGTH));

	close(fd);

	// parse string
	unsigned int left_bound, right_bound;
	char* saveptr1 = NULL;
	char* saveptr2 = NULL;
	char* str;

	while((str = strtok_r(online_str, ",", &saveptr1))) {
		str = strtok_r(str, "-", &saveptr2);
		if (!sscanf(str, "%u", &left_bound))
			ERROR("Failed to parse online cores file");
		right_bound = left_bound;

		str = strtok_r(str, "-", &saveptr2);
		if (str) {
			if (!sscanf(str, "%u", &right_bound))
			ERROR("Failed to parse online cores file");
		}

		// update table
		for (unsigned int i = left_bound; i <= right_bound; i++)
			is_virtual_cpu_online[i] = 1;
	}

	// log
	PRINT("Online virtual cores:")
	for (unsigned int i = 0; i < MAX_CPUS; i++) {
		if (is_virtual_cpu_online[i])
			printf("%d ", i);
	}



	/*****************************************************************************
	 *                      ACQUIRING THREAD SIBLINGS LISTS                      *
	 *****************************************************************************/

	// flags showing whether some physical core is online or not
	int is_cpu_online[MAX_CPUS];
	// sets of virtual cpus sharing same physical core
	cpu_set_t cpu_sets[MAX_CPUS];

	memset(&is_cpu_online, 0, sizeof(is_cpu_online));
	for (unsigned int i; i < MAX_CPUS; i++) 
		CPU_ZERO(&cpu_sets[i]);

	unsigned int online_cpus_num = 0;
	char filename[256];
	char core_str[10];
	int core_id;

	for (unsigned int i = 0; i < MAX_CPUS; i++) {

		// read the corresponding physical core number
		sprintf(filename, "/sys/devices/system/cpu/online/cpu/cpu%d/topology/core_id", i);
		ERRTEST(fd = open(filename, O_RDONLY));
		ERRTEST(read(fd, core_str, 10));
		close(fd);

		if (!sscanf(core_str, "%d", &core_id))
			ERROR("Failed to parse topology/core_id file");

		// update table with online physical cores and
		// increase number of online physcial cores by 1
		if (!is_cpu_online[core_id]) {
			is_cpu_online[core_id] = 1;
			online_cpus_num++;
		}

		// add virtual cpu to physical one
		CPU_SET(core_id, &cpu_sets[core_id]);
	}

	if (online_cpus_num < N)
		ERROR("THe number of physical cores online is less than requested number of threads");

	if (!(threads = malloc(N * sizeof(pthread_t))) || !(data = malloc(N * sizeof(SimpsonData))))
		ERROR("Memory allocation failed");



	/*****************************************************************************
	 *                                 CALCULATION                               *
	 *****************************************************************************/

	// prepare variables for calculation

	steps = DEFAULT_SPLIT / N;
	h = (b - a) / DEFAULT_SPLIT;
	h2 = h/2;
	long double interval_len = (b - a)/N;
	
	// A time to take stones away...
	unsigned int i;
	unsigned int j = 0;
	long double interval_start;


	for (i = 0, interval_start = a;
	i < N;
	++i, interval_start += interval_len) {

		// find next online physical core
		for (; !is_cpu_online[j]; j++);

		data[i] = (SimpsonData) {
			interval_start,
			cpu_sets[j]
		};

		j++;
	}

	for (i = 0; i < N; i++) {
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