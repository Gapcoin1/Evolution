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
 * Type definition for the Evolution struct
 */
typedef struct Evolution Evolution;

/**
 * Some standard value for quickinsortion sort
 */
#define EV_QICKSORT_MIN 20

/**
 * Flags for the EvInitArgs
 *
 * for description have a look at EvInitArgs
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
 * Structur holding aditional information during an evolution
 *
 * +------------------------------------+-------------------------------------+
 * | Value                              | describtion                         |
 * +------------------------------------+-------------------------------------+
 * | int improovs                       | indecates how many Individual where |
 * |                                    | better then their predecessors      |
 * |                                    | during the last generation          |
 * |                                    |                                     |
 * | int generations_progressed         | indicates how many generations are  |
 * |                                    | are already processed               |
 * +------------------------------------+-------------------------------------+
 */
typedef struct {
 int improovs; 
 int generations_progressed;
} EvolutionInfo;

/**
 * An Individual wich has an definied fitness
 * the better the longer it lives
 */
typedef struct {
  void    *iv;         /* void pointer to the Individual */
  int64_t fitness;             /* the fitness of this Individual */
} Individual;

/**
 * Struct containg the neccesary information
 * to initalize an Evolution struct
 *
 * +------------------------------------+-------------------------------------+
 * | Value                              | describtion                         |
 * +------------------------------------+-------------------------------------+
 * | void *init_iv(void *opts)          | should return an void pointer to an |
 * |                                    | new initialized individual          |
 * |                                    |                                     |
 * | void clone_iv(void *dst,           | takes 2 void pointer to individuals |
 * |                       void *src,   | and should clone the content of the |
 * |                       void *opts)  | second one into the first one       |
 * |                                    |                                     |
 * | void free_iv(void *src,            | takes an void ptr to an individual  |
 * |              void *opts)           | and should free the spaces          |
 * |                                    | allocated by the given individual   |
 * |                                    |                                     |
 * | void mutate(Individual *src,       | takes an void pointer to an         |
 * |             void *opts)            | individual and should change it in  |
 * |                                    | a way that the probability to       |
 * |                                    | improove it is around 1/5           |
 * |                                    |                                     |
 * | int64_t fitness(Individual *src,   | takes an void pointer to an         |
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
 * | char continue_ev(Evolution *const  | takes an pointer to the current     |
 * |                  ev)               | Evolution struct (read onley) and   |
 * |                                    | should return 0 if the calculaten   |
 * |                                    | should stop and 1 if the calculaten |
 * |                                    | should continue                     |
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
 * Note: - The void pointer to ivs are not pointer to an Individual 
 *         struct, they are the iv element of the Individual struct.
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
  void     *(*init_iv)    (void *);
  void     (*clone_iv)    (void *, void *, void *);
  void     (*free_iv)     (void *, void *);
  void     (*mutate)      (Individual *, void *);
  int64_t  (*fitness)     (Individual *, void *);
  void     (*recombinate) (Individual *, 
                           Individual *, 
                           Individual *, 
                           void *);
  char     (*continue_ev) (Evolution *const);
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
 * Struct holding information for the thread clients
 */
typedef const struct {
  Evolution *ev;          /* pointer to the current Evolution struct */
  int index;              /* index of the current working thread     */
  // TODO add the opt ptr for the current index (at init) so we can scip the opt[index]
} EvThreadArgs;

/**
 * The Evolution struct
 *
 * the most values are already discussed: see EvInitArgs
 *
 * the other balues are:
 *
 * +------------------------------------+-------------------------------------+
 * | Value                              | describtion                         |
 * +------------------------------------+-------------------------------------+
 * | Individual **population            | the population of Individuals only  |
 * |                                    | pointers for faster sorting         |
 * |                                    |                                     |
 * | Individual * ivs                   | the Individuals                     |
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
 * | int deaths                         | number of individuals die during an |
 * |                                    | gerenation change                   |
 * |                                    |                                     |
 * | int survivors                      | number of individuals survive       |
 * |                                    | during an gerenation change         |
 * |                                    |                                     |
 * | char sort_max                      | indicates wether to sort            |
 * |                                    | Individuals by max or min fitness   |
 * |                                    |                                     |
 * | uint16_t verbose                   | the verbosity level, see flags      |
 * |                                    | describtion at EvInitArgs           |
 * |                                    |                                     |
 * | int min_quicksort                  | min array length to change from     |
 * |                                    | quick to insertion sort             |
 * |                                    |                                     |
 * | int parallel_index                 | used to syncornize the parallel     |
 * |                                    | access of the individuals during    |
 * |                                    | initializing, recombination usw.    |
 * |                                    | It will be decremented an used for  |
 * |                                    | the area where the individuals will |
 * |                                    | cosen for mutation usw.             |
 * |                                    |                                     |
 * | int parallel_start                 | indicates where to start repleacing |
 * |                                    | individuals during parallel         |
 * |                                    | calculation (array index)           |
 * |                                    |                                     |
 * | int parallel_end                   | indicates where to end repleacing   |
 * |                                    | individuals during parallel         |
 * |                                    | calculation (array index)           |
 * |                                    |                                     |
 * | int i_mut_propability              | because int rand() is faster than   |
 * |                                    | double rand() we transfer           |
 * |                                    | mutation_propability with RAND_MAX  |
 * |                                    | into i_mut_propability to faster    |
 * |                                    | calculate wether to mutate or not   |
 * |                                    |                                     |
 * | TClient *clients                   | Thread Clients which handle the     |
 * |                                    | threads used to calculate parallel, |
 * |                                    | saving syscalls by reusing threads  |
 * |                                    |                                     |
 * | EvThreadArgs thread_args           | different argments for each thread  |
 * |                                    | containing an thread specific index |
 * |                                    |                                     |
 * | EvolutionInfo info                 | additional information during an    |
 * |                                    | evolution                           |
 * +------------------------------------+-------------------------------------+
 * 
 * Note: many values are const like opts (an const pointer to an array of
 *       const void pointers), function pointers, but some not.
 *       So this is a warning: do not change any value in this struct!! 
 *       use the init function ;-)
 */
struct Evolution {
  Individual     **population;               
  Individual     *ivs;               
  void           *(*const init_iv)     (void *);
  void           (*const  clone_iv)    (void *, void *, void *);
  void           (*const  free_iv)     (void *, void *);
  char           (*const  continue_ev) (Evolution *const); 
  void           (*const  mutate)      (Individual *, void *);
  int64_t        (*const  fitness)     (Individual *, void *);
  void           (*const  recombinate) (Individual *, 
                                        Individual *, 
                                        Individual *, 
                                        void *);
  const int      population_size;
  const int      generation_limit;
  const double   mutation_propability;
  const double   death_percentage;
  const char     use_recombination;              
  const char     use_muttation;                  
  const char     always_mutate;                  
  const char     keep_last_generation;           
  const char     use_abort_requirement;          
  const int      deaths;
  const int      survivors;
  const char     sort_max;                     
  const uint16_t verbose;                  
  const int      min_quicksort;              
  void *const    *const opts;   
  const int      num_threads; 
  int            parallel_index;
  int            parallel_start;
  int            parallel_end; 
  const int      i_mut_propability;
  TClient *const thread_clients;
  EvThreadArgs   *const thread_args;
  EvolutionInfo  info;
};

/**
 * Returns pointer to an new and initialzed Evolution
 * take pointers for an EvInitArgs struct
 *
 * see comment of EvInitArgs for more informations
 */
Evolution  *new_evolution(EvInitArgs *args);  

/**
 * Starts the actual evolution, which means
 *  - calculate the fitness for each Individual
 *  - sort Individual by fitness
 *  - remove worst ivs
 *  - grow a new generation
 *  - calculate untill generation limit is reatched
 *    or continue_ev returns 0
 *
 * Retruns the best iv
 */
Individual *evolute(Evolution *ev);

/**
 * Computes an evolution for the given args
 * and returns the best Individual
 *
 * see comment of EvInitArgs for more informations
 */
Individual best_evolution(EvInitArgs *args);

/**
 * Frees unneded resauces after an evolution calculation
 * Note it don't touches the spaces of the best individual
 */
void evolution_clean_up(Evolution *ev);

/**
 * Returns the Size an Evolution with the given args will have
 *
 * sizeof_iv are the size of one individual
 * sizeof_opt are the size of one opt for one thread
 */
uint64_t ev_size(int population_size, 
                 int num_threads, 
                 int keep_last_generation, 
                 uint64_t sizeof_iv, 
                 uint64_t sizeof_opt);


#endif // end of EVOLUTION_HEADER
