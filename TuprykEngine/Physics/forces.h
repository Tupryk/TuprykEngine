#ifndef FORCES
#define FORCES

#include "../LinAlg/tensor.h"
#include "../Kinematics/configuration.h"


void combined_inertia(config_t* C, int root, tensor_t* com, tensor_t* I_cm);
float center_of_mass(config_t* C, int root, tensor_t* com);
// void centroidal_forces(config_t* C, int root, tensor_t* force, tensor_t* torque);
tensor_t* compute_mass_matrix(config_t* C);

#endif
