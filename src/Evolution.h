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
#define EV_SORT_MAX               32
#define EV_SORT_MIN               0

/**
 * Shorter Flags
 */
#define EV_UREC EV_USE_RECOMBINATION
#define EV_UMUT EV_USE_MUTATION
#define EV_AMUT EV_ALWAYS_MUTATE
#define EV_KEEP EV_KEEP_LAST_GENERATION
#define EV_ABRT EV_USE_ABORT_REQUIREMENT
#define EV_SMIN EV_SORT_MIN
#define EV_SMAX EV_SORT_MAX

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
 * smal fitness best
 */
#define EVOLUTION_SELECTION_MIN(EVOLUTION) \
  chiefsort_min(EVOLUTION->population, EVOLUTION->population_size, EV_MIN_QUICKSORT);

/**
 * Sorts the best Individual at top of the population array
 * biggest fitness best
 */
#define EVOLUTION_SELECTION_MAX(EVOLUTION) \
  chiefsort_max(EVOLUTION->population, EVOLUTION->population_size, EV_MIN_QUICKSORT);

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
 *
 * NOTE each function get an additional void pointer to with is used for specific args
 */
typedef struct Evolution Evolution;
struct Evolution {
  void *(*init_individual) (void *);        /* function to initialzes an Individual */
  void (*clone_individual) (void *,         /* function clones an individual */
                             void *,
                              void *);      /* first Individual is destination second is source */
  void (*free_individual) (void *, 
                            void *);        /* function to free (kill) an Individual */
  Individual **population;                  /* Population of Individuals only pointers for faster sorting */
  Individual *individuals;                  /* The Individuals */
  int population_size;                      /* Population size */
  void (*recombinate) (Individual *,        
           Individual *, Individual *,
             void *);                       /* recombination function(src1, src2, dst) */
  void (*mutate) (Individual *, void *);    /* mutation function */
  int (*fitness) (Individual *, void *);    /* fittnes function */
  char use_recmbination;                    /* indicates wether to use recombination or not */
  char use_muttation;                       /* indicates wether to use mutation or not */
  char always_mutate;                       /* indicates wheter to always mutate or use probability */
  char keep_last_generation;                /* indicates wheter to disgard last generation or not */
  double mutation_propability;              /* the probability of an Individual to mutate */
  double death_percentage;                  /* the percent of Individual that die during an generation change */
  char (*continue_ev) (Individual *, 
                        void *);            /* abort requirement function should return 0 to abort 1 to continue */
  char use_abort_requirement;               /* if not true calculation will go on until generation limit es reatched */
  int generation_limit;                     /* maximum of generations to calculate (even if abort requirement is used) */
  char sort_max;                            /* if the individuals should be sorted by max or min fittnes */
  void *opts;                               /* pointer to aditional opts which is given to each function */
};

// functions
Evolution *new_evolution(void *(*init_individual) (void *), void (*clone_individual) (void *, void *, void *),
                          void (*free_individual) (void *, void *), void (*mutate) (Individual *, void *),
                            int (*fitness) (Individual *, void *), void (*recombinate) (Individual *,
                              Individual *, Individual *, void *), char (*continue_ev) (Individual *, void *),
                                int population_size, int generations_limit, double mutation_propability,
                                  double death_percentage, void *opts, char flags);
Individual *evolute(Evolution *ev);
void evolution_clean_up(Evolution *ev);
Individual best_evolution(void *(*init_individual) (void *), void (*clone_individual) (void *, void *, void *),
                          void (*free_individual) (void *, void *), void (*mutate) (Individual *, void *),
                            int (*fitness) (Individual *, void *), void (*recombinate) (Individual *,
                              Individual *, Individual *, void *), char (*continue_ev) (Individual *, void *),
                                int population_size, int generations_limit, double mutation_propability,
                                  double death_percentage, void *opts, char flags);


#endif // end of EVOLUTION_HEADER
