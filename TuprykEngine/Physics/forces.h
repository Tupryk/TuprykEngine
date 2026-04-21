#ifndef FORCES
#define FORCES

#include "../LinAlg/tensor.h"
#include "../Kinematics/configuration.h"


stack* forces_on_body(config* C, int root);
float center_of_mass(config* C, int root, tensor* com);
// void centroidal_forces(config* C, int root, tensor* force, tensor* torque);

#endif
