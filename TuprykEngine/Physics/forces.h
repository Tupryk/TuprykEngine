#ifndef FORCES
#define FORCES

#include "../LinAlg/tensor.h"
#include "../Kinematics/configuration.h"


void combined_inertia(config* C, int root, tensor* com, tensor* I_cm);
float center_of_mass(config* C, int root, tensor* com);
// void centroidal_forces(config* C, int root, tensor* force, tensor* torque);
tensor* compute_mass_matrix(config* C);

#endif
