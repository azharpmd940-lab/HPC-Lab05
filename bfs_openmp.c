
#include <stdio.h>
#include <omp.h>

#define V 7

int graph[V][V] = {
    {0, 1, 1, 0, 0, 0, 0},
    {1, 0, 0, 1, 1, 0, 0},
    {1, 0, 0, 0, 0, 1, 1},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0}
};

int main(void)
{
    int visited[V] = {0};
    int frontier[V], nextFrontier[V];
    int count = 1;

    frontier[0] = 0;
    visited[0] = 1;

    printf("BFS Traversal: ");

    while (count > 0) {
        int nextCount = 0;

        // Process all vertices in the current level
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < count; i++) {
            int u = frontier[i];

            #pragma omp critical
            printf("%d ", u);

            for (int v = 0; v < V; v++) {
                if (graph[u][v]) {
                    int newlyVisited = 0;

                    #pragma omp critical
                    {
                        if (!visited[v]) {
                            visited[v] = 1;
                            newlyVisited = 1;
                        }
                    }

                    if (newlyVisited) {
                        #pragma omp critical
                        {
                            nextFrontier[nextCount] = v;
                            nextCount++;
                        }
                    }
                }
            }
        }

        // Move to the next level after all threads finish
        count = nextCount;

        for (int i = 0; i < count; i++)
            frontier[i] = nextFrontier[i];
    }

    printf("\n");
    return 0;
}
