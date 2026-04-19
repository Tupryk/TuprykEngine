#ifndef FORCES
#define FORCES

#include "../LinAlg/tensor.h"
#include "../Kinematics/configuration.h"

float center_of_mass(config* C, int root, tensor* com);
// void centroidal_forces(config* C, int root, tensor* force, tensor* torque);

#endif
