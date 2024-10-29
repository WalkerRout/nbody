#ifndef PARTICLE_H
#define PARTICLE_H

#include "mvla.h"

/// A particle in space centered about some position
typedef struct particle {
  double mass;
  v2d_t position;
  v2d_t velocity;
  v2d_t acceleration;
} particle_t;

#endif // PARTICLE_H