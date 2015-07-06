// compiled with: gcc csparse_test.c csparse.c -lm -o csparse_test

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "csparse.h"

int main(int argc, char const *argv[])
{
    cs *A = cs_spalloc(3, 3, 9, 1, 1);

    int i;
    for(i = 0; i < 3; i++) cs_entry(A, i, i, 1);
    A = cs_triplet(A);
    cs_print(A, 1);

    /*cs *B = cs_triplet(cs_multiply(A,A));*/
    cs *B = cs_multiply(A,A);
    /*cs_print(B, 0);*/

    A = cs_spalloc(3, 3, 9, 1, 1);
    cs_entry(A, 0, 0, 6);
    cs_entry(A, 0, 1, -5);
    cs_entry(A, 1, 0, -2);
    cs_entry(A, 1, 1, 1);

    double b[2];
    b[0] = 1; b[1] = 1;
    cs_usolve(A, b);
    for(i = 0; i < 2; i++)
        printf("%f\n",b[i]);

    cs_spfree(A);
    cs_spfree(B);
}
