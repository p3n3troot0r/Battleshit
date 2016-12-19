#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct { /* This is the AI */
	int pid; 
	int difficulty; // 0 = easy, 1 = medium, 2 = hard
	int dest; // initially 0
	
	double * my_board;
	double * view_board; // = allocate_double_vector(100);
	double ships[17]; // 0-1 destr; 2-4 sub; 5-7 cruis.; 8-11 bship; 12-16 carrier;
	
} player;

void markBoard(double * b1, double * b2); /* mark b1 with b2 */
int doMove(double * b1, double * raw_p, int diff); /* do move based on environment */ 

#endif
