#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <strings.h>
#include <assert.h>

// No son necesarios los defines tras el refactor
typedef struct Vector
{
    int i, j, v;
} Vector;

/**
 * Eliminadas variables globales individuales. Este tipo de variable se mapea obligatoriamente en memoria por lo que
 * se pierde tiempo en accederlas. Por supuesto, se benefician de las ventajas de la caché, pero esto no puede
 * competir con una variable que puede estar mapeada en los registros del procesador.
 **/

void init(unsigned int dim, int * A, Vector * AD)
{
    for(unsigned int k = 0; k < (dim * dim / 100); k++)
    {
        AD[k].i = rand() % (dim - 1);
        AD[k].j = rand() % (dim - 1);
        AD[k].v = rand() % 100;

        /**
         * Inicializamos la matriz BD en cada posición con dos coordenadas para indexar en la matriz y un valor
         * comprendido entre 0 y 99. Entramos en el bucle si no hay ceros en la matriz B. Si los hay, nos
         * quedamos iterando hasta que encontremos uno. Mientras estemos en el bucle, lo que haremos será ir
         * desplazándonos +1 en los dos ejes de la matriz B empezando desde 0 si nos salimos del rango en algun
         * eje dependiendo de cual eje haya obtenido el mayor número aleatorio.
         * Finalmente cuando encontramos un 0 salimos del bucle y guardamos nuestro valor en esa posición,
         * sobreeescribiendo el cero que había previamente. Aparentemente la postcondición del código es que toda la
         * matriz debe estar inicializada con números diferentes de cero, pero esto no es necesariamente así ya que
         * el valor aleatorio que guardamos al final de cada iteración es un valor entre 0 i 99 ambos incluidos.
         */
        while (A[AD[k].i * dim + AD[k].j])
        {
            if(AD[k].i < AD[k].j)
            {
                AD[k].i = (AD[k].i + 1) % dim;
            }
            else
            {
                AD[k].j = (AD[k].j + 1) % dim;
            }
        }
        A[AD[k].i * dim + AD[k].j] = AD[k].v;
    }
}


int main(int np, char *p[])
{

    int i, j, k;
    assert(np == 2);  // Check number of parameters

    unsigned int dim = atoi(p[1]);

    assert(dim <= 10000);  // Check that the first number is less or equal to N
    srand(1);  // Initialize fixed random seed for reproducible results (in the checksum of C)

    /**
     * Usamos solamente dim * dim * int por cada matriz en memoria, donde dim es menor o igual a 10000, eso es como
     * máximo 10000 * 10000  * 4 Bytes = 4 * 10 ^ 8 Bytes de memoria (por matriz) que es exactamente lo que reservamos
     * de forma estática al principio del código, sin embargo, dim no siempre será el máximo, por lo que estamos
     * reservando más memoria de la necesaria 9999 veces de cada 10000 variaciones posibles en nuestro parámetro durante
     * la ejecución del algoritmo. Para ello lo que haré será declarar las matrices justo aquí con la cantidad de
     * memoria justa y necesaria para el algoritmo. También hay que tener en cuenta que quizá luego SÍ necesitemos
     * alguna variable global, pero el objetivo siempre será minimizarlas para minimizar al máximo las situaciones de
     * sincronización, haciendo al algoritmo más rápido y a la implementación más sencilla.
     **/


    /**
     * Declaramos las variables aquí, almacenándolas en la pila, gastando un total de 4 (Bytes / elem) * dim (elem / col)
     * * dim (col) * matriz (1 / matriz) * 5 (matriz) + 4 (Bytes / elem) * dim (elem) = 20 * dim ^ 2 + 4 * dim Bytes.
     * Para dim = 10000 que es lo máximo que nos pueden dar las matrices ocupan 20 * (10 ^ 4) ^ 2 + 4 * 10 ^ 4 Bytes ->
     * 2.00004 * 10 ^ 9 Bytes * (1 MB / (2 ^ 10 Bytes) ^ 2) =~ 1907 MB =~ 2 GB, lo cual puede DE SOBRAS hacernos petar
     * la pila y causarnos un segmentation fault. La solución es guardar las variables en otro segmento. Recordemos:
     * - Malloc, realloc: Heap, memoria de sobras.
     * - Variables locales: Stack, memoria bastante limitada.
     * - Variables estáticas: Segmento de datos (reservadas en tiempo de compilación)
     * - Variables globales: Segmento de datos (reservadas en tiempo de compilación)
     * - Variables globales o estáticas no inicializadas: Segmento BSS.
     * Por tanto si usamos el Stack sufrimos stack overflow que se traduce en segmentation fault, si usamos variables
     * estáticas o globales seguimos teniendo el mismo problema y es que debemos declarar las variables como el
     * máximo que vamos a usar, por lo tanto nuestra única opción para optimizar memoria es usar variables reservadas
     * con malloc.
     **/

    /**
     * Si hacemos calloc nos ahorramos las inicializaciones a 0... Además la variable C NO se usa así que adiós.
     **/
    int * A = (int *)calloc(dim * dim, sizeof(int)),
        * B = (int *)calloc(dim * dim, sizeof(int));

    Vector * AD = (Vector *)malloc(sizeof(Vector) * (dim * dim / 100));
    Vector * BD = (Vector *)malloc(sizeof(Vector) * (dim * dim / 100));

    init(dim, A, AD);
    init(dim, B, BD);

    /**
     * La matriz AD y BD (aparentemente) se usa como almacén de índices aleatorios para acceder a las posiciones
     * de las demás matrices, por lo que la posición que solicitamos no está determinada, sinó que es random, entonces
     * la dependencia de datos es hacia toda la matriz AD y BD si usamos los valores en AD y/o BD para índexar en las
     * demás matrices. Por ejemplo, aquí hay dependencia de lectura y de escritura de la matriz C1 entera, ya que no
     * sabemos donde apuntará el índice de AD[k].i
     */
    //Sparce Matrix X Dense Matrix -> Dense Matrix
    int * C1 = (int *)calloc(dim * dim, sizeof(int));
    for (i = 0; i < dim; i++)
    {
        for (k = 0; k < (dim * dim / 100); k++)
        {
            C1[AD[k].i * dim + i] += AD[k].v * B[AD[k].j * dim + i];
        }
    }

            
    //Sparce Matrix X Sparce Matrix -> Dense Matrix
    // Calloc para ahorrarnos código.
    int * VBcol = calloc(dim, sizeof(int));
    int * C2 = (int *)calloc(dim * dim, sizeof(int));
    for (i = 0; i < dim; i++)
    {
        // Column expansion of B[*][i]
        for (k = 0; k < (dim * dim / 100); k++)
        {
            if (BD[k].j == i)
            {
                VBcol[BD[k].i] = BD[k].v;
            }
        }
        /**
         * This second for only needs VBcol for reading, so we can fork the program here and pass a copy of VBcol to
         * compute this for while the other for block reinitializes VBcol to 0 with the original VBcol data
         */
        // Full column C computation
        for (k = 0; k < (dim * dim / 100); k++)
        {
            C2[AD[k].i * dim + i] += AD[k].v * VBcol[AD[k].j];
        }
        // Clear of  B[*][i]
        for (j = 0; j < dim; j++)
        {
            VBcol[j] = 0;
        }
    }

    // VALIDATION
    // Check (MD x M -> M) vs (MD x MD -> M)
    /**
     * the two ifs in this clode block are totally independent from each other, so one thread can do the loop
     * with one if and the other will do the loop with the other if. Also we can convert the first if to a
     * reduced clausule of boolean accumulation
     */
    int neleC = 0;
    long long Sum = 0;
    for (i = 0; i < dim; i++)
    {
        for (j = 0; j < dim; j++)
        {
            if (C2[i * dim + j] != C1[i * dim + j])
            {
                printf("Not-Equal C1 & C2 pos %d, %d: %d != %d\n", i, j, C1[i * dim + j], C2[i * dim + j]);
            }
            if (C1[i * dim + j])  // Some Value
            {
                Sum += C1[i * dim + j];
                neleC++;
            }
        }
    }


    printf ("\nNumber of elements in C %d", neleC);
    printf("\nChecksum of C %lld ", Sum);
    printf("\n");
}
