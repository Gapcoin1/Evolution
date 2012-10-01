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

#include "../src/Evolution.c"

typedef struct {
  int a, b, c, d, e, f;
} Vektor;

void *init_v() {
  Vektor *v = (Vektor *) malloc(sizeof(Vektor));
  v->a = (rand() % 100) - 50;
  v->b = (rand() % 100) - 50;
  v->c = (rand() % 100) - 50;
  v->d = (rand() % 100) - 50;
  v->e = (rand() % 100) - 50;
  v->f = (rand() % 100) - 50;
  return v;
}

void clone_v(void *dst, void *src) {
  Vektor *vd = (Vektor *) dst;
  Vektor *vs = (Vektor *) src;

  dv->a = vs->a;
  dv->b = vs->b;
  dv->c = vs->c;
  dv->d = vs->d;
  dv->e = vs->e;
  dv->f = vs->f;
}

void free_v(void *dst) {
  free((Vektor *) dst);
}

void recombinate_v(Individual *src1, Individual *src2, Individual *dst) {
  Vektor *vs1 = (Vektor *) src1->individual;
  Vektor *vs2 = (Vektor *) src2->individual;
  Vektor *vd  = (Vektor *) dst->individual;

  dv->a = vs1->a;
  dv->b = vs1->b;
  dv->c = vs1->c;
  dv->d = vs2->d;
  dv->e = vs2->e;
  dv->f = vs2->f;
}

/**
 * We use always mutate and our own random
 */
void mutate_v(Individual *src) {
  Vektor *v = (Vektor *) src->individual;

  if (rand() % 100 == 0)
    v->a += (rand() % 3) - 1;
  if (rand() % 100 == 0)
    v->b += (rand() % 3) - 1;
  if (rand() % 100 == 0)
    v->c += (rand() % 3) - 1;
  if (rand() % 100 == 0)
    v->d += (rand() % 3) - 1;
  if (rand() % 100 == 0)
    v->e += (rand() % 3) - 1;
  if (rand() % 100 == 0)
    v->f += (rand() % 3) - 1;
}

int fittnes_v(Individual *src) {
  Vektor *v = (Vektor *) src->individual;

  int i = abs(v->a - v->b) + abs(v->b - v->c)
            + abs(v->c - v->d) + abs(v->d - v->e)
              + abs(v->e - v->f) + abs(v->f - v->a);

  if (i == 0) return MAX_U_INT32;

  return (int) ((1.0 / (double) i) * MAX_U_INT32);
}

int main() {
  Individual best;

  best = best_evolution(init_v, clone_v, free_v, mutate_v, fittnes_v,
                          recombinate_v, NULL, TEST_NUM_IVS, 70, 1.0, 0.5,
                             EV_UREC|EV_UMUT|EV_AMUT);

  Vektor *v = (Vektor *) best.individual;

  printf("Nest Vektor: (%d, %d, %d, %d, %d, %d)\n", v->a, v->b, v->c, v->d, v->e, v->f);
  return 0;
}

