/**
 * Last test to test Evolution under constand work
 * (every function runns in O(n))
 */
#include "../src/evolution.h"
#include "../src/C-Utils/Rand/src/rand.h"
#include <time.h>

typedef struct {
  int length;
  uint32_t rand;
} ThreadArgs;

void *init_v(void *opts) {

  ThreadArgs *args = opts;
  int i;

  int *ary = malloc(sizeof(int) * args->length);

  for (i = 0; i < args->length; i++)
    ary[i] = rand32(&args->rand) - (RAND_MAX / 2);

  return ary;
}

void clone_v(void *dst, void *src, void *opts) {

  ThreadArgs *args = opts;
  int i;

  for (i = 0; i < args->length; i++)
    ((int *) dst)[i] = ((int *) src)[i];
}

void free_v(void *dst, void *opts) {
  (void) opts;
  free((int *) dst);
}

void recombinate_v(Individual *src1, 
                   Individual *src2, 
                   Individual *dst, 
                   void *opts) {

  int i;
  ThreadArgs *args = opts;
  int *vs1 = (int *) src1->iv;
  int *vs2 = (int *) src2->iv;
  int *vd  = (int *) dst->iv;

  for (i = 0; i < args->length; i++)
    vd[i] = (rand32(&args->rand) % 2) ? vs1[i] : vs2[i];
  
}

void mutate_v(Individual *src, void *opts) {

  ThreadArgs *args = opts;
  int i, r, *ary = src->iv;

  for (i = 0; i < args->length; i++) {
    r = (rand32(&args->rand) % 3) - 1;

    ary[i] = (rand32(&args->rand) % 100) ? ary[i] : ary[i] + r;
  }

}

int64_t fittnes_v(Individual *src, void *opts) {

  ThreadArgs *args = opts;
  int i, *ary = src->iv;
  int64_t max = 0;

  for (i = 0; i < args->length; i++)
    max += (ary[i] < 0) ? ary[i] * -1 : ary[i];

  return max;
}

int main(int argc, char *argv[]) {

  if (argc != 6) {
    printf("%s <num generations> <num threads> <verbose level(0-3)> "
           "<num ivs> <length>\n", 
           argv[0]);
    exit(1);
  }

  int length = atoi(argv[5]);
  int verbose = EV_VEB0;
  int  n_threads = atoi(argv[2]);


  switch (atoi(argv[3])) {
    case 1:   verbose = EV_VEB1; break;
    case 2:   verbose = EV_VEB2; break;
    case 3:   verbose = EV_VEB3; break;
    default : verbose = EV_VEB0; 
  }

  Individual *best;
  ThreadArgs **opts = malloc(sizeof(ThreadArgs *) * n_threads);
  int i;
  for (i = 0; i < n_threads; i++) {
    opts[i] = malloc(sizeof(ThreadArgs));
    opts[i]->length = length;
    opts[i]->rand   = time(NULL) ^ i;
  }

  EvInitArgs args;
  args.init_iv              = init_v;
  args.clone_iv             = clone_v;
  args.free_iv              = free_v;
  args.mutate               = mutate_v;
  args.fitness              = fittnes_v;
  args.recombinate          = recombinate_v;
  args.continue_ev          = NULL;
  args.population_size      = atoi(argv[4]);
  args.generation_limit     = atoi(argv[1]);
  args.mutation_propability = 1.0;
  args.death_percentage     = 0.5;
  args.opts                 = (void **) opts;
  args.num_threads          = atoi(argv[2]);
  args.flags                = EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP|verbose;

  Evolution *ev = new_evolution(&args);
  best = evolute(ev);

  #ifndef NO_OUTPUT
    for (i = 0; i< length; i++) 
      printf("%d\n", *(int *) best->iv);
  #else
  (void) best;
  #endif

  return 0;
}

