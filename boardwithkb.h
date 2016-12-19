#ifndef BOARDWITHKB_H_
#define BOARDWITHKB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifdef PARALLEL
#include "mpi.h"
#endif

double *allocate_double_vector (int n);
int *allocate_int_vector (int n);
void shuffleArr(int * arr, int n);
int posIsValid(int pos, int l_size, int dir, int orient, int N, int * k) ;
void generateBoard(double * ships, int N, int n, int np) ;
int posIsValidK(int pos, int l_size, int dir, int orient, int N, int * used, int * k) ;
void generateBoardWithK(double * shipProbK, int N, int n, int * k, int np) ;
void generateIncompleteBoard(int * k, int N);

void genPlayerBoard(int * board, int N, int * ships);
int sum_board(int * board, int N);

#endif
