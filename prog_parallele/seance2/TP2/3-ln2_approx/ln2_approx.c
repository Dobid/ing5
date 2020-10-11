#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N_MAX 1000000000LL

int main (int argc, char **argv)
{
    int nb_threads;
    if(argv[1] == NULL)
    {
        perror("No arguments passed when launching the program \n Default number of threads will be : 10 \n");
        nb_threads = 10;
    }
    else
    {
        nb_threads = atoi(argv[1]);
    }

    double sum = 0.0;
    long long n;
    
    // On indique le nombre de threads à créer par openmp
    omp_set_num_threads(nb_threads);
    printf("Nombre de threads : %d\n", nb_threads);

    #pragma omp parallel for private(n), reduction(+:sum)
    for (n = N_MAX; n > 0; n--)
    {
        if (n%2 == 0)
        {
            sum -= 1.0 / (double )n;
        }
        else
        {
            sum += 1.0 / (double ) n ;
        }
    }
    printf ("sum: %.12f\n" , sum) ;
    return 0;
}