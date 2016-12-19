#ifndef BOARDWITHKB_H_
#define BOARDWITHKB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

double *allocate_double_vector (int n);
int *allocate_int_vector (int n);
void shuffleArr(int * arr, int n);
int posIsValid(int pos, int l_size, int dir, int orient, int N, int * used) ;
void generateBoard(double * ships, int N, int n) ;
int posIsValidK(int pos, int l_size, int dir, int orient, int N, int * used, int * k) ;
void generateBoardWithK(double * shipProbK, int N, int n, int * k) ;
void generateIncompleteBoard(int * k, int N);
void genPlayerBoard(double * board, int N);
int sum_board(double * board, int N);
#endif
