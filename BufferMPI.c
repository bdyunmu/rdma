/*
 * BenchmarkOMPI.c
 *
 *  Created on: 28 Sep 2017
 *      Author: huili@ruijie.com.cn
 */

/* Benchmark of Isend Irecv with Open MPI*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_WIN_SIZE 64

int main(int argc, char *argv[])
{
	if(argc!=3){
		printf("usage %s [count] [iter]\n",argv[0]);
		exit(-1);
	}

	int BUFSIZE = atoi(argv[1]);
	int ITER = atoi(argv[2]);

	printf("bs %d iter %d\n",BUFSIZE,ITER);

	char *buf = (char *)malloc(MAX_WIN_SIZE*BUFSIZE*sizeof(char));
	int i, j, r,myrank, numprocs;
	
	MPI_Status status;
	MPI_Request requests[MAX_WIN_SIZE];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	double t1 = MPI_Wtime();

	for(r = 0; r<ITER; r++)
	{
		for(i = 0; i<MAX_WIN_SIZE; i++){
			if (myrank != 0){
				MPI_Isend(buf+i*BUFSIZE, BUFSIZE, MPI_CHAR, 0, 99+i, MPI_COMM_WORLD,&requests[i]);
			}
			else {
				for (j=1; j<numprocs; j++) {
				MPI_Irecv(buf+i*BUFSIZE, BUFSIZE, MPI_CHAR, j, 99+i, MPI_COMM_WORLD,&requests[i]);
				}
			}
		}//for i
		MPI_Waitall(MAX_WIN_SIZE, &requests[0], MPI_STATUSES_IGNORE);
	}//for r

	double t2 = MPI_Wtime();
	double t3 = (t2-t1)/(double)MAX_WIN_SIZE/(double)ITER;

	int Kb = BUFSIZE*sizeof(char)/1024.0;
	double Bw = Kb/t3;
	printf("[%d] get %d Kbytes, %f sec, %f Kb/s\n",myrank,Kb,t3,Bw);
	MPI_Finalize();
	return 0;
}
