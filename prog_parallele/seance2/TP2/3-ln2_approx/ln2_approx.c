#include <stdio.h>
#include <omp.h>

#define N_MAX 1000000000LL

int main (void)
{
    double sum = 0.0;
    long long n;
    int nb_threads;
    printf("nb de threads ? : \n");
    scanf("%d", &nb_threads);
    
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