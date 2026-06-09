#ifndef NLP_GRAPHICS
#define NLP_GRAPHICS

#include <math.h>
#include "../../LinAlg/tensor.h"
#include "../../Optim/constrained/nlp.h"
#include "../../Optim/constrained/sampling/foam.h"


void draw_nlp(nlp_t* nlp, float square_side);
void draw_nlp_foam(nlp_t* nlp, foam_t* foam, float square_side, int mark_gaussians);

#endif
