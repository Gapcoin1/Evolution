/**
 * Simple implemenation of an Iterativ quick sort algorithm witch
 * fall back on a given min size to insertion sort
 */
#include <math.h>
#include <pthread.h>
#include "Array.h"

/**
 * The Element Type of the Array to sort
 */
#ifndef CHIEFSORT_TYPE
#define CHIEFSORT_TYPE int
#endif

#ifndef CHIEFSORT_BIGGER
#define CHIEFSORT_BIGGER(X, Y) X > Y
#endif

#ifndef CHIEFSORT_SMALER
#define CHIEFSORT_SMALER(X, Y) X < Y
#endif

#ifndef CHIEFSORT_EQL
#define CHIEFSORT_EQL(X, Y) X == Y
#endif

#ifndef U_INT_ARY
#define U_INT_ARY
DEFINE_ARRAY(int, IntAry)
#endif

/**
 * Struct for handling paralell sorting
 */
typedef struct {
  CHIEFSORT_TYPE *ary;
  int length;
  int threads;
  int min_insertionsort;
} ChiefSortParalell;

// functions
void *serialchiefsort(void *arg);
void *paralellchiefsort(void *arg);
void chiefsort(CHIEFSORT_TYPE *ary, int length, int min);
void insertionsort(CHIEFSORT_TYPE *ary, int length);
void quicksort(CHIEFSORT_TYPE *ary, int length);
void paralellsort(CHIEFSORT_TYPE *ary, int length, int min_insertionsort, int threads);
