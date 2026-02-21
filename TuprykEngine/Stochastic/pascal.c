#include "pascal.h"
#include <stdlib.h>


int** new_pascals_triangle(int row_count)
{
    int** tri = (int**) malloc( sizeof(int*) * row_count );
    tri[0] = (int*) malloc( sizeof(int) );
    tri[0][0] = 1;

    for (int i = 1; i < row_count; i++)
    {
        tri[i] = (int*) malloc( sizeof(int) * (i+1) );

        for (int j = 0; j < i+1; j++)
        {
            tri[i][j] = j == 0 || j == i ? tri[i-1][0] : tri[i-1][j-1] + tri[i-1][j];
        }
    }

    return tri;
}

void pascals_triangle_free(int** tri, int row_count)
{
    for (int i = 0; i < row_count; i++)
    {
        free(tri[i]);
    }
    free(tri);
}
