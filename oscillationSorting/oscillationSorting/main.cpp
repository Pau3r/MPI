#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <ctime>

#define TAG_FIRST_SEND_TO_A 322
#define TAG_SEND_TO_B_MORE 644
#define TAG_SEND_TO_A_MORE 47
#define TAG_SEND_TO_B_LESS 966
#define TAG_SEND_TO_A_LESS 52

int main(int argc, char **argv)
{
	int world_rank, world_size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	const int worker_size = world_size - 1;
	MPI_Status status;
	
	int bTypeStart = (world_size / 2) ;
	int *arrayToSort = new int[bTypeStart - 1];
	const int iterations = (bTypeStart-1 /2)  ;

	if (world_rank == 0)
	{
		srand(time(NULL));
		std::cout << "###############################################" << std::endl;
		std::cout << "elements in array: " << std::endl;
		for (int i = 0; i < bTypeStart-1; i++)
		{
			arrayToSort[i] = rand() %100; 
			std::cout << arrayToSort[i] << std::endl;
		}
		std::cout << "###############################################" << std::endl;

		for (int i = 1;i<bTypeStart;i++)
		{
			int numberToSend = arrayToSort[i - 1];
			MPI_Send(&numberToSend, 1, MPI_INT, i, TAG_FIRST_SEND_TO_A, MPI_COMM_WORLD);
		}
		for (int i = 1; i < bTypeStart; i++)
		{
			int receive;
			MPI_Recv(&receive, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			arrayToSort[i - 1] = receive;
		}
		std::cout << "sorted array"<<std::endl;
		for (int i = 0; i<bTypeStart-1;i++)
		{
			std::cout << arrayToSort[i]<<std::endl;
		}
	}
	
	if(world_rank!= 0 && world_rank<bTypeStart)
	{
		int *good = new int[bTypeStart];
		int less;
		int ileRazy = 0;
	
		
		MPI_Recv(&less,1,MPI_INT,0,TAG_FIRST_SEND_TO_A,MPI_COMM_WORLD,&status);
		int more = less;

		for (int i = 0; i < iterations; i++)
		{
			MPI_Send(&more, 1, MPI_INT, world_rank + bTypeStart, TAG_SEND_TO_B_MORE, MPI_COMM_WORLD);
			MPI_Send(&less, 1, MPI_INT, world_rank + bTypeStart - 1, TAG_SEND_TO_B_LESS, MPI_COMM_WORLD);
			
			if (less > more)
			{
				int tmp = more;
				more = less;
				less = tmp;
			}
		
			
			MPI_Recv(&less, 1, MPI_INT, world_rank + bTypeStart, TAG_SEND_TO_A_LESS, MPI_COMM_WORLD, &status);
		    MPI_Recv(&more, 1, MPI_INT, world_rank + bTypeStart - 1, TAG_SEND_TO_A_MORE, MPI_COMM_WORLD, &status);

		}
	
		MPI_Send(&less, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	for (int i = 0; i < iterations; i++)
	{
		if (world_rank != 0 && world_rank >= bTypeStart)
		{
			if (world_rank != bTypeStart && world_rank != worker_size)
			{
				int lower;
				int upper;
				MPI_Recv(&lower, 1, MPI_INT, world_rank - bTypeStart, TAG_SEND_TO_B_MORE, MPI_COMM_WORLD, &status);
				MPI_Recv(&upper, 1, MPI_INT, world_rank - bTypeStart + 1, TAG_SEND_TO_B_LESS, MPI_COMM_WORLD, &status);
				if (lower < upper)
				{
					int tmp = lower;
					lower = upper;
					upper = tmp;
				}
			

				MPI_Send(&upper, 1, MPI_INT, world_rank - bTypeStart, TAG_SEND_TO_A_LESS, MPI_COMM_WORLD);
				MPI_Send(&lower, 1, MPI_INT, world_rank - bTypeStart + 1, TAG_SEND_TO_A_MORE, MPI_COMM_WORLD);



			}
			else if (world_rank == bTypeStart)
			{
				int number;
				MPI_Recv(&number, 1, MPI_INT, world_rank - bTypeStart + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			
				MPI_Send(&number, 1, MPI_INT, world_rank - bTypeStart + 1, TAG_SEND_TO_A_MORE, MPI_COMM_WORLD);


			}
			else if (world_rank == world_size - 1)
			{
				int number;
				MPI_Recv(&number, 1, MPI_INT, world_rank - bTypeStart, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Send(&number, 1, MPI_INT, world_rank - bTypeStart, TAG_SEND_TO_A_LESS, MPI_COMM_WORLD);




			}
		}
	}

	

	MPI_Finalize();
	return 0;
}
