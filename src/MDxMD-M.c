#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <strings.h>
#include <assert.h>

#define N 10000L
#define ND N*N/100

int A[N][N],B[N][N],C[N][N],C1[N][N],C2[N][N],VBcol[N];
struct {
    int i,j,v;
}AD[ND],BD[ND];

int nn,nnD;
long long Sum;

int main(int np, char *p[])
{
    int i,j,k,neleC;

    assert(np==2);

    nn = atoi(p[1]);
    assert(nn<=N);
    srand(1);

    nnD = nn*nn/100;

    bzero(A,sizeof(int)*(nn*nn));
    bzero(B,sizeof(int)*(nn*nn));
    bzero(C,sizeof(int)*(nn*nn));
    bzero(C1,sizeof(int)*(nn*nn));
    bzero(C2,sizeof(int)*(nn*nn));
     
    for(k=0;k<nnD;k++)
    {
        AD[k].i=rand()%(nn-1);
        AD[k].j=rand()%(nn-1);
        AD[k].v=rand()%100;
        while (A[AD[k].i][AD[k].j]) {
            if(AD[k].i < AD[k].j)
                AD[k].i = (AD[k].i + 1)%nn;
            else 
                AD[k].j = (AD[k].j + 1)%nn;
        }
        A[AD[k].i][AD[k].j] = AD[k].v;
    }

    for(k=0;k<nnD;k++)
    {
        BD[k].i=rand()%(nn-1);
        BD[k].j=rand()%(nn-1);
        BD[k].v=rand()%100;
        while (B[BD[k].i][BD[k].j]) {
            if(BD[k].i < BD[k].j)
                BD[k].i = (BD[k].i + 1)%nn;
            else 
                BD[k].j = (BD[k].j + 1)%nn;
        }
        B[BD[k].i][BD[k].j] = BD[k].v;
    }

    ////Original Matrix X Matrix (column first)
    //for (i=0;i<nn;i++)
    //    for (j=0;j<nn;j++)
    //        for (k=0;k<nn;k++)
    //            C[j][i] += A[j][k] * B[k][i];
 
    //Sparce Matrix X Dense Matrix -> Dense Matrix
    for(i=0;i<nn;i++)
        for (k=0;k<nnD;k++)
            C1[AD[k].i][i] += AD[k].v * B[AD[k].j][i];
            
    //Sparce Matrix X Sparce Matrix -> Dense Matrix
    for (j=0;j<nn;j++)
        VBcol[j] = 0;

    for(i=0;i<nn;i++)
      {
        // Column expansion of B[*][i]
        for (k=0;k<nnD;k++)
            if (BD[k].j == i)
                VBcol[BD[k].i] = BD[k].v;
        // Full column C computation
        for (k=0;k<nnD;k++)
            C2[AD[k].i][i] += AD[k].v * VBcol[AD[k].j];
        // Clear of  B[*][i]
        for (j=0;j<nn;j++)
            VBcol[j] = 0;
      }
                
    // Check (MD x M -> M) vs (MD x MD -> M)
    neleC = 0;
    Sum = 0;
    for (i=0;i<nn;i++)
        for(j=0;j<nn;j++)
	  {
            if (C2[i][j] != C1[i][j])
                printf("Not-Equal C1 & C2 pos %d,%d: %d != %d\n",i,j,C1[i][j],C2[i][j]);
	    if (C1[i][j])  // Some Value
	      {
		Sum += C1[i][j];
		neleC++;
	      }
	  }

    printf ("\nNumber of elements in C %d\n",neleC);   
    printf("Checksum of C %lld \n",Sum);
}
