#ifndef EVOLUTION
#define EVOLUTION
#include "Evolution.h"

/**
 * Returns pointer to an new and initialzed Evolution
 * take pointers for varius functions:
 *
 * opts can be used for aditional function wide values
 *
 * +----------------------------------------------+------------------------------------------------------+
 * function pointer                               | describtion                                          |
 * +----------------------------------------------+------------------------------------------------------+
 * | void *init_individual(void *opts)            | should return an void pointer to an new initialized  |
 * |                                              | individual                                           |
 * |                                              |                                                      |
 * | void clone_individual(void *dst, void *src,  | takes two void pointer to individuals and should     |
 * |                        void *opts)           | clone the content of the second one                  |
 * |                                              | into the first one                                   |
 * |                                              |                                                      |
 * | void free_individual(void *src, void *opts)  | takes an void pointer to individual and should free  |
 * |                                              | the spaces allocated by the given individual         |
 * |                                              |                                                      |
 * | void mutate(Individual *src, void *opts)     | takes an void pointer to an individual and should    |
 * |                                              | change it in a way that the probability to           |
 * |                                              | improove it is around 1/5                            |
 * |                                              |                                                      |
 * | int fitness(Individual *src, void *opts)     | takes an void pointer to an individual, and should   |
 * |                                              | return an integer value that indicates how strong    |
 * |                                              | (good / improoved / near by an optimal solution) it  |
 * |                                              | is. Control the sorting order with the sort flags    |
 * |                                              |                                                      |
 * | void recombinate(Individual *src1,           | takes three void pointer to individuals and should   |
 * |                   Individual *src2,          | combinate the first two one, and should save the     |
 * |                    Individual *dst,          | result in the thired one. As mutate the probability  |
 * |                          void *opts)         | to get an improoved individuals should be around 1/5 |
 * |                                              |                                                      |
 * | char continue_ev(Individual *ivs,            | takes an pointer to the current Individuals and      |
 * |                           void *opts)        | should return 0 if the calculaten should stop and    |
 * |                                              | 1 if the calculaten should continue                  |
 * +----------------------------------------------+------------------------------------------------------+
 *
 * Note: the void pointer to individuals are not pointer to an Individual struct, theu are the individual element
 *        of the Individual struct
 * flags can be:
 *
 *    EV_UREC / EV_USE_RECOMBINATION
 *    EV_UMUT / EV_USE_MUTATION
 *    EV_AMUT / EV_ALWAYS_MUTATE
 *    EV_KEEP / EV_KEEP_LAST_GENERATION
 *    EV_ABRT / EV_USE_ABORT_REQUIREMENT
 *    EV_SMAX / EV_SORT_MAX
 *    EV_SMIN / EV_SORT_MIN
 *
 * The floowing flag combinations are possible:
 *
 * +-------------------------------------------+---------------------------------------------------------+
 * | Flag combination                          | descrion                                                |
 * +-------------------------------------------+---------------------------------------------------------+
 * | EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP|EV_ABRT   | Recombinate and always mutate individual,               |
 * |                                           | keep last generation, and use abort requirement         |
 * |                                           | function                                                |
 * |                                           |                                                         |
 * | EV_UMUT|EV_AMUT|EV_KEEP|EV_ABRT           | Onley mutate individual keep last generation and use    |
 * |                                           | abort requirement function                              |
 * |                                           |                                                         |
 * | EV_UREC|EV_UMUT|EV_KEEP|EV_ABRT           | Recombinate and maybe mutate individual (depending on   |
 * |                                           | a given probability) keep last generation and use       |
 * |                                           | abort requirement function                              |
 * |                                           |                                                         |
 * | EV_UREC|EV_KEEP|EV_ABRT                   | Recombinate individuals, keep last generation and use   |
 * |                                           | abort requirement function)                             |
 * |                                           |                                                         |
 * | EV_UREC|EV_UMUT|EV_AMUT|EV_ABRT           | Recombinate and always mutate individuals, disgard      |
 * |                                           | last generation. and use abort requirement function     |
 * |                                           |                                                         |
 * | EV_UMUT|EV_AMUT|EV_ABRT                   | Onely mutate individuals, disgard last generation,      |
 * |                                           | and use abort requirement function                      |
 * |                                           |                                                         |
 * | EV_UREC|EV_UMUT|EV_ABRT                   | Recombinate and maybe mutate individuals (depending     |
 * |                                           | on a given probability), disgard last generation and    |
 * |                                           | use abort requirement function                          |
 * |                                           |                                                         |
 * | EV_UREC|EV_ABRT                           | Recombinate individuals, disgard last generation and    |
 * |                                           | use abort requirement function                          |
 * |                                           |                                                         |
 * | EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP           | Recombinate and always mutate individuals, keep last    |
 * |                                           | generation, and calc until generation limit is          |
 * |                                           | reatched                                                |
 * |                                           |                                                         |
 * | EV_UMUT|EV_AMUT|EV_KEEP                   | Onely mutate individuals, keep last generation and      |
 * |                                           | calc until generation limit is reatched                 |
 * |                                           |                                                         |
 * | EV_UREC|EV_UMUT|EV_KEEP                   | Recombinate and maybe mutate individuals, keep last     |
 * |                                           | generation and calc until generation limit is           |
 * |                                           | reatched                                                |
 * |                                           |                                                         |
 * | EV_UREC|EV_KEEP                           | Recombinate individuals, keep last generation and       |
 * |                                           | calc until generation limit is reatched                 |
 * |                                           |                                                         |
 * | EV_UREC|EV_UMUT|EV_AMUT                   | Recombinate and always mutate individuals, disgard last |
 * |                                           | generation and calc until generation imit is reatched   |
 * |                                           |                                                         |
 * | EV_UMUT|EV_AMUT                           | Recombinate and always mutate Individuals disgard orld  |
 * |                                           | generation and calc until generation limit is reatched  |
 * |                                           |                                                         |
 * | EV_UREC|EV_UMUT                           | Recombinate and maybe mutate Individual (depending on a |
 * |                                           | given probability) disgard old generation and calc      |
 * |                                           | until generation limit is reatched                      |
 * |                                           |                                                         |
 * | EV_UREC                                   | Recombinate Individual onley disgard old generation and |
 * |                                           | calc until generation limit is reatched                 |
 * +-------------------------------------------+---------------------------------------------------------+
 *
 * to all of the above combination an EV_SMIN / EV_SMAX can be added
 * standart is EV_SMIN
 * Also an verbosytiy level of EV_VERBOSE_QUIET (EV_VEB0), EV_VERBOSE_ONELINE (EV_VEB1)
 * and EV_VERBOSE_HIGH (EV_VEB2) can be added, standart is EV_VERBOSE_QUIET
 */
Evolution *new_evolution(void *(*init_individual) (void *), void (*clone_individual) (void *, void *, void *),
                          void (*free_individual) (void *, void *), void (*mutate) (Individual *, void *),
                            int (*fitness) (Individual *, void *), void (*recombinate) (Individual *,
                              Individual *, Individual *, void *), char (*continue_ev) (Individual *, void *),
                                int population_size, int generation_limit, double mutation_propability,
                                  double death_percentage, void *opts, short flags) {

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

  ev->init_individual       = init_individual;
  ev->clone_individual      = clone_individual;
  ev->free_individual       = free_individual;
  ev->mutate                = mutate;
  ev->fitness               = fitness;
  ev->recombinate           = recombinate;
  ev->continue_ev           = continue_ev;
  ev->population_size       = population_size;
  ev->generation_limit      = generation_limit;
  ev->mutation_propability  = mutation_propability;
  ev->death_percentage      = death_percentage;
  ev->opts                  = opts;
  ev->use_recmbination      = flags & EV_USE_RECOMBINATION;
  ev->use_muttation         = flags & EV_USE_MUTATION;
  ev->always_mutate         = flags & EV_ALWAYS_MUTATE;
  ev->keep_last_generation  = flags & EV_KEEP_LAST_GENERATION;
  ev->use_abort_requirement = flags & EV_USE_ABORT_REQUIREMENT;
  ev->sort_max              = flags & EV_SORT_MAX;
  ev->verbose               = flags & (EV_VERBOSE_ONELINE | EV_VERBOSE_HIGH);

  // multiplicator if we should discard the last generation, we can't recombinate in place
  int mul = ev->keep_last_generation ? 1 : 2;

  ev->population  = (Individual **) malloc(sizeof(Individual *) * population_size * mul);
  ev->individuals = (Individual *) malloc(sizeof(Individual) * population_size * mul);

  int i;
  for (i = 0; i < population_size * mul; i++) {
    ev->population[i] = ev->individuals + i;
    ev->individuals[i].individual = init_individual(ev->opts);
    ev->population[i]->fitness = ev->fitness(ev->population[i], ev->opts);

    if (ev->verbose >= EV_VERBOSE_ONELINE)
      printf("init population: %10d\r", population_size * mul - i);
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

  /**
   * Generation loop
   * each loop lets one generation grow kills the worst individuals
   * and let new individuals born
   */
  int i, j, rand1, rand2, start, end;
  for (i = 0; i < ev->generation_limit && (!ev->use_abort_requirement
                || (ev->use_abort_requirement && ev->continue_ev(ev->individuals, ev->opts))); i++) {
    
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
        ev->recombinate(ev->population[rand1], ev->population[rand2], ev->population[j], ev->opts);
        
        // mutate Individuals
        if (ev->use_muttation) {
          if (ev->always_mutate)
            ev->mutate(ev->population[j], ev->opts);
          else {
            rand1 = rand() % EVOLUTION_MUTATE_ACCURACY;
            if (rand1 <= mutate)
              ev->mutate(ev->population[j], ev->opts);
          }
        }

        // calculate the fittnes for the new individual
        ev->population[j]->fitness = ev->fitness(ev->population[j], ev->opts);

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
      }
    // copy and mutate individuals
    } else {
      
      // if deaths == survives, make sure that all survivers are being copyed and mutated
      if (start * 2 == end) {
        for (j = 0; j < start; j++) {
          ev->clone_individual(ev->population[j + start]->individual, ev->population[j]->individual, ev->opts);
          ev->mutate(ev->population[j + start], ev->opts);

          // calculate the fittnes for the new individual
          ev->population[j + start]->fitness = ev->fitness(ev->population[j + start], ev->opts);
         
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
        }

      // else choose random survivers to mutate
      } else {
        for (j = start; j < end; j ++) {
          rand1 = rand() % start;
          ev->clone_individual(ev->population[j]->individual, ev->population[rand1]->individual, ev->opts);
          ev->mutate(ev->population[j], ev->opts);

          // calculate the fittnes for the new individual
          ev->population[j]->fitness = ev->fitness(ev->population[j], ev->opts);
         
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

    if (ev->verbose >= EV_VERBOSE_HIGH) {
      sprintf(last_improovs_str,"%.5f", (info.improovs / (double) deaths) * 100.0);
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
    ev->free_individual(ev->population[i]->individual, ev->opts);
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


#endif // end of EVOLUTION
