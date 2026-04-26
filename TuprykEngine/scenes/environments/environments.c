#include "environments.h"

env_return_t* env_step(env_t* e)
{
    e->time++;

    env_return_t er;
    er->reward = 0.f;
    er->done = 0;
    er->state = e->state;
    return er;
}
