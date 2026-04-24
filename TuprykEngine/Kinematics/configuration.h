#ifndef CONFIGURATION
#define CONFIGURATION

#include "frame.h"
#include "../Algos/lists.h"
#include "../LinAlg/tensor.h"


typedef struct
{
    int frame_id;
    tensor* force;
    tensor* torque;
    tensor* poa;
} force_t;  // TODO: Should maybe be in a different filem ie. Physics/forces.h

typedef struct
{
    int body_a;
    int body_b;

    tensor* point;
    tensor* normal;
    float depth;

    float accumulated_impulse;
} contact_t;

typedef struct
{
    int frame_count;
    frame** frames;  // Frame at index 0 is always root!!!

    int joints_count;
    int* joints;

    int lights_count;
    int* lights;
    
    tensor* q;
    tensor* q_vel;
    tensor* q_max;
    tensor* q_min;

    stack* forces;  // force_t
    tensor* gravity;
} config;

int config_colliding(config* C);
stack* config_get_contacts(config* C);
void contacts_free(stack* s);
void config_compute_contacts(config* C);
void config_populate_mass_inertias(config* C);
void config_q_delta_from_impulse(config* C, int from_frame_id, tensor* impulse_world, tensor* poa_world, tensor* q_delta);
void velocity_at_point(config* C, tensor* point, int frame_id, tensor* vel);
void config_free_contacts(config* C);
void config_set_q(config* C, float* q);
void config_update_q(config* C);
void config_empty_joints_accumulated_forces(config* C);
void impulse_to_joints_force(config* C, int from_frame_id, tensor* impulse_world, tensor* poa_world);
void config_free(config* C);
void force_free(force_t* f);
void forces_add(force_t* a, force_t* b, force_t* out);
int root_joint(config* C, int frame_id);

#endif
