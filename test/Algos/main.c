#define DEBUG

#include <stdio.h>
#include <stdlib.h>

#include "../../TuprykEngine/Algos/lists.h"
#include "../../TuprykEngine/Algos/spatial_data.h"

#include "../../TuprykEngine/ui/prints/algos.h"
#include "../../TuprykEngine/ui/graphics/basic.h"
#include "../../TuprykEngine/ui/graphics/window.h"


int test_vector()
{
    vector v = vector_create(sizeof(int));

    for (int i = 0; i < 10; i++)
    {
        vector_push(&v, &i);
    }

    print_int_vector(&v);

    vector_free(&v);

    return 0;
}

int test_int_stack()
{
    int_stack_t* s = int_stack_init();

    for (int i = 0; i < 10; i++)
    {
        int_stack_push(s, i);
    }

    print_int_stack(s);
    printf("Contains 5?: %d\n", int_stack_contains(s, 5));
    printf("Contains 11?: %d\n", int_stack_contains(s, 11));

    for (int i = 0; i < 10; i++)
    {
        printf("%d\n", int_stack_pop(s));
    }

    print_int_stack(s);

    int_stack_free(s);
 
    return 0;
}

int test_stack()
{
    pstack_t* s = stack_init();

    int* v1 = malloc(sizeof(int));
    v1[0] = 1;

    int* v2 = malloc(sizeof(int));
    v2[0] = 2;

    int* v3 = malloc(sizeof(int));
    v3[0] = 3;

    stack_push(s, v1);
    stack_push(s, v2);
    stack_push(s, v3);

    for (int i = 0; i < 3; i++)
    {
        int v = *(int*) stack_pop_at_index(s, s->size-1);
        printf("%d\n", v);
    }

    free(v1);
    free(v2);
    free(v3);
    stack_free(s);
    return 0;
}

int test_kd_tree()
{
    int k = 100;
    int indices[k];
    float dists[k];
    int total_points = 1000;
    tensor_t* points[total_points];
    for (int i = 0; i < total_points; i++)
    {
        points[i] = new_tensor_vector(2, NULL);
        tensor_fill_uniform(points[i], -1.f, 1.f);
    }
    
    kd_tree_t* kt = kd_tree_init(points, total_points);
    tensor_t* query = new_tensor_vector(2, NULL);
    kd_tree_knn(kt, query, k, indices, dists);
    tensor_free(query);
    kd_tree_free(kt);

    printf("indices: [");
    for (int i = 0; i < k; i++)
    {
        printf("%d, ", indices[i]);
    }
    printf("]\n");

    init_window();
    set_color(1.f, 1.f, 1.f);
    window_clear();

    for (int i = 0; i < total_points; i++)
    {
        set_color(0.1216f, 0.4667f, 0.7059f);
        for (int j = 0; j < k; j++)
        {
            if (indices[j] == i)
            {
                set_color(1.0f, 0.4980f, 0.0549f);
                break;
            }
        }

        int cx = ((points[i]->values[0] + 1.f) * 0.5f) * ((float) WINDOW_W);
        int cy = ((-points[i]->values[1] + 1.f) * 0.5f) * ((float) WINDOW_H);
        draw_circle(cx, cy, 2);

        tensor_free(points[i]);
    }

    window_wait();
    free_window();

    return 0;
}

int main()
{
    // test_vector();
    // test_int_stack();
    // test_stack();
    test_kd_tree();
    return 0;
}
