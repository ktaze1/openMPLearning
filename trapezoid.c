#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

void Trap(double a, double b, int n, double* global_result_p);
double f(double a);
int main(int argc, char* argv[]) {
    double global_result = 0.0;
    double a, b;
    long int n;
    int  thread_count ;
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    thread_count = strtol(argv[1], NULL, 10);
    printf("Count of Threads : %d\n", thread_count);
    a=0.0;
    b=9.0;
    n=2147483647;
    //n=9999999;
    printf("Enter a:%lf, b:%lf, and n:%ld\n",a,b,n);
    fflush(stdout);
    //scanf("%lf %lf %d", &a, &b, &n);
    fflush(stdout);

# pragma omp parallel num_threads( thread_count )
    Trap(a, b, n, &global_result);

    printf("With n = %ld trapezoids, our estimate\n", n);
    printf("of the integral from %f to %f = %.14e\n",a, b, global_result);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Running time for first block is %f\n",cpu_time_used);
 return 0 ;
}/* main */


void Trap(double a, double b, int n, double* global_result_p) {
    double h, x, my_result;
    double local_a, local_b;
    int i, local_n;
    int my_rank = omp_get_thread_num();
    int  thread_count  = omp_get_num_threads();

    h = (b-a)/n;
    local_n = n/ thread_count ;
    local_a = a + my_rank*local_n*h;
    local_b = local_a + local_n*h;
    my_result = (f(local_a) + f(local_b))/2.0;
    for (i =1 ; i <= local_n-1; i++) {
    x = local_a + i*h;
    my_result += f(x);
    }
    my_result = my_result*h;

# pragma omp critical
 *global_result_p += my_result;
} /* Trap */

double f(double a){
 /* 3x^2+6x-7*/
 return a*a;
}