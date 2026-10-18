
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

int visited[V] = {0};

void parallelDFS(int u)
{
    int shouldVisit = 0;

    // Safely check and mark the vertex
    #pragma omp critical
    {
        if (!visited[u]) {
            visited[u] = 1;
            shouldVisit = 1;
            printf("%d ", u);
        }
    }

    if (!shouldVisit)
        return;

    // Explore neighbors using OpenMP tasks
    for (int v = 0; v < V; v++) {
        if (graph[u][v]) {
            #pragma omp task firstprivate(v)
            parallelDFS(v);
        }
    }

    // Wait for this vertex's child tasks
    #pragma omp taskwait
}

int main(void)
{
    printf("DFS Traversal: ");

    #pragma omp parallel
    {
        #pragma omp single
        {
            parallelDFS(0);
        }
    }

    printf("\n");
    return 0;
}
