// sortWithWorkers.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <time.h>

void bubbleSort(int array[], int n);

int main(int argc, char **argv)
{

	int world_rank, world_size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	const int arrayLength = world_size * 100;
	int *arrayToSort = new int[arrayLength];
	int chunk = 1;
	int *numberOfElementsToSend = new int[world_size];
	int *displacementOfElements = new int[world_size];


	if (world_rank == 0)
	{
		srand(time(NULL));
		for (int i = 0; i < arrayLength; i++)
		{
			arrayToSort[i] = rand() % 100;
			//std::cout << arrayToSort[i] << std::endl;
		}
		chunk = arrayLength / (world_size - 1);
		for (int i = 0; i < world_size; i++)
		{
			numberOfElementsToSend[i] = (i != 0) ? chunk : 0;
			displacementOfElements[i] = (i - 1) * chunk;
			
		}
	}
	MPI_Bcast(&chunk, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int *recvArray = new int[arrayLength];
	MPI_Scatterv(arrayToSort,
		numberOfElementsToSend,
		displacementOfElements,
		MPI_INT,
		recvArray,
		arrayLength,
		MPI_INT,
		0,
		MPI_COMM_WORLD);

	if (world_rank==1)
	{
		
		bubbleSort(recvArray, chunk);
		/*for (int i = 0; i < chunk; i++)
		{
			std::cout << arrayToSort[i] << std::endl;
		}*/

		for (int i = 1; i < world_size; i++)
		{
			
		}


	}

	MPI_Finalize();
	return 0;
}
void bubbleSort(int array[], int n)
{
	for (int i = 0; i < (n - 1); i++)
	{
		for (int j = 0; j < n - i - 1; j++)
		{
			if (array[j] > array[j + 1]) /* For decreasing order use < */
			{
				int swap = array[j];
				array[j] = array[j + 1];
				array[j + 1] = swap;
			}
		}
	}
}