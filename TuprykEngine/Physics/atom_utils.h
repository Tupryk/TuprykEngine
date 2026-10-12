#ifndef ATOM_SIM_UTILS
#define ATOM_SIM_UTILS

#include "atom.h"

void atoms_clip_vel(struct AtomSim* ps);
void atoms_clip_pos(struct AtomSim* ps, float bounds);

#endif
