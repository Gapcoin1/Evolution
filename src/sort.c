/**
 * Justa n implementation of som Quicksort variants
 */
#ifndef CHIEFSORT
#define CHIEFSORT
#include "sort.h"


/**
 * Starts an paralell chiefsort
 */
void paralellsort(CHIEFSORT_TYPE *ary, int length, int min_insertionsort, int threads) {

  if (threads > 1) {
    ChiefSortParalell para;
    para.ary = ary;
    para.length = length;
    para.threads = threads;
    para.min_insertionsort = min_insertionsort;
 
    paralellchiefsort((void *) &para);
  } else
    chiefsort(ary, length, min_insertionsort);
}

/**
 * Just Quicksort
 */
void quicksort(CHIEFSORT_TYPE *ary, int length) {

  CHIEFSORT_TYPE piv;
  CHIEFSORT_TYPE temp;
  IntAry stack;
  int l, r, left, right;
  ARY_INIT(int, stack, (int) 5 * (log2((double) length) / 3)); 

  left  = 0;
  right = length - 1;

  while (1) {

    if (left < right) {

      l = left;
      r = right;
      piv = ary[(rand() % (r - l)) + l];

      while (l < r) {
        
        while (CHIEFSORT_BIGGER(ary[r], piv))
          r--; 

        while (CHIEFSORT_SMALER(ary[l], piv)) 
          l++; 

        if (CHIEFSORT_EQL(ary[l], ary[r])) {
          r--;

        // switch left an right
        } else if (l < r) {
          temp = ary[l];
          ary[l] = ary[r];
          ary[r] = temp;
        }

      }

      ARY_PUSH(int, stack, right)  // statt push neuer thread mit qicksort von ary + right + 1
      right = (left > l - 1) ? left : l - 1;

    } else {
      if (stack.length > 0) {
        left = right + 1;
        ARY_PULL(stack, right)
      } else break;
    }

  }

}

/**
 * Just Insertionsort
 */
void insertionsort(CHIEFSORT_TYPE *ary, int length) {
  
  int i, j;
  CHIEFSORT_TYPE temp;

  for (i = 1; i < length; i++) {
    temp = ary[i];
    j = i - 1;

    while (temp < ary[j] && j >= 0) {
      ary[j + 1] = ary[j];
      j = j - 1;
    }   
    
    ary[j + 1] = temp;
  }

}

/**
 * Improofed variant of QuickSort which uses insertionsort at an specific min array size
 */
void chiefsort(CHIEFSORT_TYPE *ary, int length, int min) {

  CHIEFSORT_TYPE piv;
  CHIEFSORT_TYPE temp;
  IntAry stack;
  int l, r, left, right;
  ARY_INIT(int, stack, (int) 5 * (log2((double) length) / 3)); 

  left  = 0;
  right = length - 1;

  while (1) {

    if (right - left < min) {
      insertionsort(ary + left, (right - left) + 1);
      left = right;
    }

    if (left < right) {

      l = left;
      r = right;
      piv = ary[(rand() % (r - l)) + l];

      while (l < r) {
        
        while (CHIEFSORT_BIGGER(ary[r], piv))
          r--; 

        while (CHIEFSORT_SMALER(ary[l], piv)) 
          l++; 

        if (CHIEFSORT_EQL(ary[l], ary[r])) {
          r--;

        // switch left an right
        } else if (l < r) {
          temp = ary[l];
          ary[l] = ary[r];
          ary[r] = temp;
        }

      }

      ARY_PUSH(int, stack, right)  // statt push neuer thread mit qicksort von ary + right + 1
      right = (left > l - 1) ? left : l - 1;

    } else {
      if (stack.length > 0) {
        left = right + 1;
        ARY_PULL(stack, right)
      } else break;
    }

  }

}


/**
 * Improofed variant of QuickSort which uses insertionsort at an specific min array size
 * 
 * it creates threas if possible when stating to clac an new part
 */
void *paralellchiefsort(void *arg) {

  ChiefSortParalell *para = (ChiefSortParalell *) arg;
  int length              = para->length;
  int min                 = para->min_insertionsort;
  CHIEFSORT_TYPE *ary     = para->ary;
  CHIEFSORT_TYPE piv;
  CHIEFSORT_TYPE temp;
  int l, r, left, right, error;
  pthread_t thread[2];

  left  = 0;
  right = length - 1;


  if (right - left < min) {
    insertionsort(para->ary + left, (right - left) + 1);
    return NULL;
  }

  // progress quicksort patern
  l = left;
  r = right;
  piv = ary[(rand() % (r - l)) + l];

  while (l < r) {
    
    while (CHIEFSORT_BIGGER(ary[r], piv))
      r--; 

    while (CHIEFSORT_SMALER(ary[l], piv)) 
      l++; 

    if (CHIEFSORT_EQL(ary[l], ary[r])) {
      r--;

    // switch left an right
    } else if (l < r) {
      temp = ary[l];
      ary[l] = ary[r];
      ary[r] = temp;
    }

  }

  // creat new threads for each patern if necessary
  ChiefSortParalell new_para1;
  new_para1.min_insertionsort = min;

  // onely one pattern (worst case)
  if (l == left || r == right) {
    new_para1.length = length - 1;
    new_para1.threads = para->threads;

    if (l == left)
      new_para1.ary = ary + 1;
    else
      new_para1.ary = ary;
    
    paralellchiefsort(&new_para1);


  // create new threads for each patren if wanted
  } else {

    ChiefSortParalell new_para2;
    new_para2.min_insertionsort = min;
    new_para2.threads = new_para1.threads = para->threads / 2;
    new_para1.length = (r - left) + 1;
    new_para1.ary = ary + left;
    new_para2.length = right - l;
    new_para2.ary = ary + l + 1;

    if (para->threads % 2 == 1)
      new_para2.threads++;

    if (new_para1.threads > 1)
      error = pthread_create(thread, NULL, &paralellchiefsort, (void *) &new_para1);
    else 
      error = pthread_create(thread, NULL, &serialchiefsort, (void *) &new_para1);

    if (new_para2.threads > 1)
      error += pthread_create(thread + 1, NULL, &paralellchiefsort, (void *) &new_para2);
    else 
      error += pthread_create(thread + 1, NULL, &serialchiefsort, (void *) &new_para2);
    
    if (error != 0) {
      printf("[ERROR in paralellchiefsort couldn't create new thread!\n");
      exit(1);
    }
    
    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);

  }


  return NULL;
}

/**
 * Improofed variant of QuickSort which uses insertionsort at an specific min array size
 * 
 * serial version for a thread
 */
void *serialchiefsort(void *arg) {

  ChiefSortParalell *para = (ChiefSortParalell *) arg;
  int length              = para->length;
  int min                 = para->min_insertionsort;
  CHIEFSORT_TYPE *ary     = para->ary;

  CHIEFSORT_TYPE piv;
  CHIEFSORT_TYPE temp;
  IntAry stack;
  int l, r, left, right;
  ARY_INIT(int, stack, (int) 5 * (log2((double) para->length) / 3)); 

  left  = 0;
  right = length - 1;

  while (1) {

    if (right - left < min) {
      insertionsort(para->ary + left, (right - left) + 1);
      left = right;
    }

    if (left < right) {

      l = left;
      r = right;
      piv = ary[(rand() % (r - l)) + l];

      while (l < r) {
        
        while (CHIEFSORT_BIGGER(ary[r], piv))
          r--; 

        while (CHIEFSORT_SMALER(ary[l], piv)) 
          l++; 

        if (CHIEFSORT_EQL(ary[l], ary[r])) {
          r--;

        // switch left an right
        } else if (l < r) {
          temp = ary[l];
          ary[l] = ary[r];
          ary[r] = temp;
        }

      }

      ARY_PUSH(int, stack, right) 
      right = (left > l - 1) ? left : l - 1;

    } else {
      if (stack.length > 0) {
        left = right + 1;
        ARY_PULL(stack, right)
      } else break;
    }

  }

  return NULL;
}

#endif //end of CHIEFSORT
