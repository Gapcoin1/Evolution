/**
 * implementation of the Traveling Salesman Problem
 * to test my Evolution-Algorithm
 */
#ifndef __TSP_H__
#define __TSP_H__
#include "../src/C-Utils/Dynamic-Array/src/dynamic-array.h"
#include "../src/C-Utils/Sort/src/sort.h"
#include "../src/evolution.h"

/**
 * uint32_t array
 */
DEF_ARY(uint32_t, UI32Ary);

/**
 * one road  between two citys of an TSP Route
 * Note: in the TSP Evolution an individual is represented by an Road
 */
typedef struct {
  uint32_t city_a;    /* id of city a */
  uint32_t city_b;    /* id of city b */
  uint32_t distance;  /* distance between city a and b */
} TSPRoad;

/**
 * one route of an TSP
 */
typedef struct {
  uint32_t length;  /* number of roads in this route */
  TSPRoad  *roads;  /* the roads of this route */
  TSPRoad  **citys; /* pointer to the roads sorted by city_a index  */
                    /* for better accessing an specific city        */
} TSPRoute;

/**
 * The TSP Matrix index [a][b] is the distance between
 * city a and city b
 */
typedef struct {
  uint32_t length;        /* the number of citys in this TSP */
  uint32_t **distances;   /* the distance matrix */
  uint32_t start;         /* the start city */
  uint32_t lower_barrier; /* the lower barrier of this TSP */
} TSP;

/**
 * struct holding neccessary information
 * for evolute an TSP
 */
typedef struct {
  uint16_t index;           /* index of the thread */
  TSP tsp;                  /* the tsp */
  UI32Ary citys;            /* indexes of all citys */
  UI32Ary tmp;              /* temp array for calculation */
  double  mut_size_reduce;  /* pecentage to controll the mutation size */
} TSPEvolution;

#endif /* __TSP_H__ */
