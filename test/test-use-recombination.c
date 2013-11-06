/**
 * Evolution test with Simple extended example fom Wikipedia: http://de.wikipedia.org/wiki/Evolutionärer_Algorithmus
 *
 * using an Vektor of ints as an Idividum
 */
#define MAX_U_INT32 4294967295L
#define TEST_NUM_IVS 100
#ifndef NULL
#define NULL 0
#endif

#include "../src/evolution.h"

typedef struct {
  int a, b, c, d, e, f;
} Vektor;

void *init_v(void *opts) {
  
  (void) opts;

  Vektor *v = (Vektor *) malloc(sizeof(Vektor));
  v->a = (rand() % 100) - 50;
  v->b = (rand() % 100) - 50;
  v->c = (rand() % 100) - 50;
  v->d = (rand() % 100) - 50;
  v->e = (rand() % 100) - 50;
  v->f = (rand() % 100) - 50;
  return v;
}

void clone_v(void *dst, void *src, void *opts) {
  
  (void) opts;

  Vektor *vd = (Vektor *) dst;
  Vektor *vs = (Vektor *) src;

  vd->a = vs->a;
  vd->b = vs->b;
  vd->c = vs->c;
  vd->d = vs->d;
  vd->e = vs->e;
  vd->f = vs->f;
}

void free_v(void *dst, void *opts) {
  
  (void) opts;

  free((Vektor *) dst);
}

void recombinate_v(Individual *src1, 
                   Individual *src2, 
                   Individual *dst, 
                   void *opts) {
  
  (void) opts;

  Vektor *vs1 = (Vektor *) src1->iv;
  Vektor *vs2 = (Vektor *) src2->iv;
  Vektor *vd  = (Vektor *) dst->iv;

  vd->a = vs1->a;
  vd->b = vs1->b;
  vd->c = vs1->c;
  vd->d = vs2->d;
  vd->e = vs2->e;
  vd->f = vs2->f;
}

void mutate_v(Individual *src, void *opts) {
  
  (void) opts;

  Vektor *v = (Vektor *) src->iv;

  v->a += (rand() % 3) - 1;
  v->b += (rand() % 3) - 1;
  v->c += (rand() % 3) - 1;
  v->d += (rand() % 3) - 1;
  v->e += (rand() % 3) - 1;
  v->f += (rand() % 3) - 1;
}

int64_t fittnes_v(Individual *src, void *opts) {
  
  (void) opts;

  Vektor *v = (Vektor *) src->iv;

  return abs(v->a - v->b) + abs(v->b - v->c)
            + abs(v->c - v->d) + abs(v->d - v->e)
              + abs(v->e - v->f) + abs(v->f - v->a);
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("%s <num generations>\n", argv[0]);
    exit(1);
  }

  Individual *best;

  EvInitArgs args;

  args.init_iv              = init_v;
  args.clone_iv             = clone_v;
  args.free_iv              = free_v;
  args.mutate               = mutate_v;
  args.fitness              = fittnes_v;
  args.recombinate          = recombinate_v;
  args.clone_iv             = NULL;
  args.population_size      = TEST_NUM_IVS;
  args.generation_limit     = atoi(argv[1]);
  args.mutation_propability = 1.0;
  args.death_percentage     = 0.5;
  args.opts                 = NULL;
  args.num_threads          = 1;
  args.flags                = EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP;

  Evolution *ev = new_evolution(&args);
  best = evolute(ev);

  Vektor *v = (Vektor *) best->iv;

  printf("Best Vektor: (%3d, %3d, %3d, %3d, %3d, %3d) %10li\n\n", v->a, v->b, v->c, v->d, v->e, v->f, 
            best->fitness);

  int i;
  for (i = 0; i < TEST_NUM_IVS; i++) {
    best = ev->population[i];
    v = (Vektor *) best->iv;

    printf("(%3d, %3d, %3d, %3d, %3d, %3d) %10li\n", v->a, v->b, v->c, v->d, v->e, v->f, 
              best->fitness);
  }

  return 0;
}
