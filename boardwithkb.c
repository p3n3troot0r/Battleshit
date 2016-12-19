/* 
 * Author: Duncan Woodbury
 * Date:	 12-05-2016
 * This file provides a method to generate arbitrary battleship boards in an effort
 *  to evaluate the probabilities of each ship(carrier(5), cruiser(3), bship(4), sub(3), destr(2))
 *  occurring at any given space.
 * A serial implementation will be provided (so this can be translated to java), then a 
 *  parallel method compatible with MPI is given.
 */

/*
 * TODO:  
 *				* Convert to parallel code for maya
 */

#include "boardwithkb.h"

double *allocate_double_vector (int n) {
  double *x;
	int j = 0;
  x = (double*) calloc (n, sizeof(double));
	
  if (x == NULL) {
    fprintf (stderr, "Problem allocating memory for vector\n");
#ifdef PARALLEL
    MPI_Abort (MPI_COMM_WORLD, 1);
#else
    exit (1);
#endif
  }

  return x;
}

int *allocate_int_vector (int n) {
  int *x;

  x = (int*) calloc (n, sizeof(int));
	int j = 0;

  if (x == NULL) {
    fprintf (stderr, "Problem allocating memory for vector\n");
#ifdef PARALLEL
    MPI_Abort (MPI_COMM_WORLD, 1);
#else
    exit (1);
#endif
  }
  return x;
}

int sum_board(double * board, int N) {
	int sum = 0;
	int i;
	for(i = 0; i < N*N; i++) {
		sum += (board[i] == 1) ? 1 : 0;
	}
	return sum;
}

/* randomly shuffle array of size n */
/* implements the fisher-yates shuffle */
void shuffleArr(int * arr, int n) {
	for(int i = n-1; i > 0; i--) {
		int ind = rand() % (i+1);
		int t = arr[i];
		arr[i] = arr[ind];
		arr[ind] = t;
	}
}


int posIsValid(int pos, int l_size, int dir, int orient, int N, int * used) {
	int y, sq;
	for(y = 0; y < l_size; y++) { 
		sq = (orient == 10) ? (pos + (-1*N*dir*y)) : (pos + dir*y);
		if(used[sq]) { return 0; }
	}
	return 1;
}

/*
 * Generates a test board and adds the respective probailities of each ship to
 *  the ships[] vector
 * Input: ships[] where 0 = destroyer, 1=sub OR cruiser (equal prob), 2=bship, 3=carrier
 * 				int N = dimension of the board (default 10)
 *				int n = # boards to generate
 */
void generateBoard(double * ships, int N, int n) {
	
	printf("generating %d boards\n",n);
	int * sizes = allocate_int_vector(5);
	sizes[0] = 2; sizes[1] = 3; sizes[2] = 3;	sizes[3] = 4; sizes[4] = 5; 
	
	int * used;
	used = allocate_int_vector(N*N);
	int i, z, y, w, sq, dir, orient, l_size, pos, u,yy;
	int valid;
	double avgfactor = (1.0/n);
  srand(time(NULL));

		for(i = 0; i < n; i++) {			
		/* place 5 ships */
		shuffleArr(sizes, 5); 
		for(z = 0; z < 5; z++) {
			valid = 0;
			/* select size */
			l_size = sizes[z];
			/* ==> take random direction */
			dir = pow(-1, rand());
			orient = (rand() % 2) ? 1 : 10 ; /* 1 csp to left/right, 10 to up/down */
			/* select position on board */
			/* string together ternary operator for bounds checking */
			while(!valid) {
					pos = (orient==1)  ? ((dir>0) ? (rand() % (N-l_size+1))*(rand() % N)  : (((l_size-1)+(rand() % (N-l_size+1)))*(rand() % N))) : ((dir>0) ? ((N*(l_size-1)) + (rand() % ((N*N)-(N*(l_size-1))))) : (rand() % (N*N-(N*l_size-1)))); 
				
				valid = posIsValid(pos, l_size, dir, orient, N, used);
			} /* should be valid now */
			//printf("BOARD i=%d size = %d orient = %d dir = %d pos = %d \n\n",i,l_size,orient,dir,pos);

			for(w = 0; w < l_size; w++) { 
				sq = (orient == 10) ? (pos + (N*-1*dir*w)) :  (pos + (dir*w));
				used[sq] = 1;
				ships[(l_size-2)+(sq*4)] += ( (l_size == 3) ? avgfactor*(1.0/2.0) : avgfactor ); 
			}
		}
		/*for(yy = 0; yy < N*N; yy++) {
			if(yy % 10 == 0) printf("\n");
			printf(" %d ", used[yy]);
		} printf("\n");*/
		memset(used,0,N*N*sizeof(int));
	}
	free(used);
	free(sizes);
}

/*
 * 	int[] k = information known about the board
 *   k[i] = {-1:unknown, 0:miss, 1:hit, 2-5 ships}
 */
int posIsValidK(int pos, int l_size, int dir, int orient, int N, int * used, int * k) {
	int y, sq;
	for(y = 0; y < l_size; y++) { 
		sq = (orient == 10) ? (pos + (-1*N*dir*y)) : (pos + dir*y);
		if((sq < 0) || (sq > ((N*N)-1))) { return 0; } // off board
		if( (used[sq] && (k[sq] < 1)) || (k[sq]==0)) { return 0; } // if used already or a miss
		if( (orient == 1) && ((((int)pos)/10) != (((int)sq)/10)) ) { // sideways and next sq off row
			return 0;
		}
	}
	return 1;
}

/*
 * Generates a test board and adds the respective probailities of each ship to
 *  the ships[] vector
 * Input: ships[] where 0 = destroyer, 1=sub OR cruiser (equal prob), 2=bship, 3=carrier
 * 				int N = dimension of the board (default 10)
 *				int n = # boards to generate
 * 				int[] k = information known about the board
 *   k[i] = {-1:unknown, 0:miss, 1:hit, 2-5 ships}
 */
void generateBoardWithK(double * shipProbK, int N, int n, int * k) {

	int * used;
	used = allocate_int_vector(N*N);
	int * used_known;
	used_known = allocate_int_vector(N*N);
	int i, z, y, w, sq, dir, orient, l_size, pos, h, yy;
	int MAX_SHIP = 5;
	int shps = 5;
	int valid;
	double avgfactor = (1.0/n);
  	srand(time(NULL));


	int q, hts;
	int s_siz = 0;
	int nhits = 0;
	int sizz[5] = {2,3,3,4,5}; 
	int hitpts[17] = {0}; 
	
	/* have to determine ship length to know which ships still to place */		
	for(q = 0; q < (N*N); q++) {
		if(k[q] > 1) { 
			// find rest of neighbors to detemrine size
			// only two cases to check since we iterate linearly: q+1, q+N
			s_siz = 1;
			used_known[q] = 1;
			
			dir = (k[q+1] == k[q]) ? 1 : 10; // if neighbor same ship, horizontal; otherwise diag
			for(h = 1; h < MAX_SHIP-1; k++) {
				if(k[q + h*dir] != k[q]) { break; }
				s_siz += 1; 
				used_known[q + h*dir] = 1;
			}
			if(s_siz > 1) {
				shps--; 
				if(s_siz == 2) { sizz[0] = -1; }
				else if(s_siz == 3) { if(sizz[1] == -1) { sizz[2] = -1; } else { sizz[1] = -1; } }
				else if(s_siz == 4) { sizz[3] = -1; }
				else if(s_siz == 5) { sizz[4] = -1; }
			}

		} 
		if(k[q] == 1) { hitpts[nhits] = q; nhits++; } 
	}			

	int * sizes = allocate_int_vector(shps);
	/* done explcitly to make shuffling easier - one alloc v n */
	for(h = 0; h < MAX_SHIP; h++) {
		if(sizz[h] != -1) {
			if(h >= shps) {sizes[h-(MAX_SHIP-shps)] = sizz[h];} else {sizes[h] = sizz[h];} }
	}
	
	for(i = 0; i < n; i++) {	
		memcpy(used, used_known, N*N*sizeof(int)); // get the known board
		shuffleArr(sizes, shps); 
		hts = nhits;
		for(z = 0; z < shps; z++) {
			
			valid = 0;
			l_size = sizes[z];

			while(!valid) {
				dir = pow(-1, rand());
				orient = (rand() % 2) ? 1 : 10 ; 
				
				if(hts) { //assign random part of ship the point
					pos = hitpts[hts-1] - (orient*(rand() % l_size));
					dir = (orient == 10) ? -1 : 1;
				}
				else {
					pos = (orient==1)  ? ((dir>0) ? (rand() % (N-l_size+1))*(rand() % N)  : (((l_size-1)+(rand() % (N-l_size+1)))*(rand() % N))) : ((dir>0) ? ((N*(l_size-1)) + (rand() % ((N*N)-(N*(l_size-1))))) : (rand() % (N*N-(N*l_size-1)))); 
				}
				valid = posIsValidK(pos, l_size, dir, orient, N, used, k);
				if(hts && valid) { hts--; }
			} /* should be valid now */

			//printf("K-Board i=%d size = %d orient = %d dir = %d pos = %d \n\n",i,l_size,orient,dir,pos);
			for(w = 0; w < l_size; w++) { 
				sq = (orient == 10) ? (pos + (N*-1*dir*w)) :  (pos + (dir*w));
				used[sq] = 1;
				shipProbK[(l_size-2)+(sq*4)] += ( (l_size == 3) ? avgfactor*(1.0/2.0) : avgfactor ); 
			}
		}
		/*
		for(yy = 0; yy < N*N; yy++) {
			if(yy % 10 == 0) printf("\n");
			printf(" %d ", used[yy]);
		} printf("\n");*/
		memset(used,0,N*N*sizeof(int));
		
	}
	free(used_known);
	free(used);
	free(sizes);
}

/*
 * k is a vector that holds a board state on it
 * Contains: hits, misses, ships 
 * N is the dimension of the board
*/
void generateIncompleteBoard(int * k, int N) {
	srand(time(NULL));
	int n_miss, n_hit;
	int n_ship; /* Want to place randomly, according to rules in board generation */
	int pos;
	
	n_miss = rand() % N;
	n_hit = rand() % 5; /* only 2+3+3+4+5=17 possible hits */
	n_ship = rand() % 3; /* Only 5 to hit */
	
	int i,j,m;
	for(i = 0; i < n_miss; i++) {
		pos = rand() % (N*N);
		k[pos] = 0;
	}
	
	for(j = 0; j < n_hit; j++) {
		pos = rand() % (N*N);
		k[pos] = 1;
	}
}

void genPlayerBoard(double * board, int N) {
	
	int * sizes = allocate_int_vector(5);
	sizes[0] = 2; sizes[1] = 3; sizes[2] = 3;	sizes[3] = 4; sizes[4] = 5; 
	
	int * used;
	used = allocate_int_vector(N*N);
	int i, z, y, w, sq, dir, orient, l_size, pos, u,yy;
	int valid;
  srand(time(NULL));

	
	  /* place 5 ships */
		shuffleArr(sizes, 5); 
		for(z = 0; z < 5; z++) {
			valid = 0;
			/* select size */
			l_size = sizes[z];
			/* ==> take random direction */
			dir = pow(-1, rand());
			orient = (rand() % 2) ? 1 : 10 ; /* 1 csp to left/right, 10 to up/down */
			/* select position on board */
			/* string together ternary operator for bounds checking */
			while(!valid) {
					pos = (orient==1)  ? ((dir>0) ? (rand() % (N-l_size+1))*(rand() % N)  : (((l_size-1)+(rand() % (N-l_size+1)))*(rand() % N))) : ((dir>0) ? ((N*(l_size-1)) + (rand() % ((N*N)-(N*(l_size-1))))) : (rand() % (N*N-(N*l_size-1)))); 
				
				valid = posIsValid(pos, l_size, dir, orient, N, used);
			} 
			
			/* debugging - should be valid now */
			printf("BOARD i=%d size = %d orient = %d dir = %d pos = %d \n\n",i,l_size,orient,dir,pos);

		}
		
		/* FOR DEBUGGING: PRINT BOARD */
		for(yy = 0; yy < N*N; yy++) {
			if(yy % 10 == 0) printf("\n");
			printf(" %d ", used[yy]);
		} printf("\n");
	
	free(used);
	free(sizes);
	
	
	
	
}
