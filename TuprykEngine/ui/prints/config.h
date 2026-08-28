#ifndef PRINT_CONFIG
#define PRINT_CONFIG

#include "../../Kinematics/configuration.h"

void print_frame(config_t* C, frame_t* f, int id, int depth);
void print_config(config_t* C);

#endif
