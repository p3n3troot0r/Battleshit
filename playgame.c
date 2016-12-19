#ifdef PARALLEL
#include "mpi.h"
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


  int id, np;	
#ifdef PARALLEL	
  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &np);
  MPI_Comm_rank (MPI_COMM_WORLD, &id);
#else
	id = 0;
	np = 1;
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
	int done = 0;
	int ht;
	player c1, c2;
	int move; 
	double * g_raw, g_cond;
	double cur_board[N*N];
	
	if(id == 0) {
		c1.my_board = allocate_int_vector(N*N); /* ship placements */
		c2.my_board = allocate_int_vector(N*N);
		c1.difficulty = 0; /* easy */
		c1.pid = 0;
	
		c1.view_board = allocate_int_vector(N*N); /* viewable world */
		c2.view_board = allocate_int_vector(N*N);
		c2.difficulty = 1; /* medium */
		c2.pid = 1;
	
		memset(c1.my_board, 0, N*N*sizeof(int));
		memset(c2.my_board, 0, N*N*sizeof(int));
		memset(c1.view_board, (-1), N*N*sizeof(int));
		memset(c2.view_board, (-1), N*N*sizeof(int));
	
		memset(&c1.hits,(-1),17*sizeof(int));
		memset(&c2.hits,(-1),17*sizeof(int));
	
//		int done = 0;
	
		genPlayerBoard(c1.my_board, N, c1.ships);
		genPlayerBoard(c2.my_board, N, c2.ships);
	
		g_raw = allocate_double_vector(N*N*nships); 
		g_cond = allocate_double_vector(N*N*nships); 
	}
	
	double * cond_prob = allocate_double_vector(N*N*nships); /* used by both computers to get cond model */
	double * raw_prob = allocate_double_vector(N*N*nships); 
	while (!done) {
		cur_board = {0};
		/*
		 * do move
		 * * Take in viewable board state, calculate most likely space
		
		 * check for hit ==> check for loss
		 * update boards
		 */
		
		memset(raw_prob, 0, N*N*nships*sizeof(double));
		memset(cond_prob, 0, N*N*nships*sizeof(double));
		generateBoard(raw_prob, N, n, np); /* creates raw probability based off n boards */
		MPI_Reduce(raw_prob, g_raw, (N*N*nships), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		if(id == 0) {
			move = getBestMove(c1.view_board, g_raw, N);	
		}


		if( id == 0 ) {
			// use get best dual for hard AI, get best move for easy
		  MPI_Allreduce(c1.view_board, &cur_board, (N*N), MPI_INT, MPI_SUM, MPI_COMM_WORLD);
		}
		generateBoardWithK(cond_prob, N, n, cur_board, np);
		MPI_Reduce(cond_prob, glob_pcond, (N*N*nships), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		
		if(id == 0) {
			move = getBestDual(c2.view_board, g_raw, g_cond, N);	
		}
			
	}
	
	if(id == 0) {
		free(g_raw);
		free(g_cond);
	}
	
  MPI_Finalize();

	free(c1.my_board);
	free(c2.my_board);
	
  return 0;
}

	