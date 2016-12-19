#include <stdio.h>
#include <stdlib.h>
#include "player.h"

void markBoard(int * b1, int * b2) { /* mark b1 with b2 */

}

int getBestMove(int * b, double * prob, int N) {
	int i;
	int m = 0;
	int max = -1;
	
	for(i = 0; i < N*N; i++) {
		if((prob[i] > max) && (b[i] < 0)) { // if this spot is good and not marked yet
			m = i;
			max = prob[i];
		}
	}
	return m;
}

int getBestDual(int * b, double * raw, double * cond, int N) {
	int i,j;
	int m = 0;
	int max = -1;
	
	for(i = 0; i < N*N; i++) {
		for(j = 0; j < N*N; j++) {
			if((raw[i] + cond[i] > max) && (b[i] < 0)) {
				m = i;
			  max = raw[i] + cond[i];
			}
		}
	}
	return m;
}

int doMove(int * b1, int diff) { /* do move based on environment */ 

	
	int n = 10000; /* generate random boards */
	int N = 10;
	int nships = 4;
	double * raw_prob = allocate_double_vector(N*N*nships); // do it every turn - more interesting
	memset(raw_prob, 0, N*N*nships);
	generateBoard(raw_prob, N, n); /* creates raw probability based off n boards */
	
	int move = 0;
	
	/* different moves for different difficulties */
	double * cond_prob;
	int p1, p2;
	
	if(diff == 0) {
		/* pick spot (NOT TRIED YET) with highest probability of being occupied 
		 * based off raw */
		p1 = getBestMove(b1, raw_prob, N);
	}
	else if(diff == 1) {
		/* pick spot (NOT TRIED YET) with highest probability of being occupied
		 * based off raw AND b1 */
		cond_prob = allocate_double_vector(N*N*nships);
		
		generateBoardWithK(cond_prob, N, n, b1);
		move = getBestDual(b1, raw_prob, cond_prob, N);
		
		free(cond_prob);
	}
	else {
		;
	}
	
	free(raw_prob);
	return move;
}
