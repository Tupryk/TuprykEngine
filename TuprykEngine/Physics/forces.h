#ifndef FORCES
#define FORCES

#include "../Kinematics/Configuration.h"

void center_of_mass(config* C, int root, tensor* com, float* total_mass);
void centroidal_forces(config* C, int root, tensor* force, tensor* torque);

#endif
