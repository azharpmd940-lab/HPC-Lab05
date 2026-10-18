
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 100
#define MAX_ITER 100000
#define TOLERANCE 0.0001

int main(void)
{
    static double plate[N][N];
    static double next[N][N];

    // Initialize the entire plate to 50 degrees
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            plate[i][j] = 50.0;
        }
    }

    // Set fixed boundary temperatures
    for (int i = 0; i < N; i++) {
        plate[i][0]     = 100.0;  // Left
        plate[i][N - 1] = 100.0;  // Right
        plate[0][i]     = 100.0;  // Top
        plate[N - 1][i] = 0.0;    // Bottom
    }

    // Copy initial values into the next grid
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            next[i][j] = plate[i][j];
        }
    }

    int iteration = 0;
    double maxChange = 0.0;
    double start = omp_get_wtime();

    for (iteration = 1; iteration <= MAX_ITER; iteration++) {
        maxChange = 0.0;

        #pragma omp parallel for reduction(max:maxChange) schedule(static)
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                next[i][j] =
                    (plate[i - 1][j] +
                     plate[i + 1][j] +
                     plate[i][j - 1] +
                     plate[i][j + 1]) / 4.0;

                double change = fabs(next[i][j] - plate[i][j]);

                if (change > maxChange) {
                    maxChange = change;
                }
            }
        }

        // Swap current and next grids
        #pragma omp parallel for schedule(static)
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                plate[i][j] = next[i][j];
            }
        }

        if (maxChange < TOLERANCE) {
            break;
        }
    }

    double elapsed = omp_get_wtime() - start;

    printf("Heat Conduction Simulation\n");
    printf("Grid size: %d x %d\n", N, N);
    printf("Iterations: %d\n", iteration <= MAX_ITER
                                   ? iteration : MAX_ITER);
    printf("Maximum temperature change: %.8f\n", maxChange);
    printf("Execution time: %.6f seconds\n", elapsed);
    printf("Final temperature at center: %.4f C\n",
           plate[N / 2][N / 2]);

    return 0;
}
