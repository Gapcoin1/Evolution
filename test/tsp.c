/**
 * implementation of the Traveling Salesman Problem
 * to test my Evolution-Algorithm
 */
#ifndef __TSP__
#define __TSP__

#include "tsp.h"

/* functions */
TSP *new_tsp(uint32_t length);
void init_tsp_ev(TSPEvolution *tsp_ev, TSP *tsp);
void *init_tsp_route(void *opts);
void clone_tsp_route(void *v_dst, void *v_src, void *opts);
void free_tsp_route(void *v_src, void *opts);
void mutate_tsp_route(Individual *iv, void *opts);
void mutate_tsp_route_reinit(Individual *iv, void *opts);
void mutate_tsp_route_switch(Individual *iv, void *opts);
int64_t tsp_route_length(Individual *iv, void *opts);
void recombinate_tsp_route(Individual *src_1,
                            Individual *src_2,
                            Individual *dst,
                            void *opts);
char tsp_continue_ev(Evolution *const ev);

/**
 * main method to start the tsp test
 */
int main(int argc, char *argv[]) {
  
  /* cmd args check */
  if (argc != 6) {
    printf("%s <num citys> <generation limit> <num generations> "
            "<num threads> <verbose(0-3)>\n", argv[0]);
    exit(1);
  }

  int verbose = EV_VEB0;

  switch (atoi(argv[3])) {
    case 1:   verbose = EV_VEB1; break;
    case 2:   verbose = EV_VEB2; break;
    case 3:   verbose = EV_VEB3; break;
    default : verbose = EV_VEB0; 
  }

  int n_threads       = atoi(argv[4]);
  int n_generations   = atoi(argv[3]);
  int n_citys         = atoi(argv[1]);
  TSP *tsp            = new_tsp(n_citys);
  TSPEvolution **opts = malloc(sizeof(TSPEvolution *) * n_threads);


  int i;
  for (i = 0; i < n_threads; i++) {
    opts[i] = malloc(sizeof(TSPEvolution));
    init_tsp_ev(opts[i], tsp);
  }

  EvInitArgs args;

  args.init_iv              = init_tsp_route;
  args.clone_iv             = clone_tsp_route;
  args.free_iv              = free_tsp_route;
  args.mutate               = mutate_tsp_route;
  args.fitness              = tsp_route_length;
  args.recombinate          = recombinate_tsp_route;
  args.continue_ev          = tsp_continue_ev;
  args.population_size      = n_generations;
  args.generation_limit     = atoi(argv[2]);
  args.mutation_propability = 0.1;
  args.death_percentage     = 0.5;
  args.opts                 = (void **) opts;
  args.num_threads          = n_threads;
  args.flags                = EV_UMUT|EV_AMUT|EV_ABRT|EV_KEEP|verbose;

  Individual *best;
  Evolution *ev = new_evolution(&args);
  best = evolute(ev);
  TSPRoute *route = best->iv;

  if (n_citys <= 40) {

    uint32_t x, y;
    for (x = 0; x < tsp->length; x++) {
      for (y = 0; y < tsp->length; y++) {
        printf("%3u", tsp->distances[x][y]);  
      }
      printf("\n");
    }
  }

  #ifndef NO_OUTPUT
  printf("shortest found path: %" PRIi64 "\n", best->fitness);

  /**
   * calculate the improvve in comparison with an random route
   */
  uint32_t rand_fitness = 0;
  for (i = 0; i < n_generations; i++) {
    Individual iv;
    iv.iv = init_tsp_route(opts[0]);
    rand_fitness += tsp_route_length(&iv, opts[0]);
    free_tsp_route(iv.iv, opts[0]);
  }
  rand_fitness /= n_generations;

  printf("improov in comparison to an average "
         "random route (%" PRIu32 "): %f\n",
         rand_fitness,
         (double) best->fitness / (double) rand_fitness);

  if (n_citys <= 40) {

    uint32_t x, y;
    for (x = 0; x < tsp->length; x++) {
      for (y = 0; y < tsp->length; y++) {
        printf("%3u", tsp->distances[x][y]);  
      }
      printf("\n");
    }

    uint32_t j;
    for (j = 0; j < route->length; j++)
      printf("[%3" PRIu32 "][%3" PRIu32 "](%3" PRIu32 ")\n",
             route->roads[j].city_a,
             route->roads[j].city_b,
             route->roads[j].distance);
  }
  #else
  (void) route;
  #endif

  return 0;

}

/**
 * creats an random TSP with a given length
 */
TSP *new_tsp(uint32_t length) {
  
  TSP *tsp = malloc(sizeof(TSP));
  srand(time(NULL));

  tsp->length    = length;
  tsp->distances = malloc(sizeof(uint32_t *) * length);
  tsp->start     = rand() % length;

  uint32_t x, y;
  for (x = 0; x < length; x++) {
    tsp->distances[x]    = malloc(sizeof(uint32_t) * length);
    tsp->distances[x][x] = 0;
  }

  /* fill tsp with random distances */
  for (x = 0; x < length; x++) {
    for (y = x + 1; y < length; y++) {
      tsp->distances[x][y] = tsp->distances[y][x] = 1 + (rand() % length);  
    }
  }

  /**
   * calculatze a lower barrier
   * by adding all local optimal distances
   * together, a optimal solutan cann be better
   * as all local solution, but an optimal solution
   * dosn't neet to include all local optimal distances
   * so this is possible a barrier never reatched
   */
  tsp->lower_barrier = 0;
  for (x = 0; x < length; x++) {

    uint32_t min = RAND_MAX;
    for (y = 0; y < length; y++) {
      if (x == y) continue;

      if (min > tsp->distances[x][y])
        min = tsp->distances[x][y];
    }
    tsp->lower_barrier += min;
  }

  return tsp;
}

/**
 * inits an given TSPEvolution with a given TSP
 */
void init_tsp_ev(TSPEvolution *tsp_ev, TSP *tsp) {

  /* soft copy (only length) */
  tsp_ev->tsp = *tsp;
  tsp_ev->mut_size_reduce = 0.0;

  ARY_INIT(uint32_t, tsp_ev->citys, tsp->length);
  ARY_INIT(uint32_t, tsp_ev->tmp, tsp->length);

  uint32_t i;
  for (i = 0; i < tsp->length; i++)
    ARY_ADD(tsp_ev->citys, i);
  
}

/**
 * functions for sorting the TSPRoads pointer array
 * by city_a index
 */
static inline char roads_bigger(TSPRoad *a, TSPRoad *b) {
  return a->city_a > b->city_a;
}
static inline char roads_smaler(TSPRoad *a, TSPRoad *b) {
  return a->city_a < b->city_a;
}
static inline char roads_equal(TSPRoad *a, TSPRoad *b) {
  return a->city_a == b->city_a;
}

/**
 * int an TSPRoute for a given TSPEvolution
 * Note: in the TSP Evolution an individual is represented by an Road
 *
 * complexity is in O(n) 
 * n = route->length
 */
void *init_tsp_route(void *opts) {
  
  TSPEvolution *tsp_ev = opts;

  /**
   * init new Route
   * length is num citys + 1 because we must travel
   * back to start city
   */
  TSPRoute *route = malloc(sizeof(TSPRoute));
  route->length   = tsp_ev->tsp.length; 
  route->roads    = malloc(sizeof(TSPRoad) * route->length);
  route->citys    = malloc(sizeof(TSPRoad *) * route->length);
 
  /**
   * clone citys to choose a route randomly by extracting
   * citys from temp array
   */
  ARY_CLONE(tsp_ev->tmp, tsp_ev->citys);

  uint32_t city_a, city_b, i;
  city_a = tsp_ev->tsp.start;
  ARY_DELETE(tsp_ev->tmp, tsp_ev->tsp.start);
  
  /**
   * calculate random route for the TSP
   */
  for (i = 0; 
       (i < route->length - 1) && ARY_NOT_EMPTY(tsp_ev->tmp); 
       i++) {

    ARY_EXTRACT(tsp_ev->tmp, city_b);
    
    route->roads[i].city_a   = city_a;
    route->roads[i].city_b   = city_b;
    route->roads[i].distance = tsp_ev->tsp.distances[city_a][city_b];

    /* next start will be current end */
    city_a = city_b;
  }

  /**
   * from the last city we must travel back to start
   */
  city_b = tsp_ev->tsp.start;

  route->roads[i].city_a   = city_a;
  route->roads[i].city_b   = city_b;
  route->roads[i].distance = tsp_ev->tsp.distances[city_a][city_b];

  /**
   * init road pointer array so that it is sorted by city_a index
   */
  for (i = 0; i < route->length; i++)
    route->citys[route->roads[i].city_a] = &route->roads[i];

  return route;
}

/**
 * clones an given TSPRoute
 *
 * complexity is in O(n) 
 * n = route->length
 */
void clone_tsp_route(void *v_dst, void *v_src, void *opts) { 
  
  (void) opts;
  TSPRoute *dst = v_dst;
  TSPRoute *src = v_src;

  memcpy(dst->roads, src->roads, sizeof(TSPRoad) * src->length);

  /**
   * change road pointer array so that it is sorted by city_a index
   */
  uint32_t i;
  for (i = 0; i < dst->length; i++)
    dst->citys[dst->roads[i].city_a] = &dst->roads[i];

}

/**
 * frees an given TSPRoute
 *
 * complexity is in O(1) 
 */
void free_tsp_route(void *v_src, void *opts) {
  
  (void) opts;
  TSPRoute *src = v_src;

  free(src->roads);
  free(src->citys);
  free(src);
}

/**  sets the distance of the road 
 * at the given index from the given TSP
 */
#define TSP_SET_DISTANCE(ROUTE, I, TSP)                                     \
  (ROUTE).roads[I].distance = (TSP).distances[(ROUTE).roads[I].city_a]      \
                                             [(ROUTE).roads[I].city_b]

void mutate_tsp_route(Individual *iv, void *opts) {
  
  if (((TSPEvolution *) opts)->mut_size_reduce <= 4.5)
    mutate_tsp_route_reinit(iv, opts);
  else
    mutate_tsp_route_switch(iv, opts);

}

/**
 * mutate an given TSPRoute
 * by switching two random citys
 *
 * complexity is in O(1) 
 * n = route->length
 */
void mutate_tsp_route_switch(Individual *iv, void *opts) {

  TSPEvolution *tsp_ev = opts;
  TSPRoute     *route = iv->iv;
 
  uint32_t start = rand() % (route->length - 1);
  uint32_t end   = 1 + (rand() % (route->length - 1));
  uint32_t tmp;

  /* switch citys */
  tmp                          = route->roads[end - 1].city_b;
  route->roads[end - 1].city_b = route->roads[start].city_b;
  route->roads[start].city_b   = tmp;

  tmp                            = route->roads[end].city_a;
  route->roads[end].city_a       = route->roads[start + 1].city_a;
  route->roads[start + 1].city_a = tmp;

  /* reset distances */
  TSP_SET_DISTANCE(*route, start, tsp_ev->tsp);
  TSP_SET_DISTANCE(*route, start + 1, tsp_ev->tsp);
  TSP_SET_DISTANCE(*route, end, tsp_ev->tsp);
  TSP_SET_DISTANCE(*route, end - 1, tsp_ev->tsp);
  
  //TODO remove roads pointer array and recombinate not used!!
}

/**
 * mutate an given TSPRoute 
 * by reinitalize an random length part
 *
 *                               TODO add a mutate function which switches two citys in the route (smal mutate)
 *                               and optional searches the best two citys to switch
 *                               and if the can be no improoves because of switching uses normal mutate with smal path
 *                               first normal mutate paths gest smaller till it has length 3 thne switch mutate is useds
 *                               and then switch mutate with search is used
 *
 * complexity is in O(n) 
 * n = route->length
 */
void mutate_tsp_route_reinit(Individual *iv, void *opts) {

  TSPEvolution *tsp_ev = opts;
  TSPRoute     *route = iv->iv;

  ARY_CLEAR(tsp_ev->tmp);

  /**
   * choose random lengt part >= 3 to recalculate   
   * wenn need min three roads because start(city_a) of road 0
   * and end(city_b) of last road are fixed
   */
  int32_t length = (rand() % (1 + route->length - 3)); 
  length -= (int32_t) (((double) (rand() % (1 + route->length - 3))) *
                        tsp_ev->mut_size_reduce);
  if (length <= 3)
    length = 3;

  /* chose random start position */
  uint32_t start = rand() % (1 + (route->length - length));
  uint32_t end   = start + length - 1;

  /**
   * adding all citys of the part of route to tmp array
   * start and end citys are fix
   */
  uint32_t i;
  for (i = start; i < end; i++)
    ARY_ADD(tsp_ev->tmp, route->roads[i].city_b);


  uint32_t city_a, city_b;
  city_a = route->roads[start].city_a;
  
  /**
   * calculate random route for the TSP
   */
  for (i = start; 
       (i < end) && ARY_NOT_EMPTY(tsp_ev->tmp); 
       i++) {

    ARY_EXTRACT(tsp_ev->tmp, city_b);
    
    route->roads[i].city_a   = city_a;
    route->roads[i].city_b   = city_b;
    route->roads[i].distance = tsp_ev->tsp.distances[city_a][city_b];

    /* next start will be current end */
    city_a = city_b;

    /**
     * change the pointer of the new city_a
     * first city_a is fix so no need to change the pointer
     */
    route->citys[city_a] = &route->roads[i + 1];
  }

  /**
   * for the last road city_b is fix
   */
  city_b = route->roads[end].city_b;
  route->roads[end].city_a   = city_a;
  route->roads[end].distance = tsp_ev->tsp.distances[city_a][city_b];

}

/**
 * In the TSP Evolution the fitness is the min length of an TSPRoute
 *
 * complexity is in O(n) 
 * n = route->length
 */
int64_t tsp_route_length(Individual *iv, void *opts) {
  
  (void) opts;
  TSPRoute *route = iv->iv;

  int64_t length = 0;
  uint32_t i;
  for (i = 0; i < route->length; i++)
    length += route->roads[i].distance;

  return length;
}

/**
 * recobinates two TSPRoads to an new improoved TSPRoad
 *
 * complexity is in O(n) 
 * n = route->length
 */
void recombinate_tsp_route(Individual *src_1,
                            Individual *src_2,
                            Individual *dst,
                            void *opts) {
  
  TSPEvolution *tsp_ev = opts;
  TSPRoute *route_a    = src_1->iv;
  TSPRoute *route_b    = src_2->iv;
  TSPRoute *new_route  = dst->iv;

  uint32_t city_a, city_b, distance;
  city_a = route_a->roads[0].city_a; /* start city */

  /* clear tmp */
  ARY_SET_NULL(tsp_ev->tmp);

  /**
   * create a new TSPRoute by selecting the next city_b with
   * the shortes distance from one of the two source routes
   */
  uint32_t i;
  for (i = 0; i < new_route->length; i++) {

    /**
     * every used city of the new route
     * will be setted to 1
     */
    ARY_AT(tsp_ev->tmp, city_a) = 1;
    
    /**
     * if city_b of route_a is already used 
     * we must take city_b form route_b 
     */
    if (ARY_AT(tsp_ev->tmp, route_a->citys[city_a]->city_b)) {

      city_b   = route_b->citys[city_a]->city_b;
      distance = route_b->citys[city_a]->distance;
    } else {

      /**
       * if also city_b of route_b is not used 
       * we can choose the better city 
       * else whe have to take the city_b of route_a
       */
      if (ARY_AT(tsp_ev->tmp, route_b->citys[city_a]->city_b) == 0 &&
          route_a->citys[city_a]->distance <
          route_b->citys[city_a]->distance) {
        
        city_b   = route_a->citys[city_a]->city_b;
        distance = route_a->citys[city_a]->distance;
      } else {
        city_b   = route_b->citys[city_a]->city_b;
        distance = route_b->citys[city_a]->distance;
      }
    }

    /* ad the better road to the new route */
    new_route->roads[i].city_a   = city_a;
    new_route->roads[i].city_b   = city_b;
    new_route->roads[i].distance = distance;
    new_route->citys[city_a]     = &new_route->roads[i];

    /* old city_b is new city_a */
    city_a = city_b;
  }
}

/**
 * continue_ev function which controls the art of 
 * mutation an gives extra output
 */
char tsp_continue_ev(Evolution *const ev) {
  
  TSP *tsp = &((TSPEvolution *) *ev->opts)->tsp;

  /* if optimal route is found stop calculation */
  if (ev->population[0]->fitness == tsp->lower_barrier)
    return 0;


  /* if no improoves reduce mutation size */
  if (ev->info.improovs == 0) {
    
    int i;
    TSPEvolution *tmp;
    for (i = 0; i < ev->num_threads; i++) {
      tmp = ev->opts[i];

      if (tmp->mut_size_reduce < 4.5)
        tmp->mut_size_reduce += 0.01;
    }


  }


  #ifndef NO_OUTPUT
  /* print status informations */
  printf("\33[2K\rimproovs %3d -> %8.5f%%   best fitness %10li  "
         "lower barrier: %" PRIu32 "  mut-%%: %f \n",              
         ev->info.improovs,
         ((ev->info.improovs / (double) ev->deaths) * 100.0),
         ev->population[0]->fitness,
         tsp->lower_barrier,
         ((TSPEvolution *) *ev->opts)->mut_size_reduce);
  #endif

  return 1;
}


#endif /* __TSP__ */
