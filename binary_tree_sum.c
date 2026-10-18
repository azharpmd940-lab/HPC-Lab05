
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct Node {
    int data;
    struct Node *left;
    struct Node *right;
} Node;

Node *createNode(int value)
{
    Node *node = malloc(sizeof(Node));

    if (node == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    node->data = value;
    node->left = NULL;
    node->right = NULL;

    return node;
}

long long treeSum(Node *root)
{
    if (root == NULL)
        return 0;

    long long leftSum = 0;
    long long rightSum = 0;

    #pragma omp task shared(leftSum) if(omp_get_active_level() < 8)
    leftSum = treeSum(root->left);

    #pragma omp task shared(rightSum) if(omp_get_active_level() < 8)
    rightSum = treeSum(root->right);

    #pragma omp taskwait

    return root->data + leftSum + rightSum;
}

void freeTree(Node *root)
{
    if (root == NULL)
        return;

    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int main(void)
{
    Node *root = createNode(10);

    root->left = createNode(5);
    root->right = createNode(15);

    root->left->left = createNode(2);
    root->left->right = createNode(7);

    root->right->left = createNode(12);
    root->right->right = createNode(20);

    long long totalSum = 0;

    #pragma omp parallel
    {
        #pragma omp single
        {
            totalSum = treeSum(root);
        }
    }

    printf("Sum of all nodes = %lld\n", totalSum);

    freeTree(root);

    return 0;
}
