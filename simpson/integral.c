#include <stdlib.h>
#include <math.h>
/*
 * Simpson integration
 */
double Simpson(double(*f)(double), double a, double b) {
    return (b - a) / 6 * (f(a) + 4 * f((a + b) / 2) + f(b));
}

/*
 * Let's integrate some function
 */
double f(double x) {
	if (x)
		return sin(x)/x;
	else
		return 1;
}
