#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../tools/alerts.h"


#define FUNC(x) x*x/(1/x+x-2+x*x)*x
#define DEFAULT_SPLIT 500000000

// hope there are no more than 256 ones
#define MAX_CPUS 256
#define MAX_SYS_FILE_LENGTH 256


typedef struct SimpsonData {
    long double a;                         // is used as thread rvalue
    cpu_set_t cpuset;
    unsigned n;
} SimpsonData;

SimpsonData* data;
pthread_t* threads;
long double a, b, h, h2;
unsigned N, steps;

int use_hyper = 0;

unsigned is_virtual_cpu_online[MAX_CPUS],  // flags showing whether some virtual cpu is online or not
         is_cpu_online[MAX_CPUS],          // flags showing whether some physical core is online or not
         online_cpus_num = 0,
         online_virtual_cpus_num = 0;
cpu_set_t cpu_sets[MAX_CPUS];              // sets of virtual cpus sharing same physical core
cpu_set_t virtual_cpu_sets[MAX_CPUS];



void arg_process(int argc, char** argv);
void cpu_process();
void* simpson(void* args);


int main(int argc, char** argv) {
    arg_process(argc, argv);               // acquire left and right bounds (a, b) and number of lines N
    cpu_process();                         // acquire lists of online cores and thread siblings

    if (!(threads = malloc(online_cpus_num * sizeof(pthread_t))) ||
        !(data = malloc(online_cpus_num * sizeof(SimpsonData))))
        ERROR("Memory allocation failed");

    // prepare variables for calculation
    steps = DEFAULT_SPLIT / N;
    h = (b - a) / DEFAULT_SPLIT;
    h2 = h/2;
    long double interval_start,
                interval_len = (b - a)/N;
    unsigned j = 0, i;
    
    if (!use_hyper) {
        for (i = 0, interval_start = a;
             i < online_cpus_num;
             ++i, interval_start += interval_len, j++) {

            // running the calculation on every physical core, but
            // some threads are "fake" and are not actually used so that
            // all the cores are loaded and Intel Turbo Boost don't distort the results
            
            while(!is_cpu_online[j]) ++j; // find next online physical core

            data[i] = (SimpsonData) {
                interval_start,
                cpu_sets[j],
                i
            };

            PRINT("Running thread on core %u", j);
        }

        for (i = 0; i < online_cpus_num; i++)
            if (pthread_create(&threads[i], NULL, &simpson, &data[i]))
                ERROR("Thread creation failed");

        long double S = 0, *r;
        for (i = 0; i < online_cpus_num; ++i) {
            if (pthread_join(threads[i], (void**) &r))
                ERROR("Thread joining failed");
            if (i < N)
                S += *r;
        }
        printf("%.12Lf\n", S);
    }
    else {
        for (unsigned i = 0, interval_start = a;
             i < online_virtual_cpus_num;
             ++i, interval_start += interval_len) {

            // running the calculation on every physical core, but
            // some threads are "fake" and are not actually used so that
            // all the cores are loaded and Intel Turbo Boost don't distort the results

            data[i] = (SimpsonData) {
                interval_start,
                virtual_cpu_sets[i],
                i
            };

            PRINT("Running thread on virtual core %u", i);
        }

        PRINT("online_virtual_cpus_num %u", online_virtual_cpus_num);
        for (unsigned i = 0; i < online_virtual_cpus_num; i++)
            if (pthread_create(&threads[i], NULL, &simpson, &data[i]))
                ERROR("Thread creation failed");

        long double S = 0, *r;
        for (unsigned i = 0; i < online_virtual_cpus_num; ++i) {
            if (pthread_join(threads[i], (void**) &r))
                ERROR("Thread joining failed");
                S += *r;
        }
        printf("%.12Lf\n", S);
    }
    free(threads);
    free(data);
    return 0;
}



void arg_process(int argc, char** argv) {
    if (argc != 4)
        ERROR("Usage: integral [lower bound] [upper bound] [NUMBER OF THREADS]");

    if (!sscanf(argv[1], "%Lf", &a) || !sscanf(argv[2], "%Lf", &b))
        ERROR("Invalid argument (type <long double>)");
    if (!sscanf(argv[3], "%u", &N))
        ERROR("Invalid argument (type <unsigned>)");
}


void cpu_process() {
    memset(&is_virtual_cpu_online, 0, sizeof(is_virtual_cpu_online));

    int fd;
    ERRTEST(fd = open("/sys/devices/system/cpu/online", O_RDONLY));
    char online_str[MAX_SYS_FILE_LENGTH];
    int bytes_read;
    ERRTEST(bytes_read = read(fd, online_str, MAX_SYS_FILE_LENGTH-1));
    close(fd);
    online_str[bytes_read] = '\0';

    unsigned left_bound, right_bound;
    char *saveptr1 = NULL,
         *saveptr2 = NULL,
         *str = strtok_r(online_str, ",", &saveptr1);

    do {
        str = strtok_r(str, "-", &saveptr2);
        if (!sscanf(str, "%u", &left_bound))
            ERROR("Failed to parse online cores file");
        right_bound = left_bound;

        str = strtok_r(NULL, "-", &saveptr2);
        if (str)
            if (!sscanf(str, "%u", &right_bound))
                ERROR("Failed to parse online cores file");

        // update table
        for (unsigned i = left_bound; i <= right_bound; i++)
            is_virtual_cpu_online[i] = 1;
    } while ((str = strtok_r(NULL, ",", &saveptr1)));


    memset(&is_cpu_online, 0, sizeof(is_cpu_online));
    for (unsigned i = 0; i < MAX_CPUS; i++) {
        CPU_ZERO(&cpu_sets[i]);
        CPU_ZERO(&virtual_cpu_sets[i]);
    }
    
    char filename[256],
         core_str[11];
    int  core_id;

    for (unsigned i = 0; i < MAX_CPUS; i++) {
        if (!is_virtual_cpu_online[i])
            continue;

        CPU_SET(i, &virtual_cpu_sets[online_virtual_cpus_num]);
        PRINT("%u virtual core is online", online_virtual_cpus_num);
        online_virtual_cpus_num++;

        // read the corresponding physical core number
        sprintf(filename, "/sys/devices/system/cpu/cpu%d/topology/core_id", i);
        ERRTEST(fd = open(filename, O_RDONLY));
        int bytes_read;
        ERRTEST(bytes_read = read(fd, core_str, 10));
        close(fd);
        core_str[bytes_read] = '\0';

        if (!sscanf(core_str, "%d", &core_id))
            ERROR("Failed to parse topology/core_id file");

        // update table with online physical cores and
        // increase number of online physical cores by 1
        if (!is_cpu_online[core_id]) {
            is_cpu_online[core_id] = 1;
            online_cpus_num++;

            // bind virtual cpu to physical one
            CPU_SET(i, &cpu_sets[core_id]);
        }

        PRINT("%d virtual cpu is on %d physical core\n", i, core_id);

    }

    if (online_cpus_num < N) {
        PRINT("The number of physical cores online is less"
        " than requested number of threads, running with all available virtual cores");
        PRINT("Virtual cores available: %u\n", online_virtual_cpus_num);
        use_hyper = 1;
    }

}


void* simpson(void* args) {
    cpu_set_t cpuset = ((SimpsonData*)args)->cpuset;
    pthread_t current_thread = pthread_self();
       if (pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset))
        PRINT("Sticking thread %u to specific core failed, interval start: %Lf", ((SimpsonData*)args)->n, ((SimpsonData*)args)->a);

    PRINT("Thread %u is running, interval start: %Lf", ((SimpsonData*)args)->n, ((SimpsonData*)args)->a);

    long double lh = h,
                lh2 = h2,
                a = ((SimpsonData*)args)->a,
                b = a + lh2,
                c = a + lh,
                func_a = FUNC(a),
                func_c,
                sum = 0;

    for (unsigned i = 0; i < steps; ++i) {
        func_c = FUNC(c);
        sum += func_a + 4 * FUNC(b) + func_c;
        func_a = func_c;
        a = c;
        b += lh;
        c += lh;
    }

    long double* rvalue = &((SimpsonData*)args)->a;
    *rvalue = sum * h/6;
    pthread_exit(rvalue);
}
