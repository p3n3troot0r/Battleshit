#ifdef PARALLEL
#include <mpi.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "ship.c"
#include "player.h"
#include "boardwithkb.h"

int main (int argc, char *argv[]) {

#ifdef PARALLEL	
  int id, np;	
  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &np);
  MPI_Comm_rank (MPI_COMM_WORLD, &id);
#endif

  /* 
	 * Introduce game
	 * INITIALLY ONLY Comp vs comp ==> Create players
	 *    LATER IF TIME enable pvp
	 * Enable gameplay - each process has boards
	 */
	
	/* for parallel */
//	double * global_raw
	
	int N = 10;
	int nships = 4;
	int n = 10000;
	
	/* Calculate initial probability model of empty board */
//	double * raw_prob = allocate_double_vector(N*N*nships);
//	memset(raw_prob, 0, N*N*nships);
//	generateBoard(raw_prob, N, n); /* creates raw probability based off n boards */
	
	// if(id == 0)
	player c1, c2;
	c1.my_board = allocate_int_vector(N*N); /* ship placements */
	c2.my_board = allocate_int_vector(N*N);
	c1.difficulty = 0; /* easy */
	c1.pid = 0;
	
	c1.view_board = allocate_int_vector(N*N); /* viewable world */
	c2.view_board = allocate_int_vector(N*N);
	c2.difficulty = 1; /* medium */
	c2.pid = 1;
	
	memset(c1.my_board, 0, N*N);
	memset(c2.my_board, 0, N*N);
	memset(c1.view_board, (-1), N*N);
	memset(c2.view_board, (-1), N*N);
	
	int done = 0;
	int move; 
	
	genPlayerBoard(c1.my_board, N);
	genPlayerBoard(c2.my_board, N);
	
	
	//double * cond_prob = allocate_double_vector(N*N*nships); /* used by both computers to get cond model */
	
	//while (!done) {
		
		/*
		 * do move
		 * * Take in viewable board state, calculate most likely space
		
		 * check for hit ==> check for loss
		 * update boards
		 */
		
	//	move = doMove(c1.view_board, c1.difficulty);
	//	
		
	//	move = doMove(c2.view_board, c2.difficulty);
	
//	}
	
	
	
#ifdef PARALLEL	
  MPI_Finalize();
#endif

	free(c1.my_board);
	free(c2.my_board);
	
  return 0;
}

	