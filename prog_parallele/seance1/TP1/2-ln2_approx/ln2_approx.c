#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N_MAX 1000000000LL

// int main (int argc, char **argv)
// {
//    double sum = 0.0;
//     long long n;
//     for (n = N_MAX; n > 0; n--)
//     {
//      	if (n % 2 == 0)
//         {
//     		  sum -= 1.0 / (double)n;
//      	}
//         else
//         {
//    		  sum += 1.0 / (double)n ;
//       	}
//     }
//     printf ("sum: %.12f\n",sum);
//     return 0;
// }

// Structure d'arguments pour les threads
typedef struct{
     // Borne minimale
     long long t_min;
     // Borne maximale
     long long t_max;
     // Id du thread
     int t_id; 
} arg;

// Tableau de sommes des threads
double th_sums[10];

// Fonction des threads 
void *func(void *arg);

int main(int argc, char **argv)
{
    // Nombre de threads                                                                      
    int nb_threads;
    // Somme finale des sommes du tableau
    double sum = 0.0;

    // Si aucun argument n'est passé
    if (argv[1] == NULL)
    {
        perror("No arguments passed when launching the program \n Default number of thread will be : 10 \n");
        nb_threads = 10;
    }
    else
    {
        // On récupère l'argument du terminal et on le converti en int
        nb_threads = atoi(argv[1]);
    }
    printf("Nombre de threads = %d\n", nb_threads);

    // On initialise le tableau des sommes à 0
    for(int i = 0; i < nb_threads; i++)
    {
        th_sums[i] = 0;
    }

    // Tableaux de threads de la taille de l'argument récupéré en paramètres
    pthread_t threads[nb_threads];
    // Tableaux de structures d'arguments pour stocker les différentes arguments des threads
    arg tab_arg[nb_threads];

    // On divise le nombre max par le nombre de threads et on cast en int pour récupérer l'entier
    long long int res = N_MAX / nb_threads;
    // On récupère le reste de la division grâce à la fonction modulo
    long long int reste = N_MAX % nb_threads;

    // On parcourt le tableaux de threads et d'arguments
    for (int i = 0; i < nb_threads; i++)
    {
        // On initialise son id à i
        tab_arg[i].t_id = i;

        // On initialise son min au max du thread précédent
        tab_arg[i].t_min = i*res;

        // Si c'est le dernier thread du tableau
        if(i == nb_threads-1)
        {
            // On initialise son max à son min plus le résultat auquel on ajoute le reste 
            tab_arg[nb_threads-1].t_max = tab_arg[nb_threads-1].t_min + res + reste;
        }
        else 
        {
            // On initialise son max à son min plus le résultat
            tab_arg[i].t_max = tab_arg[i].t_min + res;
        }

        //printf("\nmin: %lld, max: %lld", tab_arg[i].t_min, tab_arg[i].t_max);
        // On créé le thread avec pour paramètre un pointeur d'arguments
        int ret = pthread_create(&threads[i], NULL, func, &tab_arg[i]);
        if(ret != 0) perror("Erreur creation de thread !\n");
    }

    // On parcourt le tableau de threads
    for(int i = 0; i < nb_threads; i++)
    {
        // Pour chaque thread du tableau on attend qu'il soit fini
        pthread_join(threads[i], NULL);
        // On ajoute la somme calculée par chaque thread à la somme finale
        sum += th_sums[i];
    }

    // On affiche la somme finale
    printf("sum: %.12f\n", sum);
    return 0;
}

/** Fonction des threads
** Params : un pointeur d'arguments de type void qu'il faut ensuite caster en type arg
** Return : un pointeur null
**/
void* func(void* func_args)
{
    // On récupère le pointeur null d'arguments que l'on cast en pointeur de structure arg
    arg *loc_args = (arg*) func_args;
    long long int n;

    // On parcourt n depuis la borne max jusqu'à la borne min du thread en cours d'exécution
    // On accède au t_id, t_min et t_max avec une flèche car loc_args est un pointeur
    for (n = loc_args->t_max; n > loc_args->t_min; n--)
    {
        if (n % 2 == 0)
        {
            th_sums[loc_args->t_id] -= 1.0 / (double)n;
        }
        else
        {
            th_sums[loc_args->t_id] += 1.0 / (double)n;
        }
    }
    return (NULL);
}
