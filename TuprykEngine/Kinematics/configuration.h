#ifndef CONFIGURATION
#define CONFIGURATION

#include "frame.h"
#include "../Algos/lists.h"
#include "../LinAlg/tensor.h"


typedef struct
{
    int frame_id;
    tensor_t* force;
    tensor_t* torque;
    tensor_t* poa;
} force_t;  // TODO: Should maybe be in a different filem ie. Physics/forces.h

typedef struct
{
    int body_a;
    int body_b;

    tensor_t* point;
    tensor_t* normal;
    float depth;

    float accumulated_impulse;
} contact_t;

typedef struct
{
    int frame_count;
    frame_t** frames;  // Frame at index 0 is always root!!!

    int joints_count;
    int* joints;

    int lights_count;
    int* lights;
    
    tensor_t* q;
    tensor_t* q_vel;
    tensor_t* q_max;
    tensor_t* q_min;

    pstack_t* forces;  // force_t
    tensor_t* gravity;
} config;

int config_colliding(config* C);
pstack_t* config_get_contacts(config* C);
void contacts_free(pstack_t* s);
void config_compute_contacts(config* C);
void config_populate_mass_inertias(config* C);
void config_q_delta_from_impulse(config* C, int from_frame_id, tensor_t* impulse_world, tensor_t* poa_world, tensor_t* q_delta);
void velocity_at_point(config* C, tensor_t* point, int frame_id, tensor_t* vel);
void config_free_contacts(config* C);
void config_set_q(config* C, float* q);
void config_update_q(config* C);
void config_empty_joints_accumulated_forces(config* C);
void impulse_to_joints_force(config* C, int from_frame_id, tensor_t* impulse_world, tensor_t* poa_world);
void config_free(config* C);
void force_free(force_t* f);
void forces_add(force_t* a, force_t* b, force_t* out);
int root_joint(config* C, int frame_id);

#endif
