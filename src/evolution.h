/**
 * Header of an Evolution Algorithm
 *
 * to solf an optimation Problem
 */
#ifndef EVOLUTION_HEADER
#define EVOLUTION_HEADER
#include "C-Utils/Sort/src/sort.h"
#include "C-Utils/Thread-Clients/src/thread-client.h"

/**
 * Structur holding aditional information during an evolution
 */
typedef struct {
 int improovs; 
} EvolutionInfo;

/**
 * Some standard values
 */
#define EV_MUT_ACCURACY 10000
#define EV_QICKSORT_MIN 20

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
  void    *individual;         /* void pointer to the Individual */
  int64_t fitness;             /* the fitness of this Individual */
} Individual;

/**
 * Struct containg the neccesary information
 * to initalize an Evolution struct
 *
 * +------------------------------------+-------------------------------------+
 * | Value                              | describtion                         |
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
 * |                                    |                                     |
 * | int population_size                | Number of Individuals in your       |
 * |                                    | Evolution population (the real      |
 * |                                    | number of Individuals hold in       |
 * |                                    | memory can be differ)               |
 * |                                    |                                     |
 * | double generation_limit            | the maximum amoung of generation to |
 * |                                    | calculate                           |
 * |                                    |                                     |
 * | double mutation_propability        | the propability for one single      |
 * |                                    | Individual to mutate during an      |
 * |                                    | generation change                   |
 * |                                    |                                     |
 * | double death_percentage            | percent of Individuals dying during |
 * |                                    | an generation change. Also from     |
 * |                                    | (1 - death_percentage) percent of   |
 * |                                    | the best Individuals the next       |
 * |                                    | generation will be calculated       |
 * |                                    |                                     |
 * | void **opts                        | threadwide opts given as opts to    |
 * |                                    | the above function, thread[0] gets  |
 * |                                    | opts[0] usw                         |
 * |                                    |                                     |
 * | int num_threads                    | number of threads to use            |
 * |                                    |                                     |
 * | uint16_t flags                     | flags are discussed below           |
 * +------------------------------------+-------------------------------------+
 *
 * Note: - The void pointer to individuals are not pointer to an Individual 
 *         struct, they are the individual element of the Individual struct.
 *       - The last parameter of each function (opts) is an void pointer to 
 *         optional arguments of your choice
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
 * To all of the combinations below an EV_SMIN / EV_SMAX can be added
 * standart is EV_SMIN
 *
 * Also an verbosytiy level of:
 *    EV_VERBOSE_QUIET    (EV_VEB0), 
 *    EV_VERBOSE_ONELINE  (EV_VEB1)
 *    EV_VERBOSE_HIGH     (EV_VEB2) 
 *    EV_VERBOSE_ULTRA    (EV_VEB3)
 * can be added, standart is EV_VERBOSE_QUIET
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
 */
typedef struct {
  void     *(*init_individual) (void *);
  void     (*clone_individual) (void *, 
                                void *, 
                                void *);
  void     (*free_individual) (void *, void *);
  void     (*mutate) (Individual *, void *);
  int64_t  (*fitness) (Individual *, void *);
  void     (*recombinate) (Individual *, 
                           Individual *, 
                           Individual *, 
                           void *);
  char     (*continue_ev) (Individual *, void *);
  int      population_size;
  int      generation_limit;
  double   mutation_propability;
  double   death_percentage;
  void     **opts;
  int      num_threads; 
  uint16_t flags;
} EvInitArgs;



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
 * the most values ar already discussed: see EvInitArgs
 *
 * the other balues are:
 *
 * +------------------------------------+-------------------------------------+
 * | Value                              | describtion                         |
 * +------------------------------------+-------------------------------------+
 * | Individual **population            | the population of Individuals only  |
 * |                                    | pointers for faster sorting         |
 * |                                    |                                     |
 * | Individual * individuals           | the Individuals                     |
 * |                                    |                                     |
 * | char use_recombination             | indicates wether to recombinate the |
 * |                                    | Individuals during a generation     |
 * |                                    | change or not                       |           
 * |                                    |                                     |
 * | char use_muttation                 | indicates wether to mutate the      |
 * |                                    | Individuals during a generation     |
 * |                                    | change or not                       |           
 * |                                    |                                     |
 * | char always_mutate                 | indicates wether to mutate every    |
 * |                                    | Individuals during every generation |
 * |                                    |                                     |
 * | char keep_last_generation          | indicates wether to keep or discard |
 * |                                    | the last generation during a change |
 * |                                    |                                     |
 * | char use_abort_requirement         | indicates wether to use continue_ev |
 * |                                    | or to calculate until generation    |
 * |                                    | limit is reatched                   | 
 * |                                    |                                     |
 * | char sort_max                      | indicates wether to sort            |
 * |                                    | Individuals by max or min fitness   |
 * |                                    |                                     |
 * | uint16_t verbose                   | the verbosity level, see flags      |
 * |                                    | describtion at EvInitArgs           |
 * |                                    |                                     |
 * | int min_quicksort                  | min array length to change from     |
 * |                                    | quick to insertion sort             |
 * +------------------------------------+-------------------------------------+
 * 
 */
struct Evolution {
  void     *(*init_individual) (void *);
  void     (*clone_individual) (void *, 
                                void *, 
                                void *);
  void     (*free_individual) (void *, void *);
  void     (*mutate) (Individual *, void *);
  int64_t  (*fitness) (Individual *, void *);
  void     (*recombinate) (Individual *, 
                           Individual *, 
                           Individual *, 
                           void *);
  char     (*continue_ev) (Individual *, void *);
  int      population_size;
  int      generation_limit;
  double   mutation_propability;
  double   death_percentage;
  void     **opts;
  int      num_threads; 
  Individual **population;               
                                         
                                         
  Individual *individuals;               
  char   use_recombination;              
  char   use_muttation;                  
  char   always_mutate;                  
  char   keep_last_generation;           
  char   use_abort_requirement;          
  char   sort_max;                     
  uint16_t  verbose;                  
  int    min_quicksort;              
                                    
                                   
  struct {
    int parallel_index, start, end, num_threads,          // TODO explain, and my be better names 
        mutate, generations_progressed;
    TClient *thread_clients;
    EvolutionThread *ev_threads;
    char last_improovs_str[25];
    EvolutionInfo info;
  } paralell;
};



// functions
Individual *evolute(Evolution *ev);
Evolution  *new_evolution(EvInitArgs *args);  // TODO add description to each function declaration
Individual best_evolution(EvInitArgs *args);
void *threadable_init_individual(void *arg);
void *threadable_recombinate(void *arg);
void *threadable_mutation_onely_1half(void *args);
void *threadable_mutation_onely_rand(void *args);
void evolution_clean_up(Evolution *ev);
uint64_t ev_size(int population_size, 
                 int num_threads, 
                 int keep_last_generation, 
                 uint64_t sizeof_iv, 
                 uint64_t sizeof_opt);


#endif // end of EVOLUTION_HEADER
