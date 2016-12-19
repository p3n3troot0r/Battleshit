#ifdef PARALLEL
#include <mpi.h>
#endif

#include "ship.c"
#include "player.c"
#include "boardwithkb.h"

int main (int argc, char *argv[]) {
	
  int id, np;
 // char name[MPI_MAX_PROCESSOR_NAME];
	
  //MPI_Init (&argc, &argv);
  //MPI_Comm_size (MPI_COMM_WORLD, &np);
  //MPI_Comm_rank (MPI_COMM_WORLD, &id);
	
  /* 
	 * Introduce game
	 * INITIALLY ONLY Comp vs comp ==> Create players
	 *    LATER IF TIME enable pvp
	 * Enable gameplay - each process has boards
	 */
	
	
	
	
//  MPI_Finalize();

  return 0;
}

	
