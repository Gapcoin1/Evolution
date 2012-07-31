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

/**
 * Macros for Sorting the Maze Population by Fitness
 */
#define CHIEFSORT_TYPE MazeIndividual *
#define CHIEFSORT_BIGGER(X, Y) X->fitness > Y->fitness
#define CHIEFSORT_SMALER(X, Y) X->fitness < Y->fitness
#define CHIEFSORT_EQL(X, Y) X->fitness == Y->fitness
#define MAZE_EV_MIN_QUICKSORT 20
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
  chiefsort(EVOLUTION->population, EVOLUTION->population_size, MAZE_EV_MIN_QUICKSORT);

/**
 * An Individual wich has an definied fitness
 * as higher as longer it lives
 */
typedef struct {
  void *individual;         /* void pointer to the Individual */
  long fitness;             /* the fitness of this Individual */
  void *mutate_args;        /* pointer to aditional args for the mutation function */
  void *fitness_args;       /* pointer to aditional args for the fitness function */
  void *recombination_args; /* pointer to aditional args for the recombination function */
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
  void *(*init_individual) ();             /* function to initialzes an Individual */
  void (*clone_individual) (void *,        /* function clones an individual */
                             void *);      /* first Individual is destination second is source */
  void (*free_individual) (void *);        /* function to free (kill) an Individual */
  Individual **population;                 /* Population of Individuals only pointers for faster sorting */
  Individual *individuals;                 /* The Individuals */
  int population_size;                     /* Population size */
  void (*recombinate) (Individual *,       
           Individual *, Individual *);    /* recombination function(src1, src2, dst) */
  void (*mutate) (Individual *);           /* mutation function */
  int (*fitness) (Individual *);           /* fittnes function */
  char use_recmbination;                   /* indicates wether to use recombination or not */
  char use_muttation;                      /* indicates wether to use mutation or not */
  char always_mutate;                      /* indicates wheter to always mutate or use probability */
  char keep_last_generation;               /* indicates wheter to disgard last generation or not */
  double mutation_propability;             /* the probability of an Individual to mutate */
  double death_percentage;                 /* the percent of Individual that die during an generation change */
  char (*abort_requirement) (Evolution *)  /* abort_requirement function should return 0 to abort 1 to continue */
  char use_abort_requirement               /* if not true calculation will go on until generation limit es reatched */
  int generation_limit                     /* maximum of generations to calculate */
};

Evolution *new_evolution(void *(*init_individual) (), void (*clone_individual) (void *, void *),
                          void (*free_individual) (void *), void (*mutate) (Individual *),
                            int (*fitness) (Individual *), void (*recombinate) (Individual *,
                              Individual *, Individual *), void (*abort_requirement) (Evolution *),
                                int population_size, int generations_limit, double mutation_propability,
                                  double death_percentage, char flags) {

  Evolution *ev = (Evolution *) malloc(sizeof(Evolution));

  ev->init_individual       = init_individual;
  ev->clone_individual      = clone_individual;
  ev->free_individual       = free_individual;
  ev->mutate                = mutate;
  ev->fitness               = fitness;
  ev->recombinate           = recombinate;
  ev->abort_requirement     = abort_requirement;
  ev->population_size       = population_size;
  ev->generations_limit     = generations_limit;
  ev->mutation_propability  = mutation_propability;
  ev->death_percentage      = death_percentage;
  ev->use_recmbination      = flags & EV_USE_RECOMBINATION;
  ev->use_muttation         = flags & EV_USE_MUTATION;
  ev->always_mutate         = flags & EV_ALWAYS_MUTATE;
  ev->keep_last_generation  = flags & EV_KEEP_LAST_GENERATION;
  ev->use_abort_requirement = flags & EV_USE_ABORT_REQUIREMENT;
}

/**
 * Thread structur for paralell mutation
 */
typedef struct {
  MazeEvolution *mazeev;       /* MazeEvolution struct */
  int start, end, half_size;    /* start and end index in the population array  and half size of population */
  pthread_t thread;             /* the used thread */
} ThreadMutation;

/**
 * Thread strcuture for paralell population initailation
 */
typedef struct {
  MazeEvolution *mazeev;
  int start, end;
  pthread_t thread;
  #ifdef MAZE_EVOLUTION_VERBOSE
    int *to_progress;
  #endif
} ThreadPopulationInit;

// Mutex
pthread_mutex_t mazeev_mutex = PTHREAD_MUTEX_INITIALIZER;


// functions
void init_mazeevolution(MazeEvolution *mazeev, int size, int num_threads,
                           void (*mutate) (Maze *), int (*fitness) (Maze *),
                           int width, int height, int prop);
void *thread_init_population(void *arg);
void selection(MazeEvolution *mazeev);
void *thread_mutation(void *arg);
MazeIndividual *mazeevolution(int population_size, void (*mutate) (Maze *), int (*fitness) (Maze *),
                      int maze_width, int maze_height, int maze_prop, int generations, int num_threads);
void place_holder_mutation(Maze *maze);
int place_holder_fitness(Maze *maze);

/**
 * inits an given MazeEvolution with a given population size, mutation and fitness function
 *  
 * width and heigh are the resolution of the Mazes
 * prop ist the property of calculation an straight line
 */
void init_mazeevolution(MazeEvolution *mazeev, int size, int num_threads,
                           void (*mutate) (Maze *), int (*fitness) (Maze *),
                           int width, int height, int prop) {

  mazeev->population_size = size * 2;
  mazeev->mutate          = mutate;
  mazeev->fitness         = fitness;

  // init Maze Population 
  mazeev->population      = (MazeIndividual **) malloc(sizeof(MazeIndividual *) * size * 2);
  mazeev->individuals     = (MazeIndividual *) malloc(sizeof(MazeIndividual) * size * 2);

  int k;

  // serial malloc
  for (k = 0; k < mazeev->population_size; k++) {
    mazeev->population[k] = mazeev->individuals + k;
    maze_init(&mazeev->population[k]->maze, width, height, prop);
  }

  // start threads to raise Individuals if needed
  if (num_threads > 1) {

    long n = (long) ceil(((double) mazeev->population_size) / num_threads);
    ThreadPopulationInit *threads = (ThreadPopulationInit *) malloc(sizeof(
                                                    ThreadPopulationInit) * num_threads);
    int j, to_progress = mazeev->population_size;

    // init Threads
    for (j = 0, k = 0; j < mazeev->population_size; j += n, k++) {
      threads[k].mazeev     = mazeev;
      threads[k].to_progress = &to_progress;
      threads[k].start       = j;
      threads[k].end         = (j + n < mazeev->population_size) ? j + n : mazeev->population_size;
      #ifdef MAZE_EVOLUTION_VERBOSE
        printf("Population Thread %d, %d -> %d\n", k, threads[k].start, threads[k].end);
      #endif
 
    }
    num_threads = k;

    for (k = 0; k < num_threads; k++)
      pthread_create(&threads[k].thread, NULL, thread_init_population, (void *) (threads + k));
 
    // wait untill threads are finish
    for (k = 0; k < num_threads; k++)
      pthread_join(threads[k].thread, NULL);

  } else {
    for (k = 0; k < mazeev->population_size; k++) {
      maze_path_init(&mazeev->population[k]->maze);
      maze_create_path(&mazeev->population[k]->maze);
      maze_calc_exit(&mazeev->population[k]->maze);

      #ifdef MAZE_EVOLUTION_VERBOSE
        printf("\rMaze Individuals to raise: %d \r", mazeev->population_size - k);
      #endif
    }
  }

  #ifdef MAZE_EVOLUTION_VERBOSE
    printf("Maze Individuals raised: %d\n", mazeev->population_size);
  #endif

  // select the best individuals
  selection(mazeev);

}

/**
 * Inits an given range of Individuals used for paralellism
 */
void *thread_init_population(void *arg) {
  ThreadPopulationInit *init = (ThreadPopulationInit *) arg;

  int i;
  for (i = init->start; i < init->end; i++) {
    maze_path_init(&init->mazeev->population[i]->maze);
    maze_create_path(&init->mazeev->population[i]->maze);
    maze_calc_exit(&init->mazeev->population[i]->maze);

    #ifdef MAZE_EVOLUTION_VERBOSE
      pthread_mutex_lock(&mazeev_mutex);
        (*init->to_progress)--;
        printf("\rMaze Individuals to raise: %d \r", *init->to_progress);
      pthread_mutex_unlock(&mazeev_mutex);
    #endif
  }

  return NULL;
}

/**
 * Selection function Sorts the Maze Individuals by their fuitness
 *
 * (just calls chiefsort)
 */
void selection(MazeEvolution *mazeev) {
  
  chiefsort(mazeev->population, mazeev->population_size, MAZE_EV_MIN_QUICKSORT);

}

/**
 * Function tat takes a intervall of Individual and mutates them
 * used for paralell mutation
 */ 
void *thread_mutation(void *arg) {
  ThreadMutation *thread = (ThreadMutation *) arg;

  int i, k;
    
  // mutate the Individuals intervall of this thread
  for (i = thread->start, k = thread->half_size; i < thread->end; i++, k++) {
    pthread_mutex_lock(&mazeev_mutex);
      maze_clone(&thread->mazeev->population[i]->maze, &thread->mazeev->population[k]->maze);
    pthread_mutex_unlock(&mazeev_mutex);
    thread->mazeev->mutate(&thread->mazeev->population[k]->maze);
    thread->mazeev->population[k]->fitness = thread->mazeev->fitness(
                                                &thread->mazeev->population[k]->maze);
  }

  return NULL;
}

/**
 * Inits and starts an Maze evolution with a given population size,
 * mutation and fittnes function and the propertys of the Maze individuals
 *
 * Note maze_prop ist the propery to calculate an staright line:
 *
 *  0 means you have 1/3 chanse to got staright on
 *  1 menas 2/4
 *  2 menas 3/5 and so on
 *
 *  Its an simle evolution based on an generation limit. It calculates 
 *  the given number of generations and then return the best MazeIndividual
 */
MazeIndividual *mazeevolution(int population_size, void (*mutate) (Maze *), int (*fitness) (Maze *),
                      int maze_width, int maze_height, int maze_prop, int generations, int num_threads) {

  MazeEvolution mazeev;
  init_mazeevolution(&mazeev, population_size, num_threads, mutate, fitness, 
                                              maze_width, maze_height, maze_prop);

  int i, j, k, half_size = mazeev.population_size / 2;
  ThreadMutation *threads = (ThreadMutation *) malloc(sizeof(ThreadMutation) * num_threads);
  
  if (num_threads > 1) {
    long n = (long) ceil(((double) half_size) / num_threads);
 
    // init Threads
    for (j = 0, k = 0; j < half_size; j += n, k++) {
      threads[k].mazeev = &mazeev;
      threads[k].start = j;
      threads[k].end   = (j + n < half_size) ? j + n : half_size;
      threads[k].half_size = half_size;
      #ifdef MAZE_EVOLUTION_VERBOSE
        printf("Mutation Thread %d, %d -> %d of %d -> %d\n", k, threads[k].start, threads[k].end, 
                threads[k].start + threads[k].half_size, threads[k].end + threads[k].half_size);
      #endif
 
    }
    num_threads = k;
  } else {
    free(threads);
  }


  // let the evolution do their work
  for (i = 0; i < generations; i++) {

    if (num_threads > 1) {

      // start threads to mutate the best Individuals
      for (k = 0; k < num_threads; k++)
        pthread_create(&threads[k].thread, NULL, thread_mutation, (void *) (threads + k));
     
      // wait untill threads are finish
      for (k = 0; k < num_threads; k++)
        pthread_join(threads[k].thread, NULL);

    } else { // progress serial

      for (j = 0, k = half_size; j < half_size; j++, k++) {
        maze_clone(&mazeev.population[j]->maze, &mazeev.population[k]->maze);
        mazeev.mutate(&mazeev.population[k]->maze);
        mazeev.population[k]->fitness = mazeev.fitness(&mazeev.population[k]->maze);
      }
    }

    // only the best survive
    selection(&mazeev);

    #ifdef MAZE_EVOLUTION_VERBOSE
      printf("\rGeneration to grow %d \r", generations - i);
    #endif

  }

  #ifdef MAZE_EVOLUTION_VERBOSE
    printf("Generation grown: %d  \n", generations);
  #endif
  
  // kill all except the best Individual
  for (i = 1; i < mazeev.population_size; i++)
    maze_free(&mazeev.population[i]->maze);

  // copy best individual at start of individuals array
  mazeev.individuals[0] = *mazeev.population[0];

  // free space of dead Mazes
  mazeev.individuals = realloc(mazeev.individuals, sizeof(MazeIndividual));

  
  // return the best Individual
  return &mazeev.individuals[0];
}


//***************** MUTATION FUNCTIONS *****************//

/**
 * just a simly placeholder for testing
 */
void place_holder_mutation(MazeIndividual *mazeiv) {
  int x = mazeiv->maze.width;
  x *= 2;
}

/**
 * Global struct for recalc_random_block_mutation setings
 */
typedef struct {
  int block_size, times;
} RRBMutSettings;

/**
 * Cooses an Random Block from the given Maze and Recalcutaes it
 *
 * Set Setting in rrbmut_setings
 *
 * NOTE you should have called rrbmut_setings_init bevor calling this
 */
void recalc_random_block_mutation(Maze *maze) {

}

//***************** FITNESS FUNCTIONS *****************//

/**
 * just a place holder for testing
 */
int place_holder_fitness(MazeIndividual *mazeiv) {
  int x = mazeiv->maze.width;
  x *= 2;
  return rand();
}


#endif //end of MAZE_EVOLUTION
