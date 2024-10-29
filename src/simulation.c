#include <stdlib.h>
#include <assert.h>

#include "mvla.h"

#include "simulation.h"

/// Add particles to tail based on current tick position
static inline void adjust_tail(simulation_t *sim);
/// Update particle accelerations
static inline void adjust_deltas(simulation_t *sim);
/// Update particle velocity and position
static inline void adjust_particles(simulation_t *sim, double dt);

void simulation_init(simulation_t *sim, particle_t *particles, size_t particles_len) {
  assert(sim != NULL);
  simulation_init_tail(sim, particles, particles_len, 1);
}

void simulation_init_tail(
  simulation_t *sim,
  particle_t *particles,
  size_t particles_len,
  size_t tail_len
) {
  assert(sim != NULL);
  assert(tail_len > 1 && tail_len <= MAX_TAIL_LEN && "tail len invalid");
  assert(particles_len > 0 && particles != NULL && "particles invalid");

  sim->ticks = 0;

  sim->particles_len = particles_len;
  sim->particles = particles;

  sim->acceleration_deltas = calloc(particles_len, sizeof(double));
  if (sim->acceleration_deltas == NULL) {
    fprintf(stderr, "Failed to allocate memory for acceleration_deltas.\n");
    exit(EXIT_FAILURE);
  }

  sim->force_vectors = calloc(particles_len * particles_len, sizeof(v2d_t));
  if (sim->force_vectors == NULL) {
    fprintf(stderr, "Failed to allocate memory for force_vectors.\n");
    exit(EXIT_FAILURE);
  }

  sim->radii = calloc(particles_len * particles_len, sizeof(double));
  if (sim->radii == NULL) {
    fprintf(stderr, "Failed to allocate memory for radii.\n");
    exit(EXIT_FAILURE);
  }

  if (tail_len > 0) {
    sim->tail_len = tail_len;
    sim->tail = calloc(particles_len * MAX_TAIL_LEN, sizeof(v2d_t));
    if (sim->tail == NULL) {
      fprintf(stderr, "Failed to allocate memory for tail.\n");
      exit(EXIT_FAILURE);
    }
  } else {
    sim->tail_len = 1;
    sim->tail = NULL;
  }
}

void simulation_free(simulation_t *sim) {
  assert(sim != NULL);
  free(sim->acceleration_deltas);
  free(sim->force_vectors);
  free(sim->radii);
  free(sim->tail);
}

void simulation_tick(simulation_t *sim, double delta_time) {
  assert(sim != NULL);
  adjust_tail(sim);
  adjust_deltas(sim);
  adjust_particles(sim, delta_time);
  sim->ticks += 1;
}

static inline void adjust_tail(simulation_t *sim) {
  assert(sim != NULL);
  
  // particles len should never change
  const size_t particles_len = sim->particles_len;

  if (sim->ticks % TAIL_GROW_TICKS && sim->tail_len < MAX_TAIL_LEN) {
    sim->tail_len += 1;
  }

  size_t tail_section = (sim->ticks+1) % sim->tail_len;
  for (size_t i = 0; i < particles_len; ++i) {
    sim->tail[i * MAX_TAIL_LEN + tail_section] = sim->particles[i].position;
  }
}

static inline void adjust_deltas(simulation_t *sim) {
  assert(sim != NULL);
  
  // particles len should never change
  const size_t particles_len = sim->particles_len;

  for (size_t i = 0; i < particles_len; ++i) {
    // reset acceleration to 0
    sim->particles[i].acceleration = v2dd(0.0);
    for (size_t j = 0; j < particles_len; ++j) {
      if (i == j) continue;
      size_t curr = i * particles_len + j;

      // for each particle i, find forces acting by particle j
      sim->force_vectors[curr] = 
        v2d_sub(sim->particles[j].position, sim->particles[i].position);

      // update particle interaction radii with new force vectors
      sim->radii[curr] = v2d_len(sim->force_vectors[curr]);

      // find acceleration deltas for acceleration update
      double curr_rad = sim->radii[curr];
      sim->acceleration_deltas[i] = 
        sim->particles[j].mass / (curr_rad*curr_rad*curr_rad);

      // scale new acceleration with deltas and forces
      v2d_t scaled_accel = 
        v2d_mul(v2dd(sim->acceleration_deltas[i]), sim->force_vectors[curr]);
      sim->particles[i].acceleration = 
        v2d_add(sim->particles[i].acceleration, scaled_accel);
    }
  }
}

static inline void adjust_particles(simulation_t *sim, double dt) {
  assert(sim != NULL);
  
  // particles len should never change
  const size_t particles_len = sim->particles_len;

  for (size_t i = 0; i < sim->particles_len; ++i) {
    sim->particles[i].velocity = 
      v2d_add(sim->particles[i].velocity, v2d_mul(v2dd(dt), sim->particles[i].acceleration));
    sim->particles[i].position = 
      v2d_add(sim->particles[i].position, v2d_mul(v2dd(dt), sim->particles[i].velocity));
  }
}