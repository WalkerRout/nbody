#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdint.h>

#include "particle.h"

#define MAX_TAIL_LEN 50
#define TAIL_GROW_TICKS 120

/// An n-body simulation of particles
typedef struct simulation {
  // simulation tick count, starting at 0
  size_t ticks;

  // must be >0, never changes after initialization
  size_t particles_len;
  // we do not own particles, this memory will NEVER be free'd by simulation
  particle_t *particles;

  // particles_len x 1
  double *acceleration_deltas;

  // particles_len x particles_len
  v2d_t *force_vectors;

  // particles_len x particles_len
  double *radii;

  // 1 by default
  size_t tail_len;
  // particles_len x tail_len
  v2d_t *tail;
} simulation_t;

/// Initialize a new simulation with the given particles and no tail
void simulation_init(simulation_t *simulation, particle_t *particles, size_t particles_len);

/// Initialize a new simulation with the given particles and tail length
void simulation_init_tail(
  simulation_t *simulation,
  particle_t *particles,
  size_t particles_len,
  size_t tail_len
);

/// Free a simulation's memory, effectively destroying it
void simulation_free(simulation_t *simulation);

/// Update this simulation by one tick
void simulation_tick(simulation_t *simulation, double delta_time);

#endif // SIMULATION_H