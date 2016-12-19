#ifdef PARALLEL
#include <mpi.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "ship.c"
#include "player.c"
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
	
	
	
#ifdef PARALLEL	
  MPI_Finalize();
#endif

  return 0;
}

	