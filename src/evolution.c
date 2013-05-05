#ifndef EVOLUTION
#define EVOLUTION
#include "evolution.h"
#include "C-Utils/Debug/src/debug.h"


/* Evolution mutex */
static pthread_mutex_t ev_mutex = PTHREAD_MUTEX_INITIALIZER;

/********************/
/* static functions */
/********************/

/**
 * Initializes Thread Clients and Individuals
 */
static void ev_init_tc_and_ivs(Evolution *ev);

/**
 * Returns wether the given EvInitArgs are valid or not
 */
static char valid_args(EvInitArgs *args);

/**
 * Returns if a given flag combination is invalid
 */
static char ev_flags_invalid(uint64_t flags);

/**
 * Parallel init_iv function
 */
static void *threadable_init_iv(void *arg);

/**
 * Recombinates individual dunring an Generation change
 */
static inline void ev_init_recombinate(Evolution *ev);

/**
 * Mutate all individuals if we don't use recombination
 */
static inline void ev_init_mutate(Evolution *ev);

/**
 * switch old and new generation to discard the old one
 * which will be overidden at the next generation change
 */
static inline void ev_switch_ivs(Evolution *ev);

/**
 * Wakeup the Threadabel functions
 * an waits untill all work is done
 * and count the improovs of the
 * progressed generation
 */
static inline void evolute_ivs(Evolution *ev);

/**
 * Initializes the evolution process
 * by configurating and starting the threads
 * and choosing what to be done
 */
static inline void init_evolute(Evolution *ev);

/**
 * Finishes the Evolution progresse by
 * shutting down the threads
 */
static inline void close_evolute(Evolution *ev);

/**
 * Parallel recombinate
 */
static void *threadable_recombinate(void *arg);

/**
 * Thread function to do mutation onely
 * if number of survivors == number of deaths
 */
static void *threadable_mutation_onely_1half(void *arg);

/**
 * Thread function to do mutation onely
 * if numbers of survivors != number of deaths
 */
static void *threadable_mutation_onely_rand(void *arg);

/**
 * Functions for sorting the population by fitness
 * macro versions
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
 * Access the fittnes of the individual
 * at the given possition in the given Evolution
 * with the given opts
 */
#define EV_FITNESS_AT(EV, I) (EV).population[I]->fitness 

/**
 * Calculates the fittnes of the individual
 * at the given possition in the given Evolution
 * with the given opts
 */
#define EV_CALC_FITNESS_AT(EV, I, OPTS)                                       \
    (EV).population[I]->fitness = (EV).fitness((EV).population[I], OPTS)

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
 * Prints status information during 
 * threadable init individuals 
 */
#define EV_INIT_IV_OUTPUT(INDEX)                                              \
  do {                                                                        \
    pthread_mutex_lock(&ev_mutex);                                            \
    printf("init population: %10d\r", INDEX);                                 \
    pthread_mutex_unlock(&ev_mutex);                                          \
  } while (0)

/**
 * Prints status information during 
 * threadable recombination and mutation TODO make seperate output vor recombination mut_1/2
 */
#define EV_IV_STATUS_OUTPUT(EV, INDEX)                                        \
  do {                                                                        \
      pthread_mutex_lock(&ev_mutex);                                          \
      printf("Evolution: generation left %10d "                               \
             "tasks mutation-1/x %10d improovs %15.5f%%\r",                   \
             (EV).generation_limit - (EV).info.generations_progressed,        \
             (EV).overall_end - INDEX,                                       \
             (((EV).info.improovs / (double) (EV).deaths) * 100.0));          \
      pthread_mutex_unlock(&ev_mutex);                                        \
  } while (0)

/**
 * Prints status info during
 * main elvoute function
 */
#define EV_EVOLUTE_OUTPUT(EV)                                                 \
    printf("\33[2K\rimproovs: %10d -> %15.5f%%  best fitness: %10li\n",       \
           (EV).info.improovs,                                                \
           (((EV).info.improovs / (double) (EV).deaths) * 100.0),             \
           (EV).population[0]->fitness)

#define EV_THREAD_SAVE_NEW_LINE                                               \
  do {                                                                        \
    pthread_mutex_lock(&ev_mutex);                                            \
    printf("\n");                                                             \
    pthread_mutex_unlock(&ev_mutex);                                          \
  } while (0)
    


/**
 * Sadly we have to subvert the type system to initialize an const members
 * of an struct on the heap. (there are other ways with memcpy and an static
 * initialized const struct, but thats C99 and it will get even uglier than
 * than this)
 * For explanation we have to get the address of the const value cast this
 * address to an pointer of an non const version of the specific value
 * and dereference this pointer to set the acctual const value
 */
#define INIT_C_INIT_IV(X, Y) *(void *(**)(void *))                 &(X) = (Y)
#define INIT_C_CLON_IV(X, Y) *(void (**)(void *, void *, void *))  &(X) = (Y)
#define INIT_C_FREE_IV(X, Y) *(void (**)(void *, void *))          &(X) = (Y)
#define INIT_C_MUTTATE(X, Y) *(void (**)(Individual *, void *))    &(X) = (Y)
#define INIT_C_FITNESS(X, Y) *(int64_t (**)(Individual *, void *)) &(X) = (Y)
#define INIT_C_RECOMBI(X, Y) *(void (**)(Individual *,                        \
                                         Individual *,                        \
                                         Individual *,                        \
                                         void *))                  &(X) = (Y)
#define INIT_C_CONTINU(X, Y) *(char (**)(Evolution *const))        &(X) = (Y)
#define INIT_C_EVTARGS(X, Y) *(EvThreadArgs **)                    &(X) = (Y)
#define INIT_C_INT(X, Y)     *(int *)                              &(X) = (Y)
#define INIT_C_DBL(X, Y)     *(double *)                           &(X) = (Y)
#define INIT_C_OPT(X, Y)     *(void ***)                           &(X) = (Y)
#define INIT_C_TCS(X, Y)     *(TClient **)                         &(X) = (Y)
#define INIT_C_CHR(X, Y)     *(char *)                             &(X) = (Y)
#define INIT_C_U64(X, Y)     *(uint64_t *)                         &(X) = (Y)
#define INIT_C_U16(X, Y)     *(uint16_t *)                         &(X) = (Y)
#define INIT_C_EVO(X, Y)     *(Evolution **)                       &(X) = (Y)
#define INIT_C_VPT(X, Y)     *(void **)                            &(X) = (Y)

/**
 * Returns pointer to an new and initialzed Evolution
 * take pointers for an EvInitArgs struct
 *
 * see comment of EvInitArgs for more informations
 */
Evolution *new_evolution(EvInitArgs *args) {

  /* validates args */
  if (!valid_args(args))
    return NULL;

  /* int random */
  srand(time(NULL));

  /* create new Evolution */
  Evolution *ev = (Evolution *) malloc(sizeof(Evolution));

  /**
   * multiplicator: if we should discard the last generation, 
   * we can't recombinate in place
   */
  int mul = (args->flags & EV_KEEP) ? 1 : 2;

  size_t population_space  = sizeof(Individual *); 
  size_t ivs_space         = sizeof(Individual); 

  population_space         *= args->population_size * mul;
  ivs_space                *= args->population_size * mul;
                           
  ev->population           = (Individual **) malloc(population_space);
  ev->ivs                  = (Individual *) malloc(ivs_space);

  INIT_C_INIT_IV(ev->init_iv,     args->init_iv);
  INIT_C_CLON_IV(ev->clone_iv,    args->clone_iv);
  INIT_C_FREE_IV(ev->free_iv,     args->free_iv);
  INIT_C_MUTTATE(ev->mutate,      args->mutate);
  INIT_C_FITNESS(ev->fitness,     args->fitness);
  INIT_C_RECOMBI(ev->recombinate, args->recombinate);
  INIT_C_CONTINU(ev->continue_ev, args->continue_ev);
  INIT_C_EVTARGS(ev->thread_args, malloc(sizeof(EvThreadArgs) *
                                               args->num_threads));

  INIT_C_INT(ev->population_size,       args->population_size);
  INIT_C_INT(ev->generation_limit,      args->generation_limit);
  INIT_C_DBL(ev->mutation_propability,  args->mutation_propability);
  INIT_C_DBL(ev->death_percentage,      args->death_percentage);
  INIT_C_OPT(ev->opts,                  args->opts);
  INIT_C_INT(ev->num_threads,           args->num_threads);
  INIT_C_TCS(ev->thread_clients,        malloc(sizeof(TClient) * 
                                               args->num_threads));

  INIT_C_INT(ev->i_mut_propability,     (int) ((double) RAND_MAX * 
                                               ev->mutation_propability));

  INIT_C_CHR(ev->use_recombination,     args->flags & EV_UREC);
  INIT_C_CHR(ev->use_muttation,         args->flags & EV_UMUT);
  INIT_C_CHR(ev->always_mutate,         args->flags & EV_AMUT);
  INIT_C_CHR(ev->keep_last_generation,  args->flags & EV_KEEP);
  INIT_C_CHR(ev->use_abort_requirement, args->flags & EV_ABRT);
  INIT_C_CHR(ev->sort_max,              args->flags & EV_SMAX);
  INIT_C_U16(ev->verbose,               args->flags & (EV_VEB1 |
                                                       EV_VEB2 |
                                                       EV_VEB3));

  INIT_C_INT(ev->min_quicksort,         EV_QICKSORT_MIN);
  INIT_C_INT(ev->deaths,                (int) ((double) ev->population_size * 
                                               ev->death_percentage));
  INIT_C_INT(ev->survivors,             ev->population_size - ev->deaths);

  ev->threads_running                   = 0;
  ev->info.improovs                     = 0;
  ev->info.generations_progressed       = 0;

  /**
   * Initializes Thread Clients and Individuals
   */
  ev_init_tc_and_ivs(ev);

  return ev;
}

/**
 * Initializes Thread Clients and Individuals
 */
static void ev_init_tc_and_ivs(Evolution *ev) {

  /**
   * init thread lib
   * max work will be death_percentage * num individuals
   */
  int i;
  for (i = 0; i < ev->num_threads; i++)
    init_thread_client(&ev->thread_clients[i]);

  /**
   * multiplicator: if we should discard the last generation, 
   * we can't recombinate in place
   */
  int mul = ev->keep_last_generation ? 1 : 2;

  /* start and end of calculation */
  ev->overall_start = 0;
  ev->overall_end   = ev->population_size * mul;

  /**
   * number of individuals calculated by one thread
   */
  uint32_t ivs_per_thread = (ev->overall_end - ev->overall_start) /
                            ev->num_threads + 1;

  /* add work for the clients */
  for (i = 0; i < ev->num_threads; i++) {

    /* init thread args */
    INIT_C_EVO(ev->thread_args[i].ev,    ev);
    INIT_C_INT(ev->thread_args[i].index, i);
    INIT_C_VPT(ev->thread_args[i].opt,   ev->opts[i]);
    ev->thread_args[i].waiting  = 1;
    ev->thread_args[i].improovs = 0;

    /* set working area of thread i */
    ev->thread_args[i].start = ev->overall_start + i * ivs_per_thread;
    ev->thread_args[i].end   = ev->thread_args[i].start + ivs_per_thread;

    if (ev->thread_args[i].end > ev->overall_end)
      ev->thread_args[i].end = ev->overall_end;

    tc_add_func(&ev->thread_clients[i], 
                threadable_init_iv, 
                (void *) &ev->thread_args[i]);
  }

  // wait for threads
  for (i = 0; i < ev->num_threads; i++) {
    tc_join(&ev->thread_clients[i]);
  }

  /**
   * Select the best individual to survive,
   * Sort the Individuals by their fittnes
   */
  EV_SELECTION(ev); 

  if (ev->verbose >= EV_VERBOSE_HIGH)
    printf("Population Initialized\n");

}

/**
 * Undef const initializer to avoid reusing it
 */
#undef INIT_C_INIT_IV  
#undef INIT_C_CLON_IV 
#undef INIT_C_FREE_IV  
#undef INIT_C_MUTTATE   
#undef INIT_C_FITNESS  
#undef INIT_C_RECOMBI    
#undef INIT_C_CONTINU   
#undef INIT_C_EVTARGS
#undef INIT_C_INT      
#undef INIT_C_DBL      
#undef INIT_C_OPT     
#undef INIT_C_TCS      
#undef INIT_C_CHR      
#undef INIT_C_U64 
#undef INIT_C_U16
#undef INIT_C_EVO
#undef INIT_C_VPT

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
  
  return tflags != EV_UREC                           &&
         tflags != (EV_UREC|EV_UMUT)                 &&
         tflags != (EV_UMUT|EV_AMUT)                 &&
         tflags != (EV_UREC|EV_UMUT|EV_AMUT)         &&
         tflags != (EV_UREC|EV_KEEP)                 &&
         tflags != (EV_UREC|EV_UMUT|EV_KEEP)         &&
         tflags != (EV_UMUT|EV_AMUT|EV_KEEP)         &&
         tflags != (EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP) &&
         tflags != (EV_UREC|EV_ABRT)                 &&
         tflags != (EV_UREC|EV_UMUT|EV_ABRT)         &&
         tflags != (EV_UMUT|EV_AMUT|EV_ABRT)         &&
         tflags != (EV_UREC|EV_UMUT|EV_AMUT|EV_ABRT) &&
         tflags != (EV_UREC|EV_KEEP|EV_ABRT)         &&
         tflags != (EV_UREC|EV_UMUT|EV_KEEP|EV_ABRT) &&
         tflags != (EV_UMUT|EV_AMUT|EV_KEEP|EV_ABRT) &&
         tflags != (EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP|EV_ABRT);

}

/**
 * Frees unneded resauces after an evolution calculation
 * Note it don't touches the spaces of the best individual
 */
void evolution_clean_up(Evolution *ev) {
  int end = ev->keep_last_generation ? ev->population_size : 
                                       ev->population_size * 2; 
  int i;

  /**
   * free individuals starting by index one because
   * zero is the best individual
   */
  for (i = 1; i < end; i++) 
    ev->free_iv(ev->population[i]->iv, 
                ev->opts[i]);

  free(ev->ivs);
  free(ev->population);
}

/**
 * Computes an evolution for the given args
 * and returns the best Individual
 */
Individual best_evolution(EvInitArgs *args) {

  Evolution *ev   = new_evolution(args);
  Individual best = *evolute(ev);
  evolution_clean_up(ev);

  return best;
}

/**
 * Parallel init_iv function
 */
static void *threadable_init_iv(void *arg) {

  EvThreadArgs *evt = arg;
  Evolution *ev     = evt->ev;
  int i;

  /**
   * Loop untill all individuals of this thread are initialized
   */
  for (i = evt->start; i < evt->end; i++) {
     
    /**
     * create new individual
     */
    ev->population[i]          = ev->ivs + i;
    ev->ivs[i].iv              = ev->init_iv(evt->opt);
    ev->population[i]->fitness = ev->fitness(ev->population[i], 
                                             evt->opt);  
    /**
     * prints status informations if wanted
     */
    if (ev->verbose >= EV_VERBOSE_ONELINE) {
      EV_INIT_IV_OUTPUT(i);

      if (ev->verbose >= EV_VERBOSE_HIGH)
        EV_THREAD_SAVE_NEW_LINE;
    }
  }

  return NULL;

}

/**
 * Recombinates individual dunring an Generation change
 */
static inline void ev_init_recombinate(Evolution *ev) {

  int j;

  /**
   * If we keep the last generation, we can recombinate in place
   * else we first calculate an new population (start -> end,
   * is the area where the individuals will be overidden by new ones
   */
  if (ev->keep_last_generation) {
    ev->overall_start = ev->survivors;
    ev->overall_end   = ev->population_size;
  } else {
    ev->overall_start = ev->population_size;
    ev->overall_end   = ev->population_size * 2;
  }

  /**
   * number of individuals calculated by one thread
   */
  uint32_t ivs_per_thread = (ev->overall_end - ev->overall_start) /
                            ev->num_threads + 1;

  /* start parallel working */
  for (j = 0; j < ev->num_threads; j++) {

    ev->thread_args[j].start = ev->overall_start + j * ivs_per_thread;
    ev->thread_args[j].end   = ev->thread_args[j].start + ivs_per_thread;

    if (ev->thread_args[j].end > ev->overall_end)
      ev->thread_args[j].end = ev->overall_end;

    tc_add_func(&ev->thread_clients[j], 
                threadable_recombinate, 
                (void *) &ev->thread_args[j]);
  }

}

/**
 * Mutate all individuals if we don't use recombination
 */
static inline void ev_init_mutate(Evolution *ev) {

  int j;

  /**
   * If we keep the last generation, we can recombinate in place
   * else we first calculate an new population (start -> end,
   * is the area where the individuals will be overidden by new ones
   */
  if (ev->keep_last_generation) {
    ev->overall_start = ev->survivors;
    ev->overall_end   = ev->population_size;
  } else {
    ev->overall_start = ev->population_size;
    ev->overall_end   = ev->population_size * 2;
  }

  /**
   * number of individuals calculated by one thread
   */
  uint32_t ivs_per_thread = (ev->overall_end - ev->overall_start) /
                            ev->num_threads + 1;

  /* setting start and end areas for each thread */
  for (j = 0; j < ev->num_threads; j++) {

    ev->thread_args[j].start = ev->overall_start + j * ivs_per_thread;
    ev->thread_args[j].end   = ev->thread_args[j].start + ivs_per_thread;

    if (ev->thread_args[j].end > ev->overall_end)
      ev->thread_args[j].end = ev->overall_end;

  }
     
  /**
   * if deaths == survivors, making sure that all survivers 
   * are being copyed and mutated
   */
  if (ev->overall_start * 2 == ev->overall_end) {

    /* start parallel working */
    for (j = 0; j < ev->num_threads; j++) {

      tc_add_func(&ev->thread_clients[j], 
                  threadable_mutation_onely_1half, 
                  (void *) &ev->thread_args[j]);
    }

  /* else choose random survivers to mutate */
  } else {

    /* start parallel working */
    for (j = 0; j < ev->num_threads; j++) {

      tc_add_func(&ev->thread_clients[j], 
                  threadable_mutation_onely_rand, 
                  (void *) &ev->thread_args[j]);
    }
  }

}

/**
 * switch old and new generation to discard the old one
 * which will be overidden at the next generation change
 */
static inline void ev_switch_ivs(Evolution *ev) {

  int j;
  Individual *tmp_iv;

  for (j = 0; j < ev->population_size; j++) {
    tmp_iv = ev->population[j];
    ev->population[j] = ev->population[ev->population_size + j];
    ev->population[ev->population_size + j] = tmp_iv;
  }
}

/**
 * Wakeup the Threadabel functions
 * an waits untill all work is done
 * and count the improovs of the
 * progressed generation
 */
static inline void evolute_ivs(Evolution *ev) {
  
  int j;

  /**
   * wakeup all threads
   */
  for (j = 0; j < ev->num_threads; j++) 
    ev->thread_args[j].waiting = 0;

  /**
   * Wait untill all threads are finished
   */
  for (j = 0; j < ev->num_threads; j++) 
    while (!ev->thread_args[j].waiting)
      sched_yield();

  /**
   * resets and count improovs
   * of the finished threads
   */
  ev->info.improovs = 0;

  for (j = 0; j < ev->num_threads; j++)
    ev->info.improovs += ev->thread_args[j].improovs;

}

/**
 * Initializes the evolution process
 * by configurating and starting the threads
 * and choosing what to be done
 */
static inline void init_evolute(Evolution *ev) {

  /* start threads */
  ev->threads_running = 1;

  /**
   * init recombinatin or mutatation
   * depeding on given flags in init
   */
  if (ev->use_recombination)
    ev_init_recombinate(ev);
  else 
    ev_init_mutate(ev);
  
}

/**
 * Finishes the Evolution progresse by
 * shutting down the threads
 */
static inline void close_evolute(Evolution *ev) {

  int j;
  
  /* shudtown threads */
  ev->threads_running = 0;

  /* wait untill all threads finished */
  for (j = 0; j < ev->num_threads; j++) 
    tc_join(&ev->thread_clients[j]);

  /* clear line if neccesary */
  if (ev->verbose >= EV_VERBOSE_ONELINE)
    printf("\33[2K\r");

}

/**
 * do the actual evolution, which means
 *  - calculate the fitness for each Individual
 *  - sort Individual by fitness
 *  - remove worst ivs
 *  - grow a new generation
 *
 * Retruns the best individual
 */
Individual *evolute(Evolution *ev) {

  int i;

  /**
   * initalize the evolution process
   */
  init_evolute(ev);

  /**
   * Generation loop
   * each loop lets one generation grow kills the worst individuals
   * and let new individuals born (depending on the previous given flags)
   */
  for (i = 0; 
       i < ev->generation_limit && 
       (!ev->use_abort_requirement || 
        ev->continue_ev(ev)); 
       i++) {

    /**
     * recombinates or mutates individuals
     * depeding on given flags in init
     */
    evolute_ivs(ev);
  
    /**
     * switch old and new generation to discard the old one
     * which will be overidden next time
     */
    if (!ev->keep_last_generation)
      ev_switch_ivs(ev);

    /**
     * Select the best individuals to survindividuale,
     * Sort the Individuals by theur fittnes
     */
    EV_SELECTION(ev);

    /* update progressed generations */
    ev->info.generations_progressed = i + 1;

    /**
     * print status informations if wanted
     */
    if (ev->verbose >= EV_VERBOSE_HIGH)
      EV_EVOLUTE_OUTPUT(*ev);
  }

  /* shutdown threads */
  close_evolute(ev);

  // return the best
  return ev->population[0];
}


/**
 * Parallel recombinate
 */
static void *threadable_recombinate(void *arg) {

  EvThreadArgs *evt = arg;
  Evolution *ev     = evt->ev;
  int j, rand1, rand2;

  /**
   * for recombination there musst be min two individuals
   */
  if (ev->survivors <= 1)
    ERR_MSG("less than 2 individuals in recombinate");

  /**
   * during the actual evolution we keep threads running
   * and waiting till there is next next work
   */
  while (ev->threads_running) {

    /* wait untill there is new work todo */
    while (evt->waiting) {
      if (!ev->threads_running) return NULL;
      sched_yield();
    }

    /* reset threadwide iprooves */
    evt->improovs = 0;

    /**
     * loop untill all recombinations of this thread are done
     */
    for (j = evt->start; j < evt->end; j++) {

      /**
       * from two randomly choosen Individuals 
       * of the untouched (best) part we calculate an new one 
       * */
      rand2 = rand1 = rand() % ev->overall_start;
      while (rand1 == rand2) rand2 = rand() % ev->overall_start; 
      
      /* recombinate individuals */
      ev->recombinate(ev->population[rand1], 
                      ev->population[rand2], 
                      ev->population[j], 
                      evt->opt);
      
      /* mutate Individuals */
      if (ev->use_muttation) {
        if (ev->always_mutate)
          ev->mutate(ev->population[j], evt->opt);
        else {
          if (rand() <= ev->i_mut_propability)
            ev->mutate(ev->population[j], evt->opt);
        }
      }

      /* calculate the fittnes for the new individuals */
      ev->population[j]->fitness = ev->fitness(ev->population[j], 
                                               evt->opt);

      /**
       * store if the new individual is better as the old one
       */
      if (ev->sort_max) {
        if (ev->population[j]->fitness > ev->population[rand1]->fitness && 
            ev->population[j]->fitness > ev->population[rand2]->fitness) {

          evt->improovs++;
        }

      } else {
        if (ev->population[j]->fitness < ev->population[rand1]->fitness && 
            ev->population[j]->fitness < ev->population[rand2]->fitness) {

          evt->improovs++;
        }
      }

      /**
       * print status informations if wanted
       */
      if (ev->verbose >= EV_VERBOSE_ONELINE) {
        EV_IV_STATUS_OUTPUT(*ev, j);

        if (ev->verbose >= EV_VERBOSE_ULTRA)
          EV_THREAD_SAVE_NEW_LINE;
      }
    }

    /**
     * after recobination of the current part of the current
     * generation is done wait till the next generation starts
     */
    evt->waiting = 1;

  }

  return NULL;
}

/**
 * Thread function to do mutation onely
 * if number of survivors == number of deaths
 */
static void *threadable_mutation_onely_1half(void *arg) {

  EvThreadArgs *evt = arg;
  Evolution *ev     = evt->ev;
  int j;
  
  /**
   * during the actual evolution we keep threads running
   * and waiting till thre is next next work
   */
  while (ev->threads_running) {

    /* wait untill there is new work todo */
    while (evt->waiting) {
      if (!ev->threads_running) return NULL;
      sched_yield();
    }

    /* reset threadwide iprooves */
    evt->improovs = 0;
 
    /**
     * loop untill all mutations of this thread are done
     */
    for (j = evt->start; j < evt->end; j++) {
 
      /**
       * clone the current individual (from the survivors)
       * and override an individual in the deaths-part
       */
      ev->clone_iv(ev->population[j]->iv, 
                   ev->population[j - ev->overall_start]->iv, 
                   evt->opt);
 
      /* muttate the cloned individual */
      ev->mutate(ev->population[j], 
                 evt->opt);
 
      /* calculate the fittnes for the new individual */
      EV_CALC_FITNESS_AT(*ev, j, evt->opt);
      
      /**
       * store if the new individual is better as the old one
       */
      if (ev->sort_max) {
        if (EV_FITNESS_AT(*ev, j) > 
            EV_FITNESS_AT(*ev, j - ev->overall_start)) {
 
          evt->improovs++;
        }
 
      } else {
        if (EV_FITNESS_AT(*ev, j) <
            EV_FITNESS_AT(*ev, j - ev->overall_start)) {
 
          evt->improovs++;
        }
      }
      
      /**
       * print status informations if wanted
       */
      if (ev->verbose >= EV_VERBOSE_ONELINE) {
        EV_IV_STATUS_OUTPUT(*ev, j);
 
        if (ev->verbose >= EV_VERBOSE_ULTRA)
          EV_THREAD_SAVE_NEW_LINE;
      }
    }

    /**
     * after mutation of the current part of the current
     * generation is done wait till the next generation starts
     */
    evt->waiting = 1;
  }

  return NULL;
}

/**
 * Thread function to do mutation onely
 * if numbers of survivors != number of deaths
 */
static void *threadable_mutation_onely_rand(void *arg) {

  EvThreadArgs *evt = arg;
  Evolution *ev = evt->ev;
  int j, rand1;

  /**
   * during the actual evolution we keep threads running
   * and waiting till thre is next next work
   */
  while (ev->threads_running) {

    /* wait untill there is new work todo */
    while (evt->waiting) {
      if (!ev->threads_running) return NULL;
      sched_yield();
    }


    /* reset threadwide iprooves */
    evt->improovs = 0;
 
    /**
     * loop untill all mutations are done
     */
    for (j = evt->start; j < evt->end; j++) {
 
      /**
       * clone random individual (from the survivors)
       * and override the current individual in the deaths-part
       */
      rand1 = rand() % ev->overall_start;
      ev->clone_iv(ev->population[j]->iv, 
                   ev->population[rand1]->iv, 
                   evt->opt);
 
      /* muttate the cloned individual */
      ev->mutate(ev->population[j], evt->opt);
 
      /* calculate the fittnes for the new individual */
      ev->population[j]->fitness = ev->fitness(ev->population[j], 
                                               evt->opt);
     
      /**
       * store if the new individual is better as the old one
       */
      if (ev->sort_max) {
        if (ev->population[j]->fitness > ev->population[rand1]->fitness) {
 
          evt->improovs++;
        }
 
      } else {
        if (ev->population[j]->fitness < ev->population[rand1]->fitness) {
 
          evt->improovs++;
        }
      }
     
      /**
       * print status informations if wanted
       */
      if (ev->verbose >= EV_VERBOSE_ONELINE) {
        EV_IV_STATUS_OUTPUT(*ev, j);
 
        if (ev->verbose >= EV_VERBOSE_ULTRA)
          EV_THREAD_SAVE_NEW_LINE;
      }
    }

    /**
     * after mutation of the current part of the current
     * generation is done wait till the next generation starts
     */
    evt->waiting = 1;
  }

  return NULL;
}

/**
 * returns the Size an Evolution with the given args will have
 */
uint64_t ev_size(int population_size, 
                 int num_threads, 
                 int keep_last_generation, 
                 uint64_t sizeof_iv, 
                 uint64_t sizeof_opt) {
  
  uint64_t mul = keep_last_generation ? 1 : 2;

  uint64_t size = (uint64_t) sizeof(Evolution);
  size += (uint64_t) sizeof(EvThreadArgs) * num_threads;
  size += (uint64_t) sizeof(TClient)      * num_threads;
  size += sizeof_opt * num_threads;
  size += (uint64_t) sizeof(Individual *) * population_size * mul;
  size += (uint64_t) sizeof(Individual)   * population_size * mul;
  size += sizeof_iv  * population_size    * mul;

  return size;
}

#endif // end of EVOLUTION
