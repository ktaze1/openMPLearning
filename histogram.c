#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

void Hello(void);
double findMax(double*);
double findMin(double*);
int FindBin(double, double*, int);
int FindBinParallel(double*, double*, int, int*, omp_lock_t);
#define BINCOUNT 5
#define LENGTHOFARRAY 10000

int main(int argc, char *argv[]) {

 omp_lock_t writelock;
 clock_t start, end;
 double cpu_time_used;

 omp_init_lock(&writelock);

 int threadNumber = strtol(argv[1], NULL, 10);
 srand(time(NULL));
 double randomNumbers[LENGTHOFARRAY];
 int binCounts[BINCOUNT] = { 0 };
 double binMaxes[BINCOUNT] = { 0.0 };

 for (int i = 0; i < LENGTHOFARRAY; i++) {
  randomNumbers[i] = (double) (rand() % 10000) / 100;
 }
 double minNumber = findMin(randomNumbers);
 double maxNumber = findMax(randomNumbers);

 printf("minNumber:%3.2lf\n", minNumber);
 printf("maxNumber:%3.2lf\n", maxNumber);

 double binWidth = (double) (maxNumber - minNumber) / BINCOUNT;

 for (int i = 0; i < BINCOUNT; i++) {
  binMaxes[i] = minNumber + binWidth * (i + 1);
  printf("binMaxes[%d]:%3.2lf\n", i, binMaxes[i]);
 }

 /******************Bu kısım Parallel çalışacak******************/
 start = clock();
#pragma omp parallel num_threads(threadNumber)
 FindBinParallel(randomNumbers, binMaxes, BINCOUNT, binCounts, writelock);

 omp_destroy_lock(&writelock);
 /*
  for(int i = 0;i <LENGTHOFARRAY;i++){
  int bin = FindBin(randomNumbers[i],binMaxes,BINCOUNT,minNumber);
  binCounts[bin]++;
  }
  */
 end = clock();
 cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
 printf("Running time for first block is %f\n", cpu_time_used);
 int toplamSayi = 0;

 for (int i = 0; i < BINCOUNT; i++) {
  //printf("binCounts[%d]:%3d\n",i,binCounts[i]);
  toplamSayi += binCounts[i];
 }

 printf("ToplamSayi = %d\n", toplamSayi);

 return 0;
}
//void Hello(void) {
// int myRank = omp_get_thread_num();
// int threadNumber = omp_get_num_threads();
// printf("Hello From Thread out of %d from %d\n", myRank, threadNumber);
//}

double findMax(double *doubleArray) {
 double max = doubleArray[0];

 for (int i = 1; i < LENGTHOFARRAY; i++) {
  max = doubleArray[i] > max ? doubleArray[i] : max;
 }
 return max;
}

double findMin(double *doubleArray) {
 double min = doubleArray[0];

 for (int i = 1; i < LENGTHOFARRAY; i++) {
  min = doubleArray[i] < min ? doubleArray[i] : min;
 }
 return min;

}

int FindBin(double number, double *binMaxes, int binCount) {

 for (int i = 0; i < binCount; i++)
  if (number <= binMaxes[i])
   return i;

}
int FindBinParallel(double *numbers, double *binMaxes, int binCount,
  int *binCounts, omp_lock_t lock) {

 int myRank = omp_get_thread_num();
 int threadNumber = omp_get_num_threads();
 int rank = LENGTHOFARRAY / threadNumber;
 int bas;
 int bit;
 bas = myRank * rank;
 bit = (myRank == threadNumber - 1) ? LENGTHOFARRAY : (myRank + 1) * rank;

//  printf("%d. Thread Bas:%d Bitis :%d\n",myRank,bas,bit);
 for (int i = bas; i < bit; i++) {
  int bin = FindBin(numbers[i], binMaxes, binCount);
#pragma omp critical
//  omp_set_lock(&lock);
  binCounts[bin] = binCounts[bin] + 1;
//  omp_unset_lock(&lock);
 }
 return 0;

}