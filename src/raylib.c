#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <raylib.h>

#define MVLA_IMPLEMENTATION
#include "mvla.h"
#undef  MVLA_IMPLEMENTATION

#include "particle.h"
#include "simulation.h"

#define WIDTH 1000
#define HEIGHT 800

/// We would like to normalize the momentum of the particles for this simulation
void adjust_particle_momentum(particle_t *particles, size_t particles_len) {
  assert(particles != NULL);

  double momentum_x = 0.0, momentum_y = 0.0, total_mass = 0.0;

  for (size_t i = 0; i < particles_len; ++i) {
    momentum_x += particles[i].velocity.x * particles[i].mass;
    momentum_y += particles[i].velocity.y * particles[i].mass;
    total_mass += particles[i].mass;
  }

  for (size_t i = 0; i < particles_len; ++i) {
    particles[i].velocity.x -= momentum_x / total_mass;
    particles[i].velocity.y -= momentum_y / total_mass;
  }
}

/// Creates and returns array of particles
particle_t *create_particles(size_t *out_particles_len) {
  assert(out_particles_len != NULL);

  *out_particles_len = 8;

  particle_t *particles = calloc(*out_particles_len, sizeof(particle_t));
  if (particles == NULL) {
    fprintf(stderr, "Failed to allocate memory for particles.\n");
    exit(EXIT_FAILURE);
  }

  double n = 0.0;
  double gap = 50.0;
  particles[0] = (particle_t){
    .mass = 1000.0 * 1000.0,
    .position = v2d(GetRenderWidth() / 2.0, GetRenderHeight() / 2.0 + n++*gap),
    .velocity = v2dd(0.0),
    .acceleration = v2dd(0.0),
  };
  particles[1] = (particle_t){
    .mass = 2000.0,
    .position = v2d(GetRenderWidth() / 2.0, GetRenderHeight() / 2.0 + n++*gap),
    .velocity = v2d(140.0, 0.0),
    .acceleration = v2dd(0.0),
  };
  particles[2] = (particle_t){
    .mass = 1000.0,
    .position = v2d(GetRenderWidth() / 2.0, GetRenderHeight() / 2.0 + n++*gap),
    .velocity = v2d(140.0 / sqrt(1.5), 0.0),
    .acceleration = v2dd(0.0),
  };
  particles[3] = (particle_t){
    .mass = 700.0,
    .position = v2d(GetRenderWidth() / 2.0, GetRenderHeight() / 2.0 + n++*gap),
    .velocity = v2d(140.0 / sqrt(2.0), 0.0),
    .acceleration = v2dd(0.0),
  };
  particles[4] = (particle_t){
    .mass = 1000.0,
    .position = v2d(GetRenderWidth() / 2.0, GetRenderHeight() / 2.0 + n++*gap),
    .velocity = v2d(140.0 / sqrt(3.0), 0.0),
    .acceleration = v2dd(0.0),
  };
  particles[5] = (particle_t){
    .mass = 1000.0,
    .position = v2d(GetRenderWidth() / 2.0, GetRenderHeight() / 2.0 + n++*gap),
    .velocity = v2d(140.0 / 2.0, 0.0),
    .acceleration = v2dd(0.0),
  };
  particles[6] = (particle_t){
    .mass = 1.0,
    .position = v2d(GetRenderWidth() / 2.0, GetRenderHeight() / 2.0 + n++*gap),
    .velocity = v2d(150.0 / 2.0, 0.0),
    .acceleration = v2dd(0.0),
  };
  particles[7] = (particle_t){
    .mass = 1000.0,
    .position = v2d(GetRenderWidth() / 2.0, GetRenderHeight() / 2.0 + n++*gap),
    .velocity = v2d(140.0 / (2.0 * sqrt(2.0)), 0.0),
    .acceleration = v2dd(0.0),
  };

  adjust_particle_momentum(particles, *out_particles_len);

  return particles;
}

/// Raylib specific rendering for the simulation
void draw_simulation(const simulation_t *const sim) {
  assert(sim != NULL);

  // kinda scuffed, but this is a tandem list with sim->particles -> particle radius
  double particle_rads[] = {10.0, 6.0, 4.0, 3.0, 4.0, 4.0, 1.0, 4.0};

  BeginDrawing();
  ClearBackground(BLACK);
  DrawFPS(10, 10);
  for (size_t p = 0; p < sim->particles_len; ++p) {
    for (size_t t = 0; t < sim->tail_len; ++t) {
      size_t curr = p * MAX_TAIL_LEN + t;
      double x = sim->tail[curr].x;
      double y = sim->tail[curr].y;
      DrawCircle(x, y, 2, GRAY);
      DrawCircle(x, y, 2, GRAY);
    }
    DrawCircle(
      sim->particles[p].position.x, sim->particles[p].position.y, 
      particle_rads[p],
      WHITE
    );
  }
  EndDrawing();
}

int main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  // --- begin rendering simulation ---

  // create window
  InitWindow(WIDTH, HEIGHT, "nbody");
  SetTargetFPS(120);

  // create particles
  size_t particles_len;
  particle_t *particles = create_particles(&particles_len);

  // create simulation
  simulation_t sim = {0};
  simulation_init(&sim, particles, particles_len);

  // run simulation
  while (!WindowShouldClose()) {
    double dt = GetFrameTime();

    // reset simulation when 'R' key is pressed
    if (IsKeyPressed(KEY_R)) {
      // clear the structures holding our state
      simulation_free(&sim);
      free(particles);
      // create particles, then create sim with new particles
      particles = create_particles(&particles_len);
      simulation_init(&sim, particles, particles_len);
    }

    simulation_tick(&sim, dt);
    draw_simulation(&sim);
  }

  // close window
  CloseWindow();

  // free simulation
  simulation_free(&sim);

  // free particles
  free(particles);

  return EXIT_SUCCESS;
}