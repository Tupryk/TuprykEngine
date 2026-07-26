#ifndef FOAM
#define FOAM

#include "../nlp.h"
#include "../../../Algos/lists.h"
#include "../../../LinAlg/tensor.h"


typedef struct
{
    // TODO: Move this to Stochastic/
    float scaling;
    tensor_t* mu;
    tensor_t* cov;
} gaussian_t;

typedef struct
{
    // TODO: Use spatial data-structures
    int dim;
    float inital_std;
    float inital_scaling;
    size_t max_foam;
    stack* force_fields;  // type: gaussian_t
} foam_t;

foam_t* foam_init(nlp_t* nlp, size_t max_foam, float inital_std, float inital_scaling);
float foam_eval(foam_t* foam, nlp_t* nlp, tensor_t* x);
void foam_eval2(foam_t* foam, nlp_t* nlp, tensor_t* x, tensor_t* out);
void foam_add_particle(foam_t* foam, tensor_t* x);
void foam_free(foam_t* foam);

#endif
