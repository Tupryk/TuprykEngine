#include "stoch.h"
#include <stdio.h>


void print_pascals_triangle(int** tri, int row_count)
{
    for (int i = 0; i < row_count; i++)
    {
        for (int j = 0; j < i+1; j++)
        {
            printf("%d ", tri[i][j]);
        }
        printf("\n");
    }
}
