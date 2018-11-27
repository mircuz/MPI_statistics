/*
 ============================================================================
 Name        : MPI_Gauss_Sum.c
 Author      : Mirco Meazzo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello MPI World in C 
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include <math.h>


int main( int argc, char** argv ) {

	MPI_Init( &argc, &argv );

	int size, rank;
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	int lim[2];
	lim[0] = 0; lim[1] = 100;
	float arr[ lim[1] ];
	int count = lim[1]/size;
	float subarr[ count ];

	//--------------------Random sequence generator-------------------------------\\
	//-------------Place in the array arr[] the data to be be analized------------\\
	srand(0);
	if (rank == 0) {
		printf("Processor %d is generating the sequence of numbers...\n", rank);
		for (int i =lim[0]; i < lim[1]; ++i ){

			arr[i] = rand() % 100;
		}
		printf("Process completed!!\n");
	}
	//----------------------------------------------------------------------------\\
	//----------------------------------------------------------------------------\\

	// Spread the dataset on all processors
	MPI_Scatter( &arr, count, MPI_INT, subarr, count, MPI_INT, 0, MPI_COMM_WORLD );
	
	// Perform the local mean
	float sum = 0;
	for (int i = 0; i < count; ++i) {
		sum += subarr[i];
	}
	sum = sum / count;

	// Perform the global sum
	float mean;
	MPI_Allreduce( &sum, &mean, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD );

	// Compute the global sum locally on each processor
	mean = mean / size;

	// Only processor 0 prints
	if (rank == 0) {
		printf("\nThe AVERAGE is %f\n", mean);
	}

	// Compute the squared standard deviation on each processor
	float squared_std_dev = 0;
	for (int i = 0; i < count; ++i) {
		squared_std_dev += pow( subarr[i] - mean, 2) ;
	}

	// Compute the std_dev
	float std_dev = 0;
	MPI_Reduce( &squared_std_dev, &std_dev, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD );
	std_dev = sqrt( std_dev /
				(size * count));

	if (rank == 0) {

		printf("The STANDARD DEVIATION is %f\n", std_dev);
	}


	MPI_Finalize();
	return 0;
}
