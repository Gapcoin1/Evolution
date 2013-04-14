#ifndef EVOLUTION
#define EVOLUTION
#include "Evolution.h"

/**
 * Functions for Sorting the Population by Fitness
 * Macro versions
 */
static inline char macro_bigger(Individual *a, Individual *b) {
  return a->fitness > b->fitness;
}

static inline char macro_smaler(Individual *a, Individual *b) {
  return a->fitness < b->fitness;
}

static inline char macro_equal(Individual *a, Individual *b) {
  return a->fitness == b->fitness;
}

/* void pointer version */
char macro_bigger(void *a, void *b) {
  return ((Individual *) a)->fitness > ((Individual *) b)->fitness;
}

char macro_smaler(void *a, void *b) {
  return ((Individual *) a)->fitness < ((Individual *) b)->fitness;
}

char macro_equal(void *a, void *b) {
  return ((Individual *) a)->fitness == ((Individual *) b)->fitness;
}

#define CHIEFSORT_TYPE Individual *
#define CHIEFSORT_BIGGER(X, Y) X->fitness > Y->fitness
#define CHIEFSORT_SMALER(X, Y) X->fitness < Y->fitness
#define CHIEFSORT_EQL(X, Y) X->fitness == Y->fitness
#define EV_MIN_QUICKSORT 20
#include "sort.h"


/**
 * Returns pointer to an new and initialzed Evolution
 * take pointers for varius functions:
 *
 * opts can be used for aditional function wide values
 *
 * +------------------------------------+-------------------------------------+
 * | function pointer                   | describtion                         |
 * +------------------------------------+-------------------------------------+
 * | void *init_individual(void *opts)  | should return an void pointer to an |
 * |                                    | new initialized individual          |
 * |                                    |                                     |
 * | void clone_individual(void *dst,   | takes 2 void pointer to individuals |
 * |                       void *src,   | and should clone the content of the |
 * |                       void *opts)  | second one into the first one       |
 * |                                    |                                     |
 * | void free_individual(void *src,    | takes an void pointer to individual |
 * |                      void *opts)   | and should free the spaces          |
 * |                                    | allocated by the given individual   |
 * |                                    |                                     |
 * | void mutate(Individual *src,       | takes an void pointer to an         |
 * |             void *opts)            | individual and should change it in  |
 * |                                    | a way that the probability to       |
 * |                                    | improove it is around 1/5           |
 * |                                    |                                     |
 * | int fitness(Individual *src,       | takes an void pointer to an         |
 * |             void *opts)            | individual, and should return an    |
 * |                                    | integer value that indicates how    |
 * |                                    | strong (good / improoved / near by  |
 * |                                    | an optimal solution) it is. Control |
 * |                                    | the sorting order with the flags    |
 * |                                    |                                     |
 * | void recombinate(Individual *src1, | takes 3 void pointer to individuals |
 * |                  Individual *src2, | and should combinate the first two  |
 * |                  Individual *dst,  | one, and should save the result in  |
 * |                  void *opts)       | the thired one. As mutate the       |
 * |                                    | probability to get an improoved     |
 * |                                    | individuals should be around 1/5    |
 * |                                    |                                     |
 * | char continue_ev(Individual *ivs,  | takes an pointer to the current     |
 * |                  void *opts)       | Individuals and should return 0 if  |
 * |                                    | the calculaten should stop and 1 if |
 * |                                    | the calculaten should continue      |
 * +------------------------------------+-------------------------------------+
 *
 * Note: the void pointer to individuals are not pointer to an Individual 
 *       struct, they are the individual element of the Individual struct
 *
 * flags can be:
 *
 *    EV_UREC / EV_USE_RECOMBINATION
 *    EV_UMUT / EV_USE_MUTATION
 *    EV_AMUT / EV_ALWAYS_MUTATE
 *    EV_KEEP / EV_KEEP_LAST_GENERATION
 *    EV_ABRT / EV_USE_ABORT_REQUIREMENT
 *    EV_SMAX / EV_SORT_MAX
 *    EV_SMIN / EV_SORT_MIN
 *    EV_VEB0 / EV_VERBOSE_QUIET
 *    EV_VER1 / EV_VERBOSE_ONELINE
 *    EV_VER2 / EV_VERBOSE_HIGH
 *    EV_VER3 / EV_VERBOSE_ULTRA
 *
 * The floowing flag combinations are possible:
 *
 * +---------------------------------+----------------------------------------+
 * | Flag combination                | descrion                               |
 * +---------------------------------+----------------------------------------+
 * | EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP | Recombinate and always mutate          |
 * | |EV_ABRT                        | individuals, keep last generation,     |
 * |                                 | and use abort requirement function     |
 * |                                 |                                        |
 * | EV_UMUT|EV_AMUT|EV_KEEP|EV_ABRT | Onley mutate individual keep last      |
 * |                                 | generation and use abort requirement   |
 * |                                 | function                               |
 * |                                 |                                        |
 * | EV_UREC|EV_UMUT|EV_KEEP|EV_ABRT | Recombinate and maybe mutate           |
 * |                                 | individuals (depending on a given      |
 * |                                 | probability) keep last generation and  |
 * |                                 | use abort requirement function         |
 * |                                 |                                        |
 * | EV_UREC|EV_KEEP|EV_ABRT         | Recombinate individuals, keep last     |
 * |                                 | generation and use abort requirement   |
 * |                                 | function                               |
 * |                                 |                                        |
 * | EV_UREC|EV_UMUT|EV_AMUT|EV_ABRT | Recombinate and always mutate          |
 * |                                 | individuals, disgard last generation.  |
 * |                                 | and use abort requirement function     |
 * |                                 |                                        |
 * | EV_UMUT|EV_AMUT|EV_ABRT         | Onely mutate individuals, disgard last |
 * |                                 | generation, and use abort requirement  |
 * |                                 | function                               |
 * |                                 |                                        |
 * | EV_UREC|EV_UMUT|EV_ABRT         | Recombinate and maybe mutate           |
 * |                                 | individuals (depending on a given      |
 * |                                 | probability), disgard last generation  |
 * |                                 | and use abort requirement function     |
 * |                                 |                                        |
 * | EV_UREC|EV_ABRT                 | Recombinate individuals, disgard last  |
 * |                                 | generation and use abort requirement   |
 * |                                 | function                               |
 * |                                 |                                        |
 * | EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP | Recombinate and always mutate          |
 * |                                 | individuals, keep last generation, and |
 * |                                 | calc until generation limit is         |
 * |                                 | reatched                               |
 * |                                 |                                        |
 * | EV_UMUT|EV_AMUT|EV_KEEP         | Onely mutate individuals, keep last    |
 * |                                 | generation and calc until generation   |
 * |                                 | limit is reatched                      |
 * |                                 |                                        |
 * | EV_UREC|EV_UMUT|EV_KEEP         | Recombinate and maybe mutate           |
 * |                                 | individuals, keep last generation and  |
 * |                                 | calc until generation limit is         |
 * |                                 | reatched                               |
 * |                                 |                                        |
 * | EV_UREC|EV_KEEP                 | Recombinate individuals, keep last     |
 * |                                 | generation and calc until generation   |
 * |                                 | limit is reatched                      |
 * |                                 |                                        |
 * | EV_UREC|EV_UMUT|EV_AMUT         | Recombinate and always mutate          |
 * |                                 | individuals, disgard last generation   |
 * |                                 | and calc until generation imit is      |
 * |                                 | reatched                               |
 * |                                 |                                        |
 * | EV_UMUT|EV_AMUT                 | Recombinate and always mutate          |
 * |                                 | Individuals disgard old generation and |
 * |                                 | calc until generation limit is         |
 * |                                 | reatched                               |
 * |                                 |                                        |
 * | EV_UREC|EV_UMUT                 | Recombinate and maybe mutate           |
 * |                                 | Individual (depending on a given       |
 * |                                 | probability) disgard old generation    |
 * |                                 | and calc until generation limit is     |
 * |                                 | reatched                               |
 * |                                 |                                        |
 * | EV_UREC                         | Recombinate Individual onley disgard   |
 * |                                 | old generation and calc until          |
 * |                                 | generation limit is reatched           |
 * +---------------------------------+----------------------------------------+
 *
 * to all of the above combination an EV_SMIN / EV_SMAX can be added
 * standart is EV_SMIN
 * Also an verbosytiy level of:
 *    EV_VERBOSE_QUIET    (EV_VEB0), 
 *    EV_VERBOSE_ONELINE  (EV_VEB1)
 *    EV_VERBOSE_HIGH     (EV_VEB2) 
 *    EV_VERBOSE_ULTRA    (EV_VEB3)
 * can be added, standart is EV_VERBOSE_QUIET
 */
Evolution *new_evolution(void *(*init_individual) (void *), 
                         void (*clone_individual) (void *, void *, void *),
                         void (*free_individual) (void *, void *), 
                         void (*mutate) (Individual *, void *),
                         int (*fitness) (Individual *, void *), 
                         void (*recombinate) (Individual *,
                                              Individual *, 
                                              Individual *, 
                                              void *), 
                         char (*continue_ev) (Individual *, void *),
                         int population_size, 
                         int generation_limit, 
                         double mutation_propability,
                         double death_percentage, 
                         void *opts, 
                         short flags) {

  return new_evolution_parallel(init_individual, 
                                clone_individual, 
                                free_individual, 
                                mutate, fitness, 
                                recombinate, 
                                continue_ev,
                                population_size, 
                                generation_limit, 
                                mutation_propability, 
                                death_percentage, 
                                &opts, 
                                1, 
                                flags);
}

/**
 * Parallel version takes multiple opts pointer each one for each thread
 */
Evolution *new_evolution_parallel(void *(*init_individual) (void *), 
                                  void (*clone_individual) (void *, void *, void *),
                                  void (*free_individual) (void *, void *), 
                                  void (*mutate) (Individual *, void *),
                                    int (*fitness) (Individual *, void *), void (*recombinate) (Individual *,
                                     Individual *, Individual *, void *), char (*continue_ev) (Individual *, void *),
                                      int population_size, int generation_limit, double mutation_propability,
                                       double death_percentage, void **opts, int num_threads, short flags) {

  int tflags = flags & ~EV_SMAX;
  tflags &= ~EV_VEB1;
  tflags &= ~EV_VEB2;
  // valid flag combination ?
  if (tflags != EV_UREC && tflags != (EV_UREC|EV_UMUT) && tflags != (EV_UREC|EV_UMUT|EV_AMUT)
       && tflags != (EV_UREC|EV_KEEP) && tflags != (EV_UREC|EV_UMUT|EV_KEEP)
        && tflags != (EV_UMUT|EV_AMUT|EV_KEEP) && tflags != (EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP)
         && tflags != (EV_UREC|EV_ABRT) && tflags != (EV_UREC|EV_UMUT|EV_ABRT)
          && tflags != (EV_UMUT|EV_AMUT|EV_ABRT) && tflags != (EV_UREC|EV_UMUT|EV_AMUT|EV_ABRT)
           && tflags != (EV_UREC|EV_KEEP|EV_ABRT) && tflags != (EV_UREC|EV_UMUT|EV_KEEP|EV_ABRT)
            && tflags != (EV_UMUT|EV_AMUT|EV_KEEP|EV_ABRT)
              && tflags != (EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP|EV_ABRT)) {
    
    #ifdef DEBUG
      printf("[DEBUG] wrong flag combination\n");
    #endif

    return NULL;
  }

  // valid other opts
  if (population_size <= 0 || generation_limit <= 0 || mutation_propability < 0.0 
        || mutation_propability > 1.0 || death_percentage < 0.0 || death_percentage > 1.0) {
   
    #ifdef DEBUG
      printf("[DEBUG] wrong opts\n");
    #endif

    return NULL;
  }
  
  // int random
  srand(time(NULL));

  Evolution *ev = (Evolution *) malloc(sizeof(Evolution));

  ev->init_individual        = init_individual;
  ev->clone_individual       = clone_individual;
  ev->free_individual        = free_individual;
  ev->mutate                 = mutate;
  ev->fitness                = fitness;
  ev->recombinate            = recombinate;
  ev->continue_ev            = continue_ev;
  ev->population_size        = population_size;
  ev->generation_limit       = generation_limit;
  ev->mutation_propability   = mutation_propability;
  ev->death_percentage       = death_percentage;
  ev->opts                   = opts;
  ev->parallel.num_threads   = num_threads;
  ev->parallel.i             = 0;
  ev->parallel.info.improovs = 0;
  ev->parallel.threads       = malloc(sizeof(pthread_t) * num_threads);
  ev->parallel.ev_threads    = malloc(sizeof(EvolutionThread) * num_threads);
  ev->parallel.mutate        = (int) ((double) EVOLUTION_MUTATE_ACCURACY * ev->mutation_propability);
  ev->use_recmbination       = flags & EV_USE_RECOMBINATION;
  ev->use_muttation          = flags & EV_USE_MUTATION;
  ev->always_mutate          = flags & EV_ALWAYS_MUTATE;
  ev->keep_last_generation   = flags & EV_KEEP_LAST_GENERATION;
  ev->use_abort_requirement  = flags & EV_USE_ABORT_REQUIREMENT;
  ev->sort_max               = flags & EV_SORT_MAX;
  ev->verbose                = flags & (EV_VERBOSE_ONELINE | EV_VERBOSE_HIGH);


  // multiplicator if we should discard the last generation, we can't recombinate in place
  int mul = ev->keep_last_generation ? 1 : 2;

  ev->population  = (Individual **) malloc(sizeof(Individual *) * population_size * mul);
  ev->individuals = (Individual *) malloc(sizeof(Individual) * population_size * mul);

  int i;
  if (num_threads > 1) {
    ev->parallel.i = population_size * mul;

    // creat threads
    for (i = 0; i < num_threads; i++) {
      ev->parallel.ev_threads[i].ev = ev;
      ev->parallel.ev_threads[i].index = i;
      pthread_create(&ev->parallel.threads[i], NULL, threadable_init_individual, (void *) &ev->parallel.ev_threads[i]);
    }

    // wait for threads
    for (i = 0; i < num_threads; i++) {
      pthread_join(ev->parallel.threads[i], NULL);
    }
  } else {
    for (i = 0; i < population_size * mul; i++) {
      ev->population[i] = ev->individuals + i;
      ev->individuals[i].individual = init_individual(ev->opts[0]);
      ev->population[i]->fitness = ev->fitness(ev->population[i], ev->opts[0]);
 
      if (ev->verbose >= EV_VERBOSE_ONELINE)
        printf("init population: %10d\r", population_size * mul - i);
    }
  }

  /**
   * Select the best individuals to survive,
   * Sort the Individuals by their fittnes
   */
  if (ev->sort_max)
    EVOLUTION_SELECTION_MAX(ev)
  else
    EVOLUTION_SELECTION_MIN(ev)

  if (ev->verbose >= EV_VERBOSE_HIGH)
    printf("Population Initialized\n");

  return ev;
}

/**
 * do the actual evolution, which means
 *  - calculate the fittnes for each Individual
 *  - sort Individual by fittnes
 *  - remove worst individuals
 *  - grow a new generation
 */
Individual *evolute(Evolution *ev) {

  int deaths   = (int) ((double) ev->population_size * ev->death_percentage);
  int survives = ev->population_size - deaths;
  int mutate   = (int) ((double) EVOLUTION_MUTATE_ACCURACY * ev->mutation_propability);
  EvolutionInfo info;
  info.improovs = 0;
  Individual *tmp_iv;
  char last_improovs_str[25];
  sprintf(last_improovs_str, "Not knowen");

  /**
   * Generation loop
   * each loop lets one generation grow kills the worst individuals
   * and let new individuals born
   */
  int i, j, rand1, rand2, start, end;
  for (i = 0; i < ev->generation_limit && (!ev->use_abort_requirement
                || (ev->use_abort_requirement && ev->continue_ev(ev->individuals, ev->opts[0]))); i++) {
    
    info.improovs = 0;
  
    /**
     * If we keep the last generation, we can recombinate in place
     * else wen wirst calculate an new populion
     */
    start = ev->keep_last_generation ? survives : ev->population_size;
    end   = ev->keep_last_generation ? ev->population_size : ev->population_size * 2; 


    if (ev->use_recmbination) {
      for (j = start; j < end; j++) {

        // from 2 Individuals of the untouched part we calculate an new one
        rand2 = rand1 = rand() % start;
        while (rand1 == rand2) rand2 = rand() % start; // TODO prevent endless loop if one one idividual survives
        
        // recombinate individuals
        ev->recombinate(ev->population[rand1], ev->population[rand2], ev->population[j], ev->opts[0]);
        
        // mutate Individuals
        if (ev->use_muttation) {
          if (ev->always_mutate)
            ev->mutate(ev->population[j], ev->opts[0]);
          else {
            rand1 = rand() % EVOLUTION_MUTATE_ACCURACY;
            if (rand1 <= mutate)
              ev->mutate(ev->population[j], ev->opts[0]);
          }
        }

        // calculate the fittnes for the new individual
        ev->population[j]->fitness = ev->fitness(ev->population[j], ev->opts[0]);

        /**
         * store if the new individual is better as the old one
         */
        if (ev->sort_max) {
          if (ev->population[j]->fitness > ev->population[rand1]->fitness
               && ev->population[j]->fitness > ev->population[rand2]->fitness) 
            info.improovs++;

        } else {
          if (ev->population[j]->fitness < ev->population[rand1]->fitness
               && ev->population[j]->fitness < ev->population[rand2]->fitness)
            info.improovs++;
        }


        if (ev->verbose >= EV_VERBOSE_ONELINE) {
          printf("Evolution: generation left %10d tasks recombination %10d improovs %9s%%\r", 
                                            ev->generation_limit - i, end - j, last_improovs_str);
        }
        if (ev->verbose >= EV_VERBOSE_ULTRA) {
          printf("Evolution: generation left %10d tasks recombination %10d improovs %9s%%\n", 
                                            ev->generation_limit - i, end - j, last_improovs_str);
        }
      }
    // copy and mutate individuals
    } else {
      
      // if deaths == survives, make sure that all survivers are being copyed and mutated
      if (start * 2 == end) {
        for (j = 0; j < start; j++) {
          ev->clone_individual(ev->population[j + start]->individual, ev->population[j]->individual, ev->opts[0]);
          ev->mutate(ev->population[j + start], ev->opts[0]);

          // calculate the fittnes for the new individual
          ev->population[j + start]->fitness = ev->fitness(ev->population[j + start], ev->opts[0]);
         
          /**
           * store if the new individual is better as the old one
           */
          if (ev->sort_max) {
            if (ev->population[j + start]->fitness > ev->population[j]->fitness) 
              info.improovs++;

          } else {
            if (ev->population[j + start]->fitness < ev->population[j]->fitness) 
              info.improovs++;
          }
         
         
          if (ev->verbose >= EV_VERBOSE_ONELINE) {
            printf("Evolution: generation left %10d tasks mutation-1/2 %10d improovs %9s%%\r", 
                                            ev->generation_limit - i, start - j, last_improovs_str);
          }
          if (ev->verbose >= EV_VERBOSE_ULTRA) {
            printf("Evolution: generation left %10d tasks mutation-1/2 %10d improovs %9s%%\n", 
                                            ev->generation_limit - i, start - j, last_improovs_str);
          }
        }

      // else choose random survivers to mutate
      } else {
        for (j = start; j < end; j ++) {
          rand1 = rand() % start;
          ev->clone_individual(ev->population[j]->individual, ev->population[rand1]->individual, ev->opts[0]);
          ev->mutate(ev->population[j], ev->opts[0]);

          // calculate the fittnes for the new individual
          ev->population[j]->fitness = ev->fitness(ev->population[j], ev->opts[0]);
         
          /**
           * store if the new individual is better as the old one
           */
          if (ev->sort_max) {
            if (ev->population[j]->fitness > ev->population[rand1]->fitness)
              info.improovs++;

          } else {
            if (ev->population[j]->fitness < ev->population[rand1]->fitness)
              info.improovs++;
          }
         
         
          if (ev->verbose >= EV_VERBOSE_ONELINE) {
            printf("Evolution: generation left %10d tasks mutation-1/x %10d improovs %9s%%\r", 
                                          ev->generation_limit - i, end - j, last_improovs_str);
          }
          if (ev->verbose >= EV_VERBOSE_ULTRA) {
            printf("Evolution: generation left %10d tasks mutation-1/x %10d improovs %9s%%\n", 
                                          ev->generation_limit - i, end - j, last_improovs_str);
          }
        }
      }
    }
  

    if (!ev->keep_last_generation) {
      
      /**
       * switch old and new generation to discard the old one
       * which will be overidden next time
       */
      for (j = 0; j < ev->population_size; j++) {
        tmp_iv = ev->population[j];
        ev->population[j] = ev->population[ev->population_size + j];
        ev->population[ev->population_size + j] = tmp_iv;
      }
    }

    /**
     * Select the best individuals to survive,
     * Sort the Individuals by theur fittnes
     */
    if (ev->sort_max)
      EVOLUTION_SELECTION_MAX(ev)
    else
      EVOLUTION_SELECTION_MIN(ev)

    if (ev->verbose >= EV_VERBOSE_ONELINE)
      sprintf(last_improovs_str, "%.5f", (info.improovs / (double) deaths) * 100.0);

    if (ev->verbose >= EV_VERBOSE_HIGH) {
      printf("improovs: %10d -> %9s%%      best fitness: %10li                                                         \n", 
                info.improovs, last_improovs_str, ev->population[0]->fitness);
    }
  }

  if (ev->verbose >= EV_VERBOSE_ONELINE)
    printf("                                                                                           \r");

  // return the best
  return ev->population[0];
}

/**
 * Frees unneded resauces after an evolution calculation
 */
void evolution_clean_up(Evolution *ev) {
  int end = ev->keep_last_generation ? ev->population_size : ev->population_size * 2; 
  int i;
  for (i = 1; i < end; i++) {
    ev->free_individual(ev->population[i]->individual, ev->opts[0]);
  }
  free(ev->individuals);
  free(ev->population);
}

/**
 * Computes an evolution for the given params
 * and returns the best Individual
 */
Individual best_evolution(void *(*init_individual) (void *), void (*clone_individual) (void *, void *, void *),
                          void (*free_individual) (void *, void *), void (*mutate) (Individual *, void *),
                            int (*fitness) (Individual *, void *), void (*recombinate) (Individual *,
                              Individual *, Individual *, void *), char (*continue_ev) (Individual *, void *),
                                int population_size, int generation_limit, double mutation_propability,
                                  double death_percentage, void *opts, short flags) {

  Evolution *ev = new_evolution(init_individual, clone_individual, free_individual, mutate,
                            fitness, recombinate, continue_ev, population_size, generation_limit, 
                              mutation_propability, death_percentage, opts, flags);

  Individual best = *evolute(ev);
  evolution_clean_up(ev);

  return best;
}

/**
 * Computes an evolution for the given params
 * and returns the best Individual parallel version
 */
Individual best_evolution_parallel(void *(*init_individual) (void *), void (*clone_individual) (void *, void *, void *),
                          void (*free_individual) (void *, void *), void (*mutate) (Individual *, void *),
                            int (*fitness) (Individual *, void *), void (*recombinate) (Individual *,
                              Individual *, Individual *, void *), char (*continue_ev) (Individual *, void *),
                                int population_size, int generation_limit, double mutation_propability,
                                  double death_percentage, void **opts, int num_threads, short flags) {

  Evolution *ev = new_evolution_parallel(init_individual, clone_individual, free_individual, mutate,
                            fitness, recombinate, continue_ev, population_size, generation_limit, 
                              mutation_propability, death_percentage, opts, num_threads, flags);

  Individual best = *evolute_parallel(ev);
  evolution_clean_up(ev);

  return best;
}

/**
 * Parallel init_individual function
 */
void *threadable_init_individual(void *arg) {
  EvolutionThread *evt = arg;
  Evolution *ev = evt->ev;
  int i;

  while (1) {
     
    pthread_mutex_lock(&ev_mutex);
      if (ev->parallel.i <= 0)
        break;
 
      i = ev->parallel.i;
      ev->parallel.i--;
    pthread_mutex_unlock(&ev_mutex);
 
    ev->population[i] = ev->individuals + i;
    ev->individuals[i].individual = ev->init_individual(ev->opts[evt->index]);
    ev->population[i]->fitness = ev->fitness(ev->population[i], ev->opts[evt->index]);
 
    pthread_mutex_lock(&ev_mutex);
      if (ev->verbose >= EV_VERBOSE_ONELINE)
        printf("init population: %10d\r", i);
    pthread_mutex_unlock(&ev_mutex);
 
  }

  // after break we must unlock
  pthread_mutex_unlock(&ev_mutex);

  return arg;
}

/**
 * do the actual evolution, which means
 *  - calculate the fittnes for each Individual
 *  - sort Individual by fittnes
 *  - remove worst individuals
 *  - grow a new generation
 *
 * Parallel version
 */
Individual *evolute_parallel(Evolution *ev) {

  int deaths   = (int) ((double) ev->population_size * ev->death_percentage);
  int survives = ev->population_size - deaths;
  Individual *tmp_iv;
  sprintf(ev->parallel.last_improovs_str, "Not knowen");

  /**
   * Generation loop
   * each loop lets one generation grow kills the worst individuals
   * and let new individuals born
   */
  int i, j, start, end;
  for (i = 0; i < ev->generation_limit && (!ev->use_abort_requirement
                || (ev->use_abort_requirement && ev->continue_ev(ev->individuals, ev->opts[0]))); i++) {
    
    ev->parallel.info.improovs = 0;
    ev->parallel.generations_progressed = i;
  
    /**
     * If we keep the last generation, we can recombinate in place
     * else wen wirst calculate an new populion
     */
    start = ev->keep_last_generation ? survives : ev->population_size;
    end   = ev->keep_last_generation ? ev->population_size : ev->population_size * 2; 


    if (ev->use_recmbination) {
      ev->parallel.start = ev->parallel.i = start;
      ev->parallel.end = end;

      // create threads
      for (j = 0; j < ev->parallel.num_threads; j++) {
        pthread_create(&ev->parallel.threads[j], NULL, threadable_recombinate, (void *) &ev->parallel.ev_threads[j]);
      }

      // wait untill threads finished
      for (j = 0; j < ev->parallel.num_threads; j++) {
        pthread_join(ev->parallel.threads[j], NULL);
      }

    // copy and mutate individuals
    } else {
      
      // if deaths == survives, make sure that all survivers are being copyed and mutated
      if (start * 2 == end) {
        ev->parallel.i = 0;
        ev->parallel.start = start;
        ev->parallel.end = end;

        // create threads
        for (j = 0; j < ev->parallel.num_threads; j++) {
          pthread_create(&ev->parallel.threads[j], NULL, threadable_mutation_onely_1half, (void *) &ev->parallel.ev_threads[j]);
        }
       
        // wait untill threads finished
        for (j = 0; j < ev->parallel.num_threads; j++) {
          pthread_join(ev->parallel.threads[j], NULL);
        }

      // else choose random survivers to mutate
      } else {
        ev->parallel.start = ev->parallel.i = start;
        ev->parallel.end = end;

        // create threads
        for (j = 0; j < ev->parallel.num_threads; j++) {
          pthread_create(&ev->parallel.threads[j], NULL, threadable_mutation_onely_rand, (void *) &ev->parallel.ev_threads[j]);
        }
       
        // wait untill threads finished
        for (j = 0; j < ev->parallel.num_threads; j++) {
          pthread_join(ev->parallel.threads[j], NULL);
        }
      }
    }
  

    if (!ev->keep_last_generation) {
      
      /**
       * switch old and new generation to discard the old one
       * which will be overidden next time
       */
      for (j = 0; j < ev->population_size; j++) {
        tmp_iv = ev->population[j];
        ev->population[j] = ev->population[ev->population_size + j];
        ev->population[ev->population_size + j] = tmp_iv;
      }
    }

    /**
     * Select the best individuals to survive,
     * Sort the Individuals by theur fittnes
     */
    if (ev->sort_max)
      EVOLUTION_SELECTION_MAX(ev)
    else
      EVOLUTION_SELECTION_MIN(ev)

    if (ev->verbose >= EV_VERBOSE_ONELINE)
      sprintf(ev->parallel.last_improovs_str, "%.5f", (ev->parallel.info.improovs / (double) deaths) * 100.0);

    if (ev->verbose >= EV_VERBOSE_HIGH) {
      printf("improovs: %10d -> %9s%%      best fitness: %10li                                                         \n", 
                ev->parallel.info.improovs, ev->parallel.last_improovs_str, ev->population[0]->fitness);
    }
  }

  if (ev->verbose >= EV_VERBOSE_ONELINE)
    printf("                                         "
            "                                                  \r");

  // return the best
  return ev->population[0];
}

/**
 * Parallel recombinate
 */
void *threadable_recombinate(void *arg) {
  EvolutionThread *evt = arg;
  Evolution *ev = arg;
  int j, rand1, rand2;

  while (1) {

    pthread_mutex_lock(&ev_mutex);
      if (ev->parallel.i >= ev->parallel.end)
        break;
      
      j = ev->parallel.i;
      ev->parallel.i++;
    pthread_mutex_unlock(&ev_mutex);

    // from 2 Individuals of the untouched part we calculate an new one
    rand2 = rand1 = rand() % ev->parallel.start;
    while (rand1 == rand2) rand2 = rand() % ev->parallel.start; // TODO prevent endless loop if one one idividual survives
    
    // recombinate individuals
    ev->recombinate(ev->population[rand1], ev->population[rand2], ev->population[j], ev->opts[evt->index]);
    
    // mutate Individuals
    if (ev->use_muttation) {
      if (ev->always_mutate)
        ev->mutate(ev->population[j], ev->opts[evt->index]);
      else {
        rand1 = rand() % EVOLUTION_MUTATE_ACCURACY;
        if (rand1 <= ev->parallel.mutate)
          ev->mutate(ev->population[j], ev->opts[evt->index]);
      }
    }

    // calculate the fittnes for the new individual
    ev->population[j]->fitness = ev->fitness(ev->population[j], ev->opts[evt->index]);

    /**
     * store if the new individual is better as the old one
     */
    if (ev->sort_max) {
      if (ev->population[j]->fitness > ev->population[rand1]->fitness
           && ev->population[j]->fitness > ev->population[rand2]->fitness) {
        pthread_mutex_lock(&ev_mutex);
          ev->parallel.info.improovs++;
        pthread_mutex_unlock(&ev_mutex);
      }

    } else {
      if (ev->population[j]->fitness < ev->population[rand1]->fitness
           && ev->population[j]->fitness < ev->population[rand2]->fitness) {
        pthread_mutex_lock(&ev_mutex);
          ev->parallel.info.improovs++;
        pthread_mutex_unlock(&ev_mutex);
      }
    }


    if (ev->verbose >= EV_VERBOSE_ONELINE) {
      pthread_mutex_lock(&ev_mutex);
        printf("Evolution: generation left %10d tasks recombination %10d improovs %9s%%\r", 
                ev->generation_limit - ev->parallel.generations_progressed, ev->parallel.end - j, ev->parallel.last_improovs_str);
      pthread_mutex_unlock(&ev_mutex);
    }
    if (ev->verbose >= EV_VERBOSE_ULTRA) {
      pthread_mutex_lock(&ev_mutex);
        printf("Evolution: generation left %10d tasks recombination %10d improovs %9s%%\n", 
                ev->generation_limit - ev->parallel.generations_progressed, ev->parallel.end - j, ev->parallel.last_improovs_str);
      pthread_mutex_unlock(&ev_mutex);
    }
  }

  // after break we must unlock
  pthread_mutex_unlock(&ev_mutex);

  return arg;
}

/**
 * Thread function to do mutate onely, if survives == deaths
 */
void *threadable_mutation_onely_1half(void *arg) {

  EvolutionThread *evt = arg;
  Evolution *ev = arg;
  int j;

  while (1) {

    pthread_mutex_lock(&ev_mutex);
      if (ev->parallel.i >= ev->parallel.end)
        break;
      
      j = ev->parallel.i;
      ev->parallel.i++;
    pthread_mutex_unlock(&ev_mutex);

    ev->clone_individual(ev->population[j + ev->parallel.start]->individual, ev->population[j]->individual, ev->opts[evt->index]);
    ev->mutate(ev->population[j + ev->parallel.start], ev->opts[evt->index]);

    // calculate the fittnes for the new individual
    ev->population[j + ev->parallel.start]->fitness = ev->fitness(ev->population[j + ev->parallel.start], ev->opts[evt->index]);
    
    /**
     * store if the new individual is better as the old one
     */
    if (ev->sort_max) {
      if (ev->population[j + ev->parallel.start]->fitness > ev->population[j]->fitness) {
        pthread_mutex_lock(&ev_mutex);
          ev->parallel.info.improovs++;
        pthread_mutex_unlock(&ev_mutex);
      }

    } else {
      if (ev->population[j + ev->parallel.start]->fitness < ev->population[j]->fitness) {
        pthread_mutex_lock(&ev_mutex);
          ev->parallel.info.improovs++;
        pthread_mutex_unlock(&ev_mutex);
      }
    }
    
    
    if (ev->verbose >= EV_VERBOSE_ONELINE) {
      pthread_mutex_lock(&ev_mutex);
        printf("Evolution: generation left %10d tasks mutation-1/2 %10d improovs %9s%%\r", 
                                        ev->generation_limit - ev->parallel.generations_progressed, ev->parallel.start - j, ev->parallel.last_improovs_str);
      pthread_mutex_unlock(&ev_mutex);
    }
    if (ev->verbose >= EV_VERBOSE_ULTRA) {
      pthread_mutex_lock(&ev_mutex);
        printf("Evolution: generation left %10d tasks mutation-1/2 %10d improovs %9s%%\n", 
                                        ev->generation_limit - ev->parallel.generations_progressed, ev->parallel.start - j, ev->parallel.last_improovs_str);
      pthread_mutex_unlock(&ev_mutex);
    }
  }

  // after break we must unlock
  pthread_mutex_unlock(&ev_mutex);

  return arg;
}

/**
 * Thread function to do mutate onely, if survives != deaths
 */
void *threadable_mutation_onely_rand(void *arg) {

  EvolutionThread *evt = arg;
  Evolution *ev = arg;
  int j, rand1;

  while (1) {

    pthread_mutex_lock(&ev_mutex);
      if (ev->parallel.i >= ev->parallel.end)
        break;
      
      j = ev->parallel.i;
      ev->parallel.i++;
    pthread_mutex_unlock(&ev_mutex);

    rand1 = rand() % ev->parallel.start;
    ev->clone_individual(ev->population[j]->individual, ev->population[rand1]->individual, ev->opts[evt->index]);
    ev->mutate(ev->population[j], ev->opts[evt->index]);

    // calculate the fittnes for the new individual
    ev->population[j]->fitness = ev->fitness(ev->population[j], ev->opts[evt->index]);
   
    /**
     * store if the new individual is better as the old one
     */
    if (ev->sort_max) {
      if (ev->population[j]->fitness > ev->population[rand1]->fitness) {
        pthread_mutex_lock(&ev_mutex);
          ev->parallel.info.improovs++;
        pthread_mutex_unlock(&ev_mutex);
      }

    } else {
      if (ev->population[j]->fitness < ev->population[rand1]->fitness) {
        pthread_mutex_lock(&ev_mutex);
          ev->parallel.info.improovs++;
        pthread_mutex_unlock(&ev_mutex);
      }
    }
   
   
    if (ev->verbose >= EV_VERBOSE_ONELINE) {
      pthread_mutex_lock(&ev_mutex);
        printf("Evolution: generation left %10d tasks mutation-1/x %10d improovs %9s%%\r", 
                                      ev->generation_limit - ev->parallel.generations_progressed, ev->parallel.end - j, ev->parallel.last_improovs_str);
      pthread_mutex_unlock(&ev_mutex);
    }
    if (ev->verbose >= EV_VERBOSE_ULTRA) {
      pthread_mutex_lock(&ev_mutex);
        printf("Evolution: generation left %10d tasks mutation-1/x %10d improovs %9s%%\n", 
                                      ev->generation_limit - ev->parallel.generations_progressed, ev->parallel.end - j, ev->parallel.last_improovs_str);
      pthread_mutex_unlock(&ev_mutex);
    }
  }

  // after break we must unlock
  pthread_mutex_unlock(&ev_mutex);

  return arg;
}

/**
 * returns the Size an Evolution with the given args will have
 */
u_int64_t ev_size(int population_size, int num_threads, int keep_last_generation, 
                                        u_int64_t sizeof_iv, u_int64_t sizeof_opt) {
  
  u_int64_t mul = keep_last_generation ? 1 : 2;

  u_int64_t size = (u_int64_t) sizeof(Evolution);
  size += (u_int64_t) sizeof(EvolutionThread) * num_threads;
  size += (u_int64_t) sizeof(pthread_t) * num_threads;
  size += sizeof_opt * num_threads;
  size += (u_int64_t) sizeof(Individual *) * population_size * mul;
  size += (u_int64_t) sizeof(Individual) * population_size * mul;
  size += sizeof_iv * population_size * mul;

  return size;
}

#endif // end of EVOLUTION
