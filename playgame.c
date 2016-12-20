
#include "mpi.h"


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
  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &np);
  MPI_Comm_rank (MPI_COMM_WORLD, &id);
//	id = 0;
//	np = 1;
  //srand(time(NULL));

	if(id == 0) { printf("np = %d \n",np); }
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
	double * g_raw, * g_cond, * ond_raw, * ond_cond;
	int * cur_board = allocate_int_vector(N*N); // for all processes
	int cmove;
	int s_time, e_time;
	
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
		ond_raw = allocate_double_vector(N*N);
		ond_cond = allocate_double_vector(N*N);
	}
	
	int q;
	int cont = 0;
	int lost;
	
	double * cond_prob = allocate_double_vector(N*N*nships); /* used by both computers to get cond model */
	double * raw_prob = allocate_double_vector(N*N*nships);
	 
	MPI_Barrier(MPI_COMM_WORLD);
	s_time = MPI_Wtime();
	while (!done) {
		//memset(&cur_board, 0, N*N*sizeof(int));
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
		
		MPI_Barrier(MPI_COMM_WORLD);
		
		if(id == 0) {
			
			makeoneD(g_raw, ond_raw);

			/* PLAYER 1 (EASY) MOVE */
			move = getBestMove(c1.view_board, ond_raw, N);
			cmove = c2.my_board[move];
			if(c2.my_board[move] > 1) {
				// got a hit -- which ship was it? cmove
				c1.view_board[move] = 1;
				
				// cmove has the ship - check if that many adjacents in either dir hit
				for(q = 0; q < cmove; q++) {
					
					if(q == cmove-1) {
						if(cmove == 2) { printf("Player 1 sunk Player 2s destroyer!\n"); }
						if(cmove == 3) { printf("Player 1 sunk Player 2s sub/cruiser!\n"); }
						if(cmove == 4) { printf("Player 1 sunk Player 2s batteleship!\n"); }
						if(cmove == 5) { printf("Player 1 sunk Player 2s carrier!\n"); }
					}
					
					cont = 0;
					if(move + (q*1) < 100) {
						if(move + (q*10) < 100) {
							if( (c2.my_board[move+(1*q)] == cmove) || (c2.my_board[move+(q*10)] == cmove) ) { cont = 1; }
						} 
						else if (c2.my_board[move+(q*1)] == cmove) { cont = 1; }
					}
					if(move - (q*1) >= 0) {
						if(move - (q*10) >= 0) {
							if( (c2.my_board[move-(q*1)] == cmove) || (c2.my_board[move-(q*10)] == cmove) ) { cont = 1; }
						}
						else if (c2.my_board[move-(q*1)] == cmove) { cont = 1; }
					}
					if(!cont) { break; }
				}
				
			} else { 
				c1.view_board[move] = 0;
				printf("Player 1 missed! \n");
			}
			
			/* CHECK IF P2 LOST */
			lost = sum_board(c1.view_board, N);
			if(lost == 17) {
				done = 1;
				e_time = MPI_Wtime();
				printf("Player 1 wins! Game took %5.10e seconds \n", (e_time - s_time));
				break;
			}
			
		}

		//memset(&cur_board, 0, N*N*sizeof(int));
		if( id == 0 ) {
			// use get best dual for hard AI, get best move for easy
		  //MPI_Allreduce(c1.view_board, &cur_board, (N*N), MPI_INT, MPI_SUM, MPI_COMM_WORLD);
			memcpy(cur_board, c2.view_board, N*N*sizeof(int));
			MPI_Bcast(cur_board, N*N, MPI_INT, 0, MPI_COMM_WORLD);
		}
		
		/* PLAYER 2 (MEDIUM) MOVE */
		MPI_Barrier(MPI_COMM_WORLD);
		
		generateBoardWithK(cond_prob, N, n, cur_board, np);
		MPI_Reduce(cond_prob, g_cond, (N*N*nships), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		
		if(id == 0) {
			makeoneD(g_cond, ond_cond);
			move = getBestDual(c2.view_board, ond_raw, ond_cond, N);	
			cmove = c2.my_board[move];
			if(c1.my_board[move] > 1) {
				// got a hit -- which ship was it? cmove
				c2.view_board[move] = 1;
				
				// cmove has the ship - check if that many adjacents in either dir hit
				for(q = 0; q < cmove; q++) {
					if(q == cmove-1) {
						if(cmove == 2) { printf("Player 2 sunk Player 1s destroyer!\n"); }
						if(cmove == 3) { printf("Player 2 sunk Player 1s sub/cruiser!\n"); }
						if(cmove == 4) { printf("Player 2 sunk Player 1s batteleship!\n"); }
						if(cmove == 5) { printf("Player 2 sunk Player 1s carrier!\n"); }
					}
					
					cont = 0;
					if(move + (q*1) < 100) {
						if(move + (q*10) < 100) {
							if( (c1.my_board[move+(q*1)] == cmove) || (c1.my_board[move+(q*10)] == cmove) ) { cont = 1; }
						} 
						else if (c1.my_board[move+(q*1)] == cmove) { cont = 1; }
					}
					if(move - 1 >= 0) {
						if(move - 10 >= 0) {
							if( (c1.my_board[move-(q*1)] == cmove) || (c1.my_board[move-(q*10)] == cmove) ) { cont = 1; }
						}
						else if (c1.my_board[move-(q*1)] == cmove) { cont = 1; }
					}
					if(!cont) { break; }
				}
				
			} else { 
				c2.view_board[move] = 0;
				printf("Player 2 missed! \n");
				
			}
			
			/* CHECK IF P1 LOST */
			lost = sum_board(c2.view_board, N);
			if(lost == 17) {
				done = 1;
				e_time = MPI_Wtime();
				printf("Player 2 wins! Game took %5.10e seconds \n", (e_time - s_time));
				break;
			}
			
		}
	}

	free(cond_prob);
	free(raw_prob);
	
	if(id == 0) {
		free(g_raw);
		free(g_cond);
		free(ond_raw);
		free(ond_cond);
		free(c1.my_board);
		free(c2.my_board);
		free(c1.view_board);
		free(c2.view_board);
	}
	
  MPI_Finalize();


  return 0;
}

	