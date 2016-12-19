#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct {
	
	char name[2]; /* DE, SB, CU, BS, CA */ 
	double * coords; // allocate dynamically
	
} ship; 