/**
 * Header of an Evolution Algorithm
 *
 * to solf an optimation Problem
 */
#ifndef EVOLUTION
#define EVOLUTION
#include <pthread.h>

/**
 * Some Makros
 */
#define EVOLUTION_TRUE  1
#define EVOLUTION_FALSE 0
#define EVOLUTION_MUTATE_ACCURACY 10000

/**
 * Macros for Sorting the Population by Fitness
 */
#define CHIEFSORT_TYPE MazeIndividual *
#define CHIEFSORT_BIGGER(X, Y) X->fitness > Y->fitness
#define CHIEFSORT_SMALER(X, Y) X->fitness < Y->fitness
#define CHIEFSORT_EQL(X, Y) X->fitness == Y->fitness
#define EV_MIN_QUICKSORT 20
#include "sort.c"

/**
 * Flags for the new_evolution function
 */
#define EV_USE_RECOMBINATION      1
#define EV_USE_MUTATION           2
#define EV_ALWAYS_MUTATE          4
#define EV_KEEP_LAST_GENERATION   8
#define EV_USE_ABORT_REQUIREMENT  16

/**
 * Shorter Flags
 */
#define EV_UREC EV_USE_RECOMBINATION
#define EV_UMUT EV_USE_MUTATION
#define EV_AMUT EV_ALWAYS_MUTATE
#define EV_KEEP EV_KEEP_LAST_GENERATION
#define EV_ABRT EV_USE_ABORT_REQUIREMENT

/**
 * Sorts the best Individual at top of the population array
 */
#define EVOLUTION_SELECTION(EVOLUTION) \
  chiefsort(EVOLUTION->population, EVOLUTION->population_size, EV_MIN_QUICKSORT);

/**
 * An Individual wich has an definied fitness
 * as higher as longer it lives
 */
typedef struct {
  void *individual;         /* void pointer to the Individual */
  long fitness;             /* the fitness of this Individual */
} Individual;

/**
 * Structur holding the Individual Population.
 * It also holds an recombination, mutation and fitness function
 * So as an Mutation probability
 *
 * fitness function takes an Individual and should return the strength (fitness) of it
 * mutation function takes an Individual and should mutate it in an specific way
 * recombination function takes three Individuals frist and second should be cobinated
 * and saved in the thierd Individual (recombination function must not initialize the thierd Individual)
 *
 * the init Individual function should return an pointer to an initialized individual
 * (void pointer individual of Individual struct)
 *
 * the new born individuals are 1 - death_percentage, what means the Population keeps their size
 */
typedef struct Evolution Evolution;
struct Evolution {
  void *(*init_individual) ();              /* function to initialzes an Individual */
  void (*clone_individual) (void *,         /* function clones an individual */
                             void *);       /* first Individual is destination second is source */
  void (*free_individual) (void *);         /* function to free (kill) an Individual */
  Individual **population;                  /* Population of Individuals only pointers for faster sorting */
  Individual *individuals;                  /* The Individuals */
  int population_size;                      /* Population size */
  void (*recombinate) (Individual *,        
           Individual *, Individual *);     /* recombination function(src1, src2, dst) */
  void (*mutate) (Individual *);            /* mutation function */
  int (*fitness) (Individual *);            /* fittnes function */
  char use_recmbination;                    /* indicates wether to use recombination or not */
  char use_muttation;                       /* indicates wether to use mutation or not */
  char always_mutate;                       /* indicates wheter to always mutate or use probability */
  char keep_last_generation;                /* indicates wheter to disgard last generation or not */
  double mutation_propability;              /* the probability of an Individual to mutate */
  double death_percentage;                  /* the percent of Individual that die during an generation change */
  char (*continue_ev) (Individual *)        /* abort requirement function should return 0 to abort 1 to continue */
  char use_abort_requirement                /* if not true calculation will go on until generation limit es reatched */
  int generation_limit                      /* maximum of generations to calculate (even if abort requirement is used) */
};

// functions
Evolution *new_evolution(void *(*init_individual) (), void (*clone_individual) (void *, void *),
                          void (*free_individual) (void *), void (*mutate) (Individual *),
                            int (*fitness) (Individual *), void (*recombinate) (Individual *,
                              Individual *, Individual *), char (*continue_ev) (Individual *),
                                int population_size, int generations_limit, double mutation_propability,
                                  double death_percentage, char flags);
Individual *evolute(Evolution *ev);
void evolution_clean_up(Evolution *ev);
Individual *best_evolution(void *(*init_individual) (), void (*clone_individual) (void *, void *),
                          void (*free_individual) (void *), void (*mutate) (Individual *),
                            int (*fitness) (Individual *), void (*recombinate) (Individual *,
                              Individual *, Individual *), char (*continue_ev) (Individual *),
                                int population_size, int generations_limit, double mutation_propability,
                                  double death_percentage, char flags);


/**
 * Returns pointer to an new and initialzed Evolution
 * take pointers for varius functions:
 *
 * +----------------------------------------------+------------------------------------------------------+
 * function pointer                               | describtion                                          |
 * +----------------------------------------------+------------------------------------------------------+
 * | void *init_individual()                      | should return an void pointer to an new initialized  |
 * |                                              | individual                                           |
 * |                                              |                                                      |
 * | void clone_individual(void *dst, void *src)  | takes two void pointer to individuals and should     |
 * |                                              | clone the content of the second one                  |
 * |                                              | into the first one                                   |
 * |                                              |                                                      |
 * | void free_individual(void *src)              | takes an void pointer to individual and should free  |
 * |                                              | the spaces allocated by the given individual         |
 * |                                              |                                                      |
 * | void mutate(void *src)                       | takes an void pointer to an individual and should    |
 * |                                              | change it in a way that the probability to           |
 * |                                              | improove it is around 1/5                            |
 * |                                              |                                                      |
 * | int fitness(void *src)                       | takes an void pointer to an individual, and should   |
 * |                                              | return an integer value that indicates how strong    |
 * |                                              | (good / improoved / near by an optimal solution) it  |
 * |                                              | is, as higher as better                              |
 * |                                              |                                                      |
 * | void recombinate(void *src1, void *src2,     | takes three void pointer to individuals and should   |
 * |                                   void *dst) | combinate the first two one, and should save the     |
 * |                                              | result in the thired one. As mutate the probability  |
 * |                                              | to get an improoved individuals should be around 1/5 |
 * |                                              |                                                      |
 * | char continue_ev(Individual *ivs)            | takes an pointer to the current Individuals and      |
 * |                                              | should return 0 if the calculaten should stop and    |
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
 */
Evolution *new_evolution(void *(*init_individual) (), void (*clone_individual) (void *, void *),
                          void (*free_individual) (void *), void (*mutate) (Individual *),
                            int (*fitness) (Individual *), void (*recombinate) (Individual *,
                              Individual *, Individual *), char (*continue_ev) (Individual *),
                                int population_size, int generations_limit, double mutation_propability,
                                  double death_percentage, char flags) {

  // valid flag combination ?
  if (flags != EV_UREC || flags != EV_UREC|EV_UMUT || flags != EV_UREC|EV_UMUT|EV_AMUT
       || flags != EV_UREC|EV_KEEP || flags != EV_UREC|EV_UMUT|EV_KEEP 
        || flags != EV_UMUT|EV_AMUT|EV_KEEP || flags != EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP
         || flags != EV_UREC|EV_ABRT || flags != EV_UREC|EV_UMUT|EV_ABRT 
          || flags != EV_UMUT|EV_AMUT|EV_ABRT || flags != EV_UREC|EV_UMUT|EV_AMUT|EV_ABRT
           || flags != EV_UREC|EV_KEEP|EV_ABRT || flags != EV_UREC|EV_UMUT|EV_KEEP|EV_ABRT
            || flags != EV_UMUT|EV_AMUT|EV_KEEP|EV_ABRT 
              || flags != EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP|EV_ABRT) {

    return NULL;
  }

  // valid other opts
  if (population_size <= 0 || generations_limit <= 0 || mutation_propability < 0.0 
        || mutation_propability > 1.0 || death_percentage < 0.0 || death_percentage > 1.0)
    return NULL;
  
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
  ev->generations_limit     = generations_limit;
  ev->mutation_propability  = mutation_propability;
  ev->death_percentage      = death_percentage;
  ev->use_recmbination      = flags & EV_USE_RECOMBINATION;
  ev->use_muttation         = flags & EV_USE_MUTATION;
  ev->always_mutate         = flags & EV_ALWAYS_MUTATE;
  ev->keep_last_generation  = flags & EV_KEEP_LAST_GENERATION;
  ev->use_abort_requirement = flags & EV_USE_ABORT_REQUIREMENT;

  // multiplicator if we should discard the last generation, we can't recombinate in place
  mul = ev->keep_last_generation ? 1 : 2;

  ev->population  = (Individual **) malloc(sizeof(Individual *) * population_size * mul);
  ev->individuals = (Individual *) malloc(sizeof(Individual) * population_size * mul);

  int i;
  for (i = 0; i < population_size * mul; i++) {
    ev->population[i] = ev->individuals + i;
    ev->individuals[i].individual = init_individual();
    ev->individuals[i].fitness    = 0;
  }

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
  Individual *tmp_iv;

  /**
   * Generation loop
   * each loop lets one generation grow kills the worst individuals
   * and let new individuals born
   */
  int i, j, rand1, rand2, start;
  for (i = 0; i < ev->generations_limit && (!ev->use_abort_requirement
                || (ev->use_abort_requirement && ev->abort_requirement(ev->individuals))); i++) {

    // calculates the fitness for each Individual
    for (j = 0; j < ev->population_size; j++)
      ev->fitness(ev->population[j]);
  
    /**
     * Select the best individuals to survive,
     * Sort the Individuals by theur fittnes
     */
    EVOLUTION_SELECTION(ev)

    /**
     * If we keep the last generation, we can recombinate in place
     * else wen wirst calculate an new populion
     */
    start = ev->keep_last_generation ? survive : ev->population_size;
    end   = ev->keep_last_generation ? ev->population_size : ev->population_size * 2; 


    if (ev->use_recmbination) {
      for (j = start; j < end; j++) {

        // from 2 Individuals of the untouched part we calculate an new one
        rand2 = rand1 = rand() % start;
        while (rand1 == rand2) rand2 = rand() % start;
        
        // recombinate individuals
        ev->recombinate(ev-population[rand1], ev->population[rand2], ev->population[j]);
        
        // mutate Individuals
        if (ev->use_muttation) {
          if (ev->always_mutate)
            ev->mutate(ev->population[j]);
          else {
            rand1 = rand() % EVOLUTION_MUTATE_ACCURACY;
            if (rand1 <= mutate)
              ev->mutate(ev->population[j]);
          }
        }
      }
    // copy and mutate individuals
    } else {
      
      // if deaths == survives, make sure that all survivers are being copyed and mutated
      if (start * 2 == end) {
        for (j = 0; j < start; j++) {
          ev->clone_individual(ev->population[j + start]->individual, population[j]->individual);
          ev->mutate(ev->population[j + start]);
        }

      // else choose random survivers to mutate
      } else {
        for (j = start; j < end; j ++) {
          rand1 = rand() % start;
          ev->clone_individual(ev->population[j]->individual, population[rand1]->individual);
          ev->mutate(ev->population[j]);
        }
      }
    }
  

    if (!ev->keep_last_generation) {
      
      /**
       * sitch old and new generation to discard the old one
       * which will be overidden next time
       */
      for (j = 0; j < ev->population_size; j++) {
        tmp_iv = ev->population[j];
        ev->population[j] = ev->population[ev->population_size + j];
        ev->population[ev->population_size + j] = tmp_iv;
      }
    }
  }


  // calculates the fitness for each Individual of the last Generation
  for (j = 0; j < ev->population_size; j++)
    ev->fitness(ev->population[j]);
  
  // Sort the Individuals by their fittnes
  EVOLUTION_SELECTION(ev)

  // return the best
  return ev->population[0];
}

/**
 * Frees unneded resauces after an evolution calculation
 */
void evolution_clean_up(Evolution *ev) {
  int end   = ev->keep_last_generation ? ev->population_size : ev->population_size * 2; 
  int i;
  for (i = 1; i < end; i++) {
    ev->free_individual(ev->population[i]->individual);
  }
  free(ev->individuals);
  free(ev->population);
}

/**
 * Computes an evolution for the given params
 * and returns the best Individual
 */
Individual *best_evolution(void *(*init_individual) (), void (*clone_individual) (void *, void *),
                          void (*free_individual) (void *), void (*mutate) (Individual *),
                            int (*fitness) (Individual *), void (*recombinate) (Individual *,
                              Individual *, Individual *), char (*continue_ev) (Individual *),
                                int population_size, int generations_limit, double mutation_propability,
                                  double death_percentage, char flags) {

  Evolution *ev = new_evolution(init_individual, clone_individual, free_individual, mutate,
                            fitness, recombinate, continue_ev, population_size, generations_limit, 
                              mutation_propability, death_percentage, flags);

  Individual best = *evolute(ev);
  evolution_clean_up(ev);

  return best;
}

