#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>


//Function that returns the elapsed time
double measureTime(struct timeval begin, struct timeval end) 
{
   	long seconds;
   	long microseconds;
	double elapsed;
	seconds = end.tv_sec - begin.tv_sec;
	microseconds = end.tv_usec - begin.tv_usec;
	elapsed = seconds + microseconds * 1e-6;	
		
	return elapsed;
}


/*Function that updates the lattice
*
* @param1: The lattice
* @param2: The lattice
* @param3: The size of the lattice (nxn)
* @param4: The iteration counter
* @param5: How many times the lattice will be updated
*
*/
_global_void updateLattice(int *evenLattice, int *oddLattice, int n, int k, int limit)
{
	while(k < limit)
    {
    	printf("\n k: %d\n\n", k+1);
    	 //Find next round's lattice (up+right+down+left+itself)
		for(int i = 0 ; i < n ; i++)
	   	{
			for(int j = 0 ; j < n ; j++)
			{
			
				if(k%2 == 0)//round is even -> read from even, write to odd
				{
					if((evenLattice[((i+n-1)%n)*n+j]+
									evenLattice[i*n+(j+1)%n] +
									evenLattice[((i+1)%n)*n+j]+
									evenLattice[(i*n+(j+n-1)%n)]+
									evenLattice[i*n+j]) > 0)
						oddLattice[i*n+j] =  1;
					else
						oddLattice[i*n+j] =  -1;
					
					//print the correct lattice	just to check
					if(oddLattice[i*n+j] >0)
						printf(" "); //To print them alligned
					printf(" %d", oddLattice[i*n+j]);				
					
				} 
					
    			
				else //round is odd -> read from odd, write to even
				{
					if((oddLattice[((i+n-1)%n)*n+j]+
								oddLattice[i*n+(j+1)%n] +
								oddLattice[((i+1)%n)*n+j]+
								oddLattice[(i*n+(j+n-1)%n)]+
								oddLattice[i*n+j]) > 0)
					evenLattice[i*n+j] = 1;
					else
						evenLattice[i*n+j] = -1;
						
					//print the correct lattice	just to check		
					if(evenLattice[i*n+j] >0)
						printf(" "); //To print them alligned
					printf(" %d", evenLattice[i*n+j]);
				}		
			}
			printf("\n"); //Print to the next line
		}
		k++; //Go to next round
		printf("\n");
	}
	_syncthreads();
}

//MAIN
int main()
{
	
	//Variables declaration
	int *evenLatticeHost; //Read only at even rounds (nxn size). This is the initial lattice too for this example
	int *evenLatticeDevice;
	
	int *oddLatticeHost; //Read only at odd rounds (nxn size)
	int *evenLatticeDevice;
	
	int n;
	int check; //Need to fill the initial lattice
	int k; //Just to know from which lattice to read and where should i write
	int limit; //How many time will the lattice be updated
	
	//Cuda variables
	int numberOfBlocks;
	int numberOfThreads;
	
	//These variables are used to emasure execution time
	struct timeval begin, end;
    double runtime;
	
	//Initalization
	n = 360;
	k = 0;
	limit = 200;
	numberOfBlocks = n;
	numberOfThreads = 1;
	
	//Allocate memory for the lattices
	_shared_evenLatticeHost = (int*)malloc(n*n *sizeof(int));
	_shared_oddLatticeHost = (int*)malloc(n*n *sizeof(int));
	
	//Allocate memory for cuda arrays
	cudaMalloc((void**)&evenLatticeDevice, n*n*sizeof(int))
	cudaMalloc((void**)&oddLatticeDevice, n*n*sizeof(int))
	
	//Fill the evenLattice with values (initial state)
    for(int i = 0 ; i < n ; i++)
    {
    	for(int j = 0 ; j < n ; j++)
    	{
    		check = rand() % 10;
    		if(check < 5)
    			evenLatticeHost[i*n+j] = 1;
    		else
    			evenLatticeHost[i*n+j] = -1;
    	}
    }
	

	/*Copy data from host to device
	*
	* Need to copy both arrays so that the device will be able to
	* handle both of them
	*/
	cudaMemcpy(evenLatticeDevice, evenLatticeHost, sizeof(int)*(n*n), cudaMemcpyHostToDevice);
	cudaMemcpy(oddLatticeDevice, oddLatticeHost, sizeof(int)*(n*n), cudaMemcpyHostToDevice);
	
	
	
	printf("Initial lattice is:\n");
    //Print the evenLattice to check if it was filled properly
    for(int i = 0 ; i < n ; i++)
    {
    	for(int j = 0 ; j < n ; j++)
    	{
    		if(evenLatticeHost[i*n+j] == 1)
    			printf(" "); //To print them alligned
    		printf(" %d", evenLatticeHost[i*n+j]);
    	}
    	printf("\n");
     }
	
	//Call the function to demonstrate the update of the lattice
    updateLattice(evenLatticeHost, oddLatticeHost, n, k, limit);
	
	//Start time
    gettimeofday(&begin, 0); 
    
	//Call the cuda function (1 block, 1 thread per block)
	updateLattice<<<numberOfBlocks ,numberOfThreads>>> (evenLatticeDevice, oddLatticeDevice, n, k, limit) 
	
	//End time
    gettimeofday(&end, 0);
    
    runtime = measureTime(begin, end);
 	printf("\n");
 	printf("The execution time is: %.3lf seconds\n\n", runtime);
	
	//Free the arrays
	free(evenLatticeHost);
	free(oddLatticeHost);
	
	cudaFree(evenLatticeDevice);
	cudaFree(oddLatticeDevice);
	
	
	return 0;
}











