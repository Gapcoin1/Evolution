#ifndef EVOLUTION
#define EVOLUTION
#include "evolution.h"
#include "C-Utils/Debug/src/debug.h"

/* static functions */
static char ev_flags_invalid(uint64_t flags);
static char valid_args(EvInitArgs *args);

// Evolution mutex
static pthread_mutex_t ev_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Functions for Sorting the Population by Fitness
 * Macro versions
 */
static inline char ev_bigger(Individual *a, Individual *b) {
  return a->fitness > b->fitness;
}

static inline char ev_smaler(Individual *a, Individual *b) {
  return a->fitness < b->fitness;
}

static inline char ev_equal(Individual *a, Individual *b) {
  return a->fitness == b->fitness;
}

/**
 * Macro for sorting the Evolution by fittnes 
 * using Macro based version onely because
 * parallel version will propably need more then
 * 16 Corse to be efficient
 */
#define EV_SELECTION(EV)                                      \
  do {                                                        \
    if ((EV)->sort_max) {                                     \
      QUICK_INSERT_SORT_MAX(Individual *,                     \
                            (EV)->population,                 \
                            (EV)->population_size,            \
                            ev_bigger,                        \
                            ev_smaler,                        \
                            ev_equal,                         \
                            (EV)->min_quicksort);             \
    } else {                                                  \
      QUICK_INSERT_SORT_MIN(Individual *,                     \
                            (EV)->population,                 \
                            (EV)->population_size,            \
                            ev_bigger,                        \
                            ev_smaler,                        \
                            ev_equal,                         \
                            (EV)->min_quicksort);             \
    }                                                         \
  } while (0)

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
 * Note: - The void pointer to individuals are not pointer to an Individual 
 *         struct, they are the individual element of the Individual struct.
 *       - The last parameter of each function (opts) is an void pointer to 
 *         optional arguments of your choice
 *
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
Evolution *new_evolution(EvInitArgs *args) {

  /* validates args */
  if (!valid_args(args))
    return NULL;

  // int random
  srand(time(NULL));

  Evolution *ev = (Evolution *) malloc(sizeof(Evolution));

  /**
   * multiplicator: if we should discard the last generation, 
   * we can't recombinate in place
   */
  int mul = ev->keep_last_generation ? 1 : 2;
  size_t population_space       = sizeof(Individual *) * args->population_size * mul;
  size_t individuals_space      = sizeof(Individual) * args->population_size * mul;

  ev->population                = (Individual **) malloc(population_space);
  ev->individuals               = (Individual *) malloc(individuals_space);
  /**
   * it's a bit trikey to initilaize a const value in an 
   * dynmic allocated struct and it gets even harder vor function pointer
   * we have to get the address where the function pointer is stored and 
   * cast this adress to an pointer of that function pointer without const
   * and dereference this pointer (of an function pointer) and set its content
   * to the given function pointer
   */
  *(void *(**)(void *)) &ev->init_individual           = args->init_individual;
  *(void (**)(void *, void *, void *)) &ev->clone_individual          = args->clone_individual;
  *(void (**)(void *, void *)) &ev->free_individual           = args->free_individual;
  *(void (**)(Individual *, void *)) &ev->mutate                    = args->mutate;
  *(int64_t (**)(Individual *, void *)) &ev->fitness                   = args->fitness;
  *(void (**)(Individual *, Individual *, Individual *, void *)) &ev->recombinate               = args->recombinate;
  *(char (**) (Individual *, void *)) &ev->continue_ev               = args->continue_ev;
  *(int *) &ev->population_size = args->population_size;
  *(int *) &ev->generation_limit = args->generation_limit;
  *(double *) &ev->mutation_propability      = args->mutation_propability;
  *(double *) &ev->death_percentage          = args->death_percentage;
  *(void ***) &ev->opts                      = args->opts;
  *(int *) &ev->num_threads      = args->num_threads;
  ev->parallel_index                = 0;
  ev->info.improovs    = 0;
  *(TClient **) &ev->thread_clients   = malloc(sizeof(TClient) * args->num_threads);
  ev->thread_args       = malloc(sizeof(EvThreadArgs) 
                                         * args->num_threads);
  *(int *) &ev->i_mut_propability           = (int) ((double) RAND_MAX 
                                         * ev->mutation_propability);
  *(char *) &ev->use_recombination          = args->flags & EV_USE_RECOMBINATION;
  *(char *) &ev->use_muttation             = args->flags & EV_USE_MUTATION;
  *(char *) &ev->always_mutate             = args->flags & EV_ALWAYS_MUTATE;
  *(char *) &ev->keep_last_generation      = args->flags & EV_KEEP_LAST_GENERATION;
  *(char *) &ev->use_abort_requirement     = args->flags & EV_USE_ABORT_REQUIREMENT;
  *(char *) &ev->sort_max                  = args->flags & EV_SORT_MAX;
  *(uint64_t *) &ev->verbose                   = args->flags & (EV_VERBOSE_ONELINE | EV_VERBOSE_HIGH);
  *(int *) &ev->min_quicksort             = EV_QICKSORT_MIN;

  /**
   * init thread lib
   * max work will be death_percentage * num individuals
   */
  int i;
  for (i = 0; i < args->num_threads; i++)
    init_thread_client(&ev->thread_clients[i]);


  ev->parallel_index = args->population_size * mul;

  /* add work for the clients */
  for (i = 0; i < args->num_threads; i++) {
    *(Evolution **) &ev->thread_args[i].ev = ev;
    *(int *) &ev->thread_args[i].index = i;
    tc_add_func(&ev->thread_clients[i], threadable_init_individual, (void *) &ev->thread_args[i]);
  }

  // wait for threads
  for (i = 0; i < args->num_threads; i++) {
    tc_join(&ev->thread_clients[i]);
  }

  /**
   * Select the best individuals to survive,
   * Sort the Individuals by their fittnes
   */
  EV_SELECTION(ev); 

  if (ev->verbose >= EV_VERBOSE_HIGH)
    printf("Population Initialized\n");

  return ev;
}

/**
 * Returns wether the given EvInitArgs are valid or not
 */
static char valid_args(EvInitArgs *args) {

  if (ev_flags_invalid(args->flags)) {
    DBG_MSG("wrong flag combination");
    return 0;
  }

  // valid other opts
  if (args->population_size      <= 0  ||
      args->generation_limit     <= 0  ||
      args->mutation_propability < 0.0 ||
      args->mutation_propability > 1.0 ||
      args->death_percentage     < 0.0 ||
      args->death_percentage     > 1.0) {
   
    DBG_MSG("wrong opts");
    return 0;
  }

  return 1;
}


/**
 * Returns if a given flag combination is invalid
 */
static char ev_flags_invalid(uint64_t flags) {

  /**
   * Sorting and verbose flags
   * can be used with any flag combination
   */
  int tflags = flags & ~EV_SMAX;
  tflags &= ~EV_VEB1;
  tflags &= ~EV_VEB2;
  tflags &= ~EV_VEB3;
  
  return tflags != EV_UREC 
         && tflags != (EV_UREC|EV_UMUT) 
         && tflags != (EV_UREC|EV_UMUT|EV_AMUT)
         && tflags != (EV_UREC|EV_KEEP) 
         && tflags != (EV_UREC|EV_UMUT|EV_KEEP)
         && tflags != (EV_UMUT|EV_AMUT|EV_KEEP) 
         && tflags != (EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP)
         && tflags != (EV_UREC|EV_ABRT) 
         && tflags != (EV_UREC|EV_UMUT|EV_ABRT)
         && tflags != (EV_UMUT|EV_AMUT|EV_ABRT) 
         && tflags != (EV_UREC|EV_UMUT|EV_AMUT|EV_ABRT)
         && tflags != (EV_UREC|EV_KEEP|EV_ABRT) 
         && tflags != (EV_UREC|EV_UMUT|EV_KEEP|EV_ABRT)
         && tflags != (EV_UMUT|EV_AMUT|EV_KEEP|EV_ABRT)
         && tflags != (EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP|EV_ABRT);

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
Individual best_evolution(EvInitArgs *args) {

  Evolution *ev = new_evolution(args);

  Individual best = *evolute(ev);
  evolution_clean_up(ev);

  return best;
}

/**
 * Parallel init_individual function
 */
void *threadable_init_individual(void *arg) {
  EvThreadArgs *evt = arg;
  Evolution *ev = evt->ev;
  int i;

  while (1) {
     
    pthread_mutex_lock(&ev_mutex);
      if (ev->parallel_index <= 0)
        break;
 
      ev->parallel_index--;
      i = ev->parallel_index;
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
Individual *evolute(Evolution *ev) {

  int deaths   = (int) ((double) ev->population_size * ev->death_percentage);
  int survives = ev->population_size - deaths;
  Individual *tmp_iv;
  sprintf(ev->last_improovs_str, "Not knowen");

  /**
   * Generation loop
   * each loop lets one generation grow kills the worst individuals
   * and let new individuals born
   */
  int i, j, start, end;
  for (i = 0; i < ev->generation_limit && (!ev->use_abort_requirement
                || (ev->use_abort_requirement && ev->continue_ev(ev->individuals, ev->opts[0]))); i++) {
    
    ev->info.improovs = 0;
    ev->info.generations_progressed = i;
  
    /**
     * If we keep the last generation, we can recombinate in place
     * else wen wirst calculate an new populion
     */
    start = ev->keep_last_generation ? survives : ev->population_size;
    end   = ev->keep_last_generation ? ev->population_size : ev->population_size * 2; 


    if (ev->use_recombination) {
      ev->parallel_start = ev->parallel_index = start;
      ev->parallel_end = end;

      // create threads
      for (j = 0; j < ev->num_threads; j++) {
        tc_add_func(&ev->thread_clients[j], threadable_recombinate, (void *) &ev->thread_args[j]);
      }

      // wait untill threads finished
      for (j = 0; j < ev->num_threads; j++) {
        tc_join(&ev->thread_clients[j]);
      }

    // copy and mutate individuals
    } else {
      
      // if deaths == survives, make sure that all survivers are being copyed and mutated
      if (start * 2 == end) {
        ev->parallel_index = 0;
        ev->parallel_start = start;
        ev->parallel_end = end;

        // create threads
        for (j = 0; j < ev->num_threads; j++) {
          tc_add_func(&ev->thread_clients[j], threadable_mutation_onely_1half, (void *) &ev->thread_args[j]);
        }
       
        // wait untill threads finished
        for (j = 0; j < ev->num_threads; j++) {
          tc_join(&ev->thread_clients[j]);
        }

      // else choose random survivers to mutate
      } else {
        ev->parallel_start = ev->parallel_index = start;
        ev->parallel_end = end;

        // create threads
        for (j = 0; j < ev->num_threads; j++) {
          tc_add_func(&ev->thread_clients[j], threadable_mutation_onely_rand, (void *) &ev->thread_args[j]);
        }
       
        // wait untill threads finished
        for (j = 0; j < ev->num_threads; j++) {
          tc_join(&ev->thread_clients[j]);
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
    EV_SELECTION(ev);

    if (ev->verbose >= EV_VERBOSE_ONELINE)
      sprintf(ev->last_improovs_str, "%.5f", (ev->info.improovs / (double) deaths) * 100.0);

    if (ev->verbose >= EV_VERBOSE_HIGH) {
      printf("improovs: %10d -> %9s%%      best fitness: %10li                                                         \n", 
                ev->info.improovs, ev->last_improovs_str, ev->population[0]->fitness);
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
  EvThreadArgs *evt = arg;
  Evolution *ev = evt->ev;
  int j, rand1, rand2;

  while (1) {

    pthread_mutex_lock(&ev_mutex);
      if (ev->parallel_index >= ev->parallel_end)
        break;
      
      j = ev->parallel_index;
      ev->parallel_index++;
    pthread_mutex_unlock(&ev_mutex);

    // from 2 Individuals of the untouched part we calculate an new one
    rand2 = rand1 = rand() % ev->parallel_start;
    while (rand1 == rand2) rand2 = rand() % ev->parallel_start; // TODO prevent endless loop if one one idividual survives
    
    // recombinate individuals
    ev->recombinate(ev->population[rand1], ev->population[rand2], ev->population[j], ev->opts[evt->index]);
    
    // mutate Individuals
    if (ev->use_muttation) {
      if (ev->always_mutate)
        ev->mutate(ev->population[j], ev->opts[evt->index]);
      else {
        if (rand() <= ev->i_mut_propability)
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
          ev->info.improovs++;
        pthread_mutex_unlock(&ev_mutex);
      }

    } else {
      if (ev->population[j]->fitness < ev->population[rand1]->fitness
           && ev->population[j]->fitness < ev->population[rand2]->fitness) {
        pthread_mutex_lock(&ev_mutex);
          ev->info.improovs++;
        pthread_mutex_unlock(&ev_mutex);
      }
    }


    if (ev->verbose >= EV_VERBOSE_ONELINE) {
      pthread_mutex_lock(&ev_mutex);
        printf("Evolution: generation left %10d tasks recombination %10d improovs %9s%%\r", 
                ev->generation_limit - ev->info.generations_progressed, ev->parallel_end - j, ev->last_improovs_str);
      pthread_mutex_unlock(&ev_mutex);
    }
    if (ev->verbose >= EV_VERBOSE_ULTRA) {
      pthread_mutex_lock(&ev_mutex);
        printf("Evolution: generation left %10d tasks recombination %10d improovs %9s%%\n", 
                ev->generation_limit - ev->info.generations_progressed, ev->parallel_end - j, ev->last_improovs_str);
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

  EvThreadArgs *evt = arg;
  Evolution *ev = evt->ev;
  int j;

  while (1) {

    pthread_mutex_lock(&ev_mutex);
      if (ev->parallel_index + ev->parallel_start >= ev->parallel_end)
        break;
      
      j = ev->parallel_index;
      ev->parallel_index++;
    pthread_mutex_unlock(&ev_mutex);

    ev->clone_individual(ev->population[j + ev->parallel_start]->individual, ev->population[j]->individual, ev->opts[evt->index]);
    ev->mutate(ev->population[j + ev->parallel_start], ev->opts[evt->index]);

    // calculate the fittnes for the new individual
    ev->population[j + ev->parallel_start]->fitness = ev->fitness(ev->population[j + ev->parallel_start], ev->opts[evt->index]);
    
    /**
     * store if the new individual is better as the old one
     */
    if (ev->sort_max) {
      if (ev->population[j + ev->parallel_start]->fitness > ev->population[j]->fitness) {
        pthread_mutex_lock(&ev_mutex);
          ev->info.improovs++;
        pthread_mutex_unlock(&ev_mutex);
      }

    } else {
      if (ev->population[j + ev->parallel_start]->fitness < ev->population[j]->fitness) {
        pthread_mutex_lock(&ev_mutex);
          ev->info.improovs++;
        pthread_mutex_unlock(&ev_mutex);
      }
    }
    
    
    if (ev->verbose >= EV_VERBOSE_ONELINE) {
      pthread_mutex_lock(&ev_mutex);
        printf("Evolution: generation left %10d tasks mutation-1/2 %10d improovs %9s%%\r", 
                                        ev->generation_limit - ev->info.generations_progressed, ev->parallel_start - j, ev->last_improovs_str);
      pthread_mutex_unlock(&ev_mutex);
    }
    if (ev->verbose >= EV_VERBOSE_ULTRA) {
      pthread_mutex_lock(&ev_mutex);
        printf("Evolution: generation left %10d tasks mutation-1/2 %10d improovs %9s%%\n", 
                                        ev->generation_limit - ev->info.generations_progressed, ev->parallel_start - j, ev->last_improovs_str);
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

  EvThreadArgs *evt = arg;
  Evolution *ev = evt->ev;
  int j, rand1;

  while (1) {

    pthread_mutex_lock(&ev_mutex);
      if (ev->parallel_index >= ev->parallel_end)
        break;
      
      j = ev->parallel_index;
      ev->parallel_index++;
    pthread_mutex_unlock(&ev_mutex);

    rand1 = rand() % ev->parallel_start;
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
          ev->info.improovs++;
        pthread_mutex_unlock(&ev_mutex);
      }

    } else {
      if (ev->population[j]->fitness < ev->population[rand1]->fitness) {
        pthread_mutex_lock(&ev_mutex);
          ev->info.improovs++;
        pthread_mutex_unlock(&ev_mutex);
      }
    }
   
   
    if (ev->verbose >= EV_VERBOSE_ONELINE) {
      pthread_mutex_lock(&ev_mutex);
        printf("Evolution: generation left %10d tasks mutation-1/x %10d improovs %9s%%\r", 
               ev->generation_limit - ev->info.generations_progressed, ev->parallel_end - j, ev->last_improovs_str);
      pthread_mutex_unlock(&ev_mutex);
    }
    if (ev->verbose >= EV_VERBOSE_ULTRA) {
      pthread_mutex_lock(&ev_mutex);
        printf("Evolution: generation left %10d tasks mutation-1/x %10d improovs %9s%%\n", 
               ev->generation_limit - ev->info.generations_progressed, ev->parallel_end - j, ev->last_improovs_str);
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
uint64_t ev_size(int population_size, int num_threads, int keep_last_generation, 
                                        uint64_t sizeof_iv, uint64_t sizeof_opt) {
  
  uint64_t mul = keep_last_generation ? 1 : 2;

  uint64_t size = (uint64_t) sizeof(Evolution);
  size += (uint64_t) sizeof(EvThreadArgs) * num_threads;
  size += (uint64_t) sizeof(pthread_t) * num_threads;
  size += sizeof_opt * num_threads;
  size += (uint64_t) sizeof(Individual *) * population_size * mul;
  size += (uint64_t) sizeof(Individual) * population_size * mul;
  size += sizeof_iv * population_size * mul;

  return size;
}

#endif // end of EVOLUTION
