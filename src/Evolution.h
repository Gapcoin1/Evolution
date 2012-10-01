/**
 * Header of an Evolution Algorithm
 *
 * to solf an optimation Problem
 */
#ifndef EVOLUTION_HEADER
#define EVOLUTION_HEADER
#include <pthread.h>

/**
 * Some Makros
 */
#define EVOLUTION_TRUE  1
#define EVOLUTION_FALSE 0
#define EVOLUTION_MUTATE_ACCURACY 10000

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
 * An Individual wich has an definied fitness
 * as higher as longer it lives
 */
typedef struct {
  void *individual;         /* void pointer to the Individual */
  long fitness;             /* the fitness of this Individual */
} Individual;

/**
 * Macros for Sorting the Population by Fitness
 */
#define CHIEFSORT_TYPE Individual *
#define CHIEFSORT_BIGGER(X, Y) X->fitness > Y->fitness
#define CHIEFSORT_SMALER(X, Y) X->fitness < Y->fitness
#define CHIEFSORT_EQL(X, Y) X->fitness == Y->fitness
#define EV_MIN_QUICKSORT 20
#include "sort.c"

/**
 * Sorts the best Individual at top of the population array
 */
#define EVOLUTION_SELECTION(EVOLUTION) \
  chiefsort(EVOLUTION->population, EVOLUTION->population_size, EV_MIN_QUICKSORT);

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
  char (*continue_ev) (Individual *);       /* abort requirement function should return 0 to abort 1 to continue */
  char use_abort_requirement;               /* if not true calculation will go on until generation limit es reatched */
  int generation_limit;                     /* maximum of generations to calculate (even if abort requirement is used) */
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
Individual best_evolution(void *(*init_individual) (), void (*clone_individual) (void *, void *),
                          void (*free_individual) (void *), void (*mutate) (Individual *),
                            int (*fitness) (Individual *), void (*recombinate) (Individual *,
                              Individual *, Individual *), char (*continue_ev) (Individual *),
                                int population_size, int generations_limit, double mutation_propability,
                                  double death_percentage, char flags);


#endif // end of EVOLUTION_HEADER
