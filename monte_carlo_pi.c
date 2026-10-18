
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>
#include <math.h>

#define NUM_SIZES 7

int main(void)
{
    const long long sample_sizes[NUM_SIZES] = {
        10LL, 100LL, 10000LL, 1000000LL,
        10000000LL, 100000000LL, 1000000000LL
    };

    const double actual_pi = acos(-1.0);

    FILE *file = fopen("pi_results.csv", "w");
    if (file == NULL) {
        perror("Unable to create pi_results.csv");
        return 1;
    }

    fprintf(file, "Points,EstimatedPi,AbsoluteError,TimeSeconds\n");

    printf("%-15s %-18s %-18s %-15s\n",
           "Points", "Estimated Pi", "Absolute Error", "Time (s)");

    for (int s = 0; s < NUM_SIZES; s++) {
        long long n = sample_sizes[s];
        long long inside = 0;

        double start = omp_get_wtime();

        #pragma omp parallel reduction(+:inside)
        {
            unsigned int seed =
                12345u + 1000u * (unsigned int)omp_get_thread_num();

            #pragma omp for
            for (long long i = 0; i < n; i++) {
                double x = 2.0 * rand_r(&seed) /
                           ((double)RAND_MAX + 1.0) - 1.0;
                double y = 2.0 * rand_r(&seed) /
                           ((double)RAND_MAX + 1.0) - 1.0;

                if (x * x + y * y <= 1.0) {
                    inside++;
                }
            }
        }

        double estimated_pi = 4.0 * (double)inside / (double)n;
        double error = fabs(estimated_pi - actual_pi);
        double elapsed = omp_get_wtime() - start;

        printf("%-15lld %-18.10f %-18.10f %-15.4f\n",
               n, estimated_pi, error, elapsed);

        fprintf(file, "%lld,%.12f,%.12f,%.6f\n",
                n, estimated_pi, error, elapsed);
    }

    fclose(file);

    printf("\nResults saved to pi_results.csv\n");
    return 0;
}
