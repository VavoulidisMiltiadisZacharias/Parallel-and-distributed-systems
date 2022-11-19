#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "mpi.h"


//Global variables
int numberOfPoints = 96;
int dimensions = 256;


/*
* Function that generates random doubles in [0 1] range in C (found it * on web in the following link:
 https://stackoverflow.com/questions/55766058/how-can-i-generate-random-doubles-in-c)
*
*/
double rd() 
{
    u_int64_t r53 = ((u_int64_t)(rand()) << 21) ^ (rand() >> 2);
    return (float)r53 / 9007199254740991.0; // 2^53 - 1s
}

/*
* Function that helps to calculate time
*/
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

/*
* Function that prints an array
*/
void printArray(int *array, int size)
{	
	printf("Array: ");
	
	for(int i = 0; i < size; i++)
	{
		printf("%d ", array[i]);
	}
	
	printf("\n");
}

/*
* Function that swaps two floats
*/
void swap(float *a, float *b)
{
	float temp = *a;
	*a = *b;
	*b = temp;
}


int partition(float *A, int left, int right)
{
    float pivot = A[right];
    int i = left, x;
 
    for (x = left; x < right; x++)
    {
        if (A[x] < pivot)
        {
            swap(&A[i], &A[x]);
            i++;
        }
    }
 
    swap(&A[i], &A[right]);
    
    return i;
}


float quickselect(float *A, int left, int right, int k)
{
 
    //p is position of pivot in the partitioned array
    int p = partition(A, left, right);
 
    //k equals pivot got lucky
    if (p == k-1)
    {
        return A[p];
    }
    //k less than pivot
    else if (k - 1 < p)
    {
        return quickselect(A, left, p - 1, k);
    }
    //k greater than pivot
    else
    {
        return quickselect(A, p + 1, right, k);
    }
}


void quicksort(float *L, int start, int end)
{
    if (start >= end)
        return;
        
    int splitPoint = partition(L, start, end);
    
    quicksort(L, start, splitPoint - 1);
    quicksort(L, splitPoint + 1, end);
}


/*
* Function that calculates euclidean distance
*/
float euclideanDistance(int *point1, int *point2)
{
	int sum = 0;
	
	for(int i = 0; i < dimensions; i++)
	{
		 sum += (point2[i] - point1[i]) * (point2[i] - point1[i]);
	}
	
	return sqrt(sum);
}

/*
* Function that prints points
*/
void printPoints(int **points, int size)
{
	for(int i = 0; i < size; i++)
	{
		printf("Point %d:\t", i);
		
		for(int j = 0; j < dimensions; j++)
		{
			printf("%d ", points[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}


/*
* Function that prints a single point
*/
void printPoint(int *point)
{
	for(int i = 0; i < dimensions; i++)
	{
		printf("%d ",point[i]);
	}
}

/*
* Function that calculates distances
*/
float* calculateDistances(float *distances, int **points, int *vantagePoint, int size)
{
	float median;
	int *rootPoint = (int*)malloc(dimensions*sizeof(int));
	
	for(int i = 0; i < size; i++)
	{
		distances[i] = euclideanDistance(points[i], vantagePoint);
	}
	
	return distances;
}


//------------ TREE INFO START ---------------//
struct node
{
	int *data;
	struct node *left;
	struct node *right;
};



struct node* newNode(int *data)
{
	struct node* node = (struct node*)malloc(sizeof(struct node));
	
	node->data = data;
	node->left = NULL;
	node->right = NULL;
	
	return node;
}



struct node* insert(struct node* root, int *data, int *vantagePoint)
{
	if(root == NULL)
	{
		root = newNode(data);
		return root;
	}
	else if(euclideanDistance(data, vantagePoint) <= euclideanDistance(root->data, vantagePoint))
	{
		root->left = insert(root->left, data, vantagePoint);
	}
	else
	{
		root->right = insert(root->right, data, vantagePoint);
	}
	
	return root;
}



void vpTree(int **points, int *vantagePoint, float *distances, int size)
{
	float median;
	int *rootPoint = (int*)malloc(dimensions*sizeof(int));
	
	if(size % 2 == 0)
	{
		float a = quickselect(distances, 0, size-1, size/2);
		float b = quickselect(distances, 0, size-1, size/2+1);
		median = (a+b)/2;
		
		for(int i = 0; i < size; i++)
		{
			float x = euclideanDistance(points[i], vantagePoint);
			
			for(int j = 0; j < size; j++)
			{
				float y = euclideanDistance(points[j], vantagePoint);
				
				if(((x+y)/2) == median)
				{
					rootPoint = points[i];
				}
			}
		}
	}
	else
	{
		median = quickselect(distances, 0, size-1, size/2+1);
		
		for(int i = 0; i < size; i++)
		{
			if(euclideanDistance(points[i], vantagePoint) == median)
			{
				rootPoint = points[i];
			}
		}
	}
	
	
	struct node *root = newNode(rootPoint);
	
	for(int i = 0; i < size; i++)
	{
		insert(root, points[i], vantagePoint);
	}
}

//------------ TREE INFO END ---------------//

//MAIN
int main()
{

	struct timeval begin, end;	// To count time
    double runtime;	
	
	gettimeofday(&begin, 0);  // Start counting time
	
	int rank, size;
	
	MPI_Status status;
	
	//MPI START
	MPI_Init(NULL, NULL);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	if(rank == 0)
	{
		printf("MPI - rank 0\n\n");
	}
	
	
	int n = numberOfPoints / size;
	
	int **points;
	int *vantagePoint;
	float *distances;
	int **gatheredPoints = NULL;
	float *gatheredDistances = NULL;
	srand(time(0)+rank+1);		// id changes to create different points
	
	
	//Allocate memory
	vantagePoint = (int*)malloc(dimensions*sizeof(int));
	distances = (float*)malloc(n*sizeof(float));
	
	if(rank == size-1)
	{
		n += numberOfPoints - n*size;	// Last processor handles all the remainders 
	}
	
	points = (int**)malloc(n*sizeof(int*));	// Allocation needs to take place here because of numberOfPoints
			
	for(int i = 0; i < n; i++)
	{
		points[i] = (int*)malloc(dimensions*sizeof(int));
	}
		
	//Create points
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < dimensions; j++)
		{
			points[i][j] = rd();
		}
	}
	
	vantagePoint = points[rand() % n];
	
	distances = calculateDistances(distances,points, vantagePoint, n);
	
	if(rank == 0)
	{
		gatheredDistances = (float*)malloc(numberOfPoints*sizeof(float));
		gatheredPoints = (int**)malloc(numberOfPoints*sizeof(int*));	// Allocation needs to take place here because of numberOfPoints
			
		for(int i = 0; i < numberOfPoints; i++)
		{
			gatheredPoints[i] = (int*)malloc(dimensions*sizeof(int));
		}
	}
	
	MPI_Gather(distances, n, MPI_FLOAT, gatheredDistances, n, MPI_FLOAT, 0, MPI_COMM_WORLD);
	
	
	if(rank == 0)
	{	
		vpTree(gatheredPoints, vantagePoint, gatheredDistances, numberOfPoints);
	}
	
	
	//Free all the arrays
	for(int i = 0; i < n; i++)		
	{
		free(points[i]);
	}
	
	free(points);
	free(distances);
	
	//MPI end
	MPI_Finalize();
	
	gettimeofday(&end, 0);  // Stop counting time
	runtime = measureTime(begin, end);
	
	printf("Time taken: %lf seconds\n", runtime); //Print execution time
	
	return 0;
}
