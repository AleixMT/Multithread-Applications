#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <strings.h>
#include <assert.h>

// No son necesarios los defines tras el refactor
struct Vector
{
    int i, j, v;
};

/**
 * Eliminadas variables globales individuales. Este tipo de variable se mapea obligatoriamente en memoria por lo que
 * se pierde tiempo en accederlas. Por supuesto, se benefician de las ventajas de la caché, pero esto no puede
 * competir con una variable que puede estar mapeada en los registros del procesador.
 **/

int main(int np, char *p[])
{
    int i, j, k;
    assert(np == 2);  // Check number of parameters

    int nn = atoi(p[1]);
    assert(nn <= N);  // Check that the first number is less or equal to N
    srand(1);  // Initialize fixed random seed for reproducible results (in the checksum of C)

    int nnD = nn * nn / 100;

    /** Usamos solamente nn * nn * int por cada matriz en memoria, donde nn es menor o igual a 10000, eso es como
     * máximo 10000 * 10000  * 4 Bytes = 4 * 10 ^ 8 Bytes de memoria que es exactamente lo que reservamos de forma
     * estática al principio del código, sin embargo, nn no siempre será el máximo, por lo que estamos reservando
     * más memoria de la necesaria 9999 veces de cada 10000 variaciones posibles en nuestro parámetro durante la
     * ejecución del algoritmo. Para ello lo que haré será declarar las matrices justo aquí con la cantidad de memoria
     * justa y necesaria para el algoritmo. También hay que tener en cuenta que quizá luego SÍ necesitemos alguna
     * variable global, pero el objetivo siempre será minimizarlas para minimizar al máximo las situaciones de
     * sincronización, haciendo al algoritmo más rápido y a la implementación más sencilla.
     */

    int A[nn][nn], B[nn][nn], C[nn][nn], C1[nn][nn], C2[nn][nn], VBcol[N];
    bzero(A, sizeof(int) * (nn * nn));
    bzero(B, sizeof(int) * (nn * nn));
    bzero(C, sizeof(int) * (nn * nn));
    bzero(C1, sizeof(int) * (nn * nn));
    bzero(C2, sizeof(int) * (nn * nn));


    Vector AD[nnD], BD[nnD];
    for(k = 0; k < nnD; k++)
    {
        AD[k].i = rand() % (nn - 1);
        AD[k].j = rand() % (nn - 1);
        AD[k].v = rand() % 100;

        while (A[AD[k].i][AD[k].j])
        {
            if(AD[k].i < AD[k].j)
            {
                AD[k].i = (AD[k].i + 1) % nn;
            }
            else
            {
                AD[k].j = (AD[k].j + 1) % nn;
            }
        }
        A[AD[k].i][AD[k].j] = AD[k].v;
    }

    for(k = 0; k < nnD; k++)
    {
        BD[k].i = rand() % (nn - 1);
        BD[k].j = rand() % (nn - 1);
        BD[k].v = rand() % 100;

        /**
         * Inicializamos la matriz BD en cada posición con dos coordenadas para indexar en la matriz y un valor
         * comprendido entre 0 y 99. Entramos en el bucle si no hay ceros en la matriz B. Si los hay, nos
         * quedamos iterando hasta que encontremos uno. Mientras estemos en el bucle, lo que haremos será ir
         * desplazándonos +1 en los dos ejes de la matriz B empezando desde 0 si nos salimos del rango en algun
         * eje dependiendo de cual eje haya obtenido el mayor número aleatorio.
         * Finalmente cuando encontramos un 0 salimos del bucle y guardamos nuestro valor en esa posición,
         * sobreeescribiendo el cero que había previamente.
         */
        while (B[BD[k].i][BD[k].j])
        {
            if(BD[k].i < BD[k].j)
            {
                BD[k].i = (BD[k].i + 1) % nn;
            }
            else
            {
                BD[k].j = (BD[k].j + 1) % nn;
            }
        }
        B[BD[k].i][BD[k].j] = BD[k].v;
    }
 
    //Sparce Matrix X Dense Matrix -> Dense Matrix
    for(i = 0; i < nn; i++)
    {
        for (k = 0; k < nnD; k++)
        {
            C1[AD[k].i][i] += AD[k].v * B[AD[k].j][i];
        }
    }

            
    //Sparce Matrix X Sparce Matrix -> Dense Matrix
    for (j = 0; j < nn; j++)
    {
        VBcol[j] = 0;
    }

    for(i = 0; i < nn; i++)
    {
        // Column expansion of B[*][i]
        for (k = 0; k < nnD; k++)
        {
            if (BD[k].j == i)
            {
                VBcol[BD[k].i] = BD[k].v;
            }
        }

        // Full column C computation
        for (k = 0; k < nnD; k++)
        {
            C2[AD[k].i][i] += AD[k].v * VBcol[AD[k].j];
        }
        // Clear of  B[*][i]
        for (j = 0; j < nn; j++)
        {
            VBcol[j] = 0;
        }
    }

    // VALIDATION
    // Check (MD x M -> M) vs (MD x MD -> M)
    int neleC = 0;
    long long Sum = 0;
    for (i = 0; i < nn; i++)
    {
        for(j = 0; j < nn; j++)
        {
            if (C2[i][j] != C1[i][j])
            {
                printf("Not-Equal C1 & C2 pos %d,%d: %d != %d\n", i, j, C1[i][j], C2[i][j]);
            }
            if (C1[i][j])  // Some Value
            {
                Sum += C1[i][j];
                neleC++;
            }
        }
    }


    printf ("\nNumber of elements in C %d", neleC);
    printf("\nChecksum of C %lld ", Sum);
    printf("\n");
}
