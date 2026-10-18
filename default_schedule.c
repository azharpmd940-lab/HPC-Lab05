
#include <stdio.h>
#include <omp.h>

#define N 20

int main(void)
{
    int thread_used[N];

    omp_set_dynamic(0);

    #pragma omp parallel num_threads(4)
    {
        #pragma omp for
        for (int i = 0; i < N; i++) {
            thread_used[i] = omp_get_thread_num();
        }
    }

    printf("Default OpenMP loop scheduling\n");
    printf("Iteration\tThread\n");

    for (int i = 0; i < N; i++) {
        printf("%d\t\t%d\n", i, thread_used[i]);
    }

    return 0;
}
