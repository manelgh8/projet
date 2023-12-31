#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h> 

int n, m, shouldExit = 0;
int **A, **B, **C;

pthread_mutex_t mutex;
sem_t empty, full;


void *prod(void *arg)
{
    int i = *(int *)arg, item = 0;
    double extraVal = 0.0;  
    for (int j = 0; j < m; j++)
    {
        item = 0;
        for (int k = 0; k < m; k++)
            item += B[i][k] * C[k][j];
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        A[i][j] = item;
        shouldExit = (i == n - 1 && j == m - 1);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    pthread_exit(NULL);
}
void *cons(void *arg)
{
    while (!shouldExit)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
    pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
    srand(time(NULL));
    printf("Enter number of rows and cols in matrix B: ");
    n = 3;
    m = 3;
    if (m != n)
    {
        perror("Error: Multiplication of B*C is not possible.\n");
        exit(EXIT_FAILURE);
    }
    B = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
        B[i] = (int *)malloc(m * sizeof(int));
    C = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
        C[i] = (int *)malloc(m * sizeof(int));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            B[i][j] = rand() % 10;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            C[i][j] = rand() % 10;
    printf("\n______  matric B __________\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
            printf("%d  ", B[i][j]);
        printf("\n");
    }
    printf("\n_______ matrice C _______\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
            printf("%d  ", C[i][j]);
        printf("\n");
    }
    printf("\n\n");

    A = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
        A[i] = (int *)malloc(m * sizeof(int));
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, 1);
    sem_init(&full, 0, 0);
    pthread_t prod_t[n], cons_t;
    for (int i = 0; i < n; i++)
    {
        int *idx = malloc(sizeof(int));
        *idx = i;
        pthread_create(&prod_t[i], NULL, prod, idx);
    }
    pthread_create(&cons_t, NULL, cons, NULL);
    for (int i = 0; i < n; i++)
        pthread_join(prod_t[i], NULL);
    pthread_join(cons_t, NULL);
    printf("\n========= A ==========\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
            printf("%d  ", A[i][j]);
        printf("\n");
    }
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
