/**
 * Header of an Evolution Algorithm
 *
 * to solf an optimation Problem
 */
#ifndef EVOLUTION_HEADER
#define EVOLUTION_HEADER
#include <pthread.h>
#include "C-Utils/Sort/src/sort.h"

/**
 * structur for aditional information during evolution
 */
typedef struct {
 int improovs; 
} EvolutionInfo;

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
#define EV_VERBOSE_QUIET          0
#define EV_VERBOSE_ONELINE        256
#define EV_VERBOSE_HIGH           512
#define EV_VERBOSE_ULTRA          768

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
#define EV_VEB0 EV_VERBOSE_QUIET
#define EV_VEB1 EV_VERBOSE_ONELINE
#define EV_VEB2 EV_VERBOSE_HIGH
#define EV_VEB3 EV_VERBOSE_ULTRA

/**
 * An Individual wich has an definied fitness
 * as higher as longer it lives
 */
typedef struct {
  void *individual;         /* void pointer to the Individual */
  long fitness;             /* the fitness of this Individual */
} Individual;

/**
 * Functions for Sorting the Population by Fitness
 * void pointer version 
 */
char void_ptr_bigger(void *a, void *b);
char void_ptr_smaler(void *a, void *b);
char void_ptr_equal(void *a, void *b);

/**
 * Struct holding information for threads
 */
typedef struct Evolution Evolution;
typedef struct {
  Evolution *ev;
  int index;
} EvolutionThread;

/**
 * The Evolution struct
 *
 * for detailed information about the function-pointers 
 * have a lock at the init function
 */
struct Evolution {
  Individual **population;                  /* Population of Individuals     */
                                            /* only pointers for faster      */
                                            /* sorting                       */
  Individual *individuals;                  /* The Individuals               */
  void   *(*init_individual) (void *);      /* function to initialzes        */
                                            /* an Individual                 */
  void   (*clone_individual) (void *,       /* function clones an individual */
                              void *,       /* first Individual is           */
                              void *);      /* destination second is source  */
  void   (*free_individual) (void *,        /* function to free (kill) an    */
                             void *);       /* Individual                    */  
  int    population_size;                   /* Population size               */
  void   (*recombinate) (Individual *,      /* recombination function(src1,  */
                         Individual *,      /*                        src2,  */
                         Individual *,      /*                        dst)   */  
                         void *);           /*                               */
  void   (*mutate) (Individual *, void *);  /* mutation function             */
  long   (*fitness) (Individual *, void *); /* fittnes function */
  char   use_recmbination;                  /* indicates wether to use recombination or not */
  char   use_muttation;                     /* indicates wether to use mutation or not */
  char   always_mutate;                     /* indicates wheter to always mutate or use probability */
  char   keep_last_generation;              /* indicates wheter to disgard last generation or not */
  double mutation_propability;              /* the probability of an Individual to mutate */
  double death_percentage;                  /* the percent of Individual that die during an generation change */
  char   (*continue_ev) (Individual *, 
                         void *);            /* abort requirement function should return 0 to abort 1 to continue */
  char   use_abort_requirement;               /* if not true calculation will go on until generation limit es reatched */
  int    generation_limit;                     /* maximum of generations to calculate (even if abort requirement is used) */
  char   sort_max;                            /* if the individuals should be sorted by max or min fittnes */
  void   **opts;                              /* pointer to aditional opts which is given to each function */
  short  verbose;                            /* the verbosity level */
  int     min_quicksort;                     /* min array length to change    *
                                             * sorting from qick- to         *
                                             * insertionsort                 */            
  struct {
    int i, start, end, num_threads, 
        mutate, generations_progressed;
    int n_threads_sort_parallel;       /* number of threads so that    *
                                             * parallel sort beats macro    *
                                             * based sorting                */
    pthread_t *threads;
    QISTA_t sort_args;                      /* args for parallel sorting */
    EvolutionThread *ev_threads;
    char last_improovs_str[25];
    EvolutionInfo info;
  } parallel;
};



// functions
Evolution *new_evolution(void *(*init_individual) (void *), void (*clone_individual) (void *, void *, void *),
                          void (*free_individual) (void *, void *), void (*mutate) (Individual *, void *),
                            long (*fitness) (Individual *, void *), void (*recombinate) (Individual *,
                              Individual *, Individual *, void *), char (*continue_ev) (Individual *, void *),
                                int population_size, int generations_limit, double mutation_propability,
                                  double death_percentage, void *opts, short flags);
Individual *evolute(Evolution *ev);
void evolution_clean_up(Evolution *ev);
Individual best_evolution(void *(*init_individual) (void *), void (*clone_individual) (void *, void *, void *),
                          void (*free_individual) (void *, void *), void (*mutate) (Individual *, void *),
                            long (*fitness) (Individual *, void *), void (*recombinate) (Individual *,
                              Individual *, Individual *, void *), char (*continue_ev) (Individual *, void *),
                                int population_size, int generations_limit, double mutation_propability,
                                  double death_percentage, void *opts, short flags);
u_int64_t ev_size(int population_size, int num_threads, int keep_last_generation, 
                                        u_int64_t sizeof_iv, u_int64_t sizeof_opt);
// parallel functions
Evolution *new_evolution_parallel(void *(*init_individual) (void *), void (*clone_individual) (void *, void *, void *),
                                   void (*free_individual) (void *, void *), void (*mutate) (Individual *, void *),
                                    long (*fitness) (Individual *, void *), void (*recombinate) (Individual *,
                                     Individual *, Individual *, void *), char (*continue_ev) (Individual *, void *),
                                      int population_size, int generation_limit, double mutation_propability,
                                       double death_percentage, void **opts, int num_threads, short flags);
Individual best_evolution_parallel(void *(*init_individual) (void *), void (*clone_individual) (void *, void *, void *),
                          void (*free_individual) (void *, void *), void (*mutate) (Individual *, void *),
                            long (*fitness) (Individual *, void *), void (*recombinate) (Individual *,
                              Individual *, Individual *, void *), char (*continue_ev) (Individual *, void *),
                                int population_size, int generations_limit, double mutation_propability,
                                  double death_percentage, void **opts, int num_threads, short flags);
void *threadable_init_individual(void *arg);
Individual *evolute_parallel(Evolution *ev);
void *threadable_recombinate(void *arg);
void *threadable_mutation_onely_1half(void *args);
void *threadable_mutation_onely_rand(void *args);


#endif // end of EVOLUTION_HEADER
