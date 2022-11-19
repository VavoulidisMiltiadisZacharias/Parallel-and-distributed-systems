#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>


/*Point
*
* @int id: The id of each point
* @double dimensions: The number of dimensions of the point 
* @double *coordinates: An array that holds the coordinates of the points
* @double distance: The distance from vantage point 
*
*/
struct point 
{
	int id;
	int dimensions;
	double *coordinates;
	double distance;
};

/*
* Function that generates random doubles in [0 1] range in C (found it * on web in the following link:
 https://stackoverflow.com/questions/55766058/how-can-i-generate-random-doubles-in-c)
*
*/
double rd() 
{
    u_int64_t r53 = ((u_int64_t)(rand()) << 21) ^ (rand() >> 2);
    return (double)r53 / 9007199254740991.0; // 2^53 - 1s
}

/*
* Function that swaps values between two variables
*/
void swap(struct point *p1 , struct point *p2)
{
    //Local variables
    struct point temp ;

    temp = *p2 ;
    *p2 = *p1 ;
    *p1 = temp ;
}

/*
* Function that allocates memory for "n" points
*/
struct point *allocateArray(int n)
{
	//Variables
    struct point *points;
    
    points = (struct point*)malloc(n*sizeof (struct point));

    //Checking memory allocation
    if(points == NULL)
    {
        printf("Something has gone totally wrong with memory allocation !!!\n\n\n") ;
        return NULL;
    }
    else
        printf("Memory allocation has been successfully done \n\n\n") ;

    return points ;
}

/*
* Function that initializes the points
*/
void initializeArray(struct point* points, int n, int dimensions)
{
	//Local variables
	int i;
	
	//Set the id for each point
	for(int i = 0 ; i < n ; i++)
	{
		points[i].id = i;
	}
	
	//Set the dimension for each point
	for(int i = 0 ; i < n ; i++)
	{
		points[i].dimensions = dimensions;
		points[i].distance = 0;
	}
	
	//Allocate memory for all the points' coordinates
	for(int i = 0 ; i < n ; i++)
	{
		points[i].coordinates = (double*)malloc(points[i].dimensions * sizeof(double));
	}
	
	//Set coordinates for every point
	for(int i = 0 ; i < n ; i++)
	{
		for(int j = 0 ; j < points[i].dimensions ; j++)
		{
			points[i].coordinates[j] = rd();	
		}
	}
	printf("Initialization has been successfully done \n\n\n");
}

/*
* Function that prints the coordinates of the points
*/
void printCoordinates(struct point* points, int n, int dimensions)
{
	//Local variables
	int i ;
	
	//Print coordinates for each point
	for(int i = 0 ; i < n ; i++)
	{
		printf("Array element: %d \n", i);
		printf("Point's id: %d \n", points[i].id);
		for(int j = 0 ; j < points[i].dimensions ; j++)
		{
			printf("coordinate[%d]: %.7lf\n",j, points[i].coordinates[j]);
		}
		printf("\n");
	}
}

/*
* Function that caclulates the distance of all points from vantage point
*/
void calculateDistances(struct point* points, int n, int dimensions)
{
	//Local variables
	int i, j;
		
	/*
	*Assume vantage point as the last one and calculate squares of 	 		*distances from it 
	*/
	for(int i = 0 ; i < n-1 ; i++)
	{
		for(int j = 0 ; j < points[i].dimensions ; j++)
		{
			//Calculate sum of squares
			points[i].distance += (points[n-1].coordinates[j]-points[i].coordinates[j])*(points[n-1].coordinates[j]-points[i].coordinates[j]);
		}
	}
	
	
	/*
	* Calculate the square root of the distances (just to comply with 		* the given Matlab code).  In reality there is no need cause the 		* result will be the same either way
	*/
	for(int i = 0 ; i < n-1 ; i++)
	{
			points[i].distance = sqrt(points[i].distance);
	}
}

/*
* Function that resets the distances of the points to zero
*/
void resetDistances(struct point* points, int n, int dimensions)
{
	//Local variables
	int i;
	
	//Reset distances
	for(int i = 0 ; i < n ; i++)
	{
		points[i].distance = 0;
	}
}

/*
* Function that swaps last and j-th element and caclulates the
* distances drom new vantage point
*/
void swapElementsAndCalculateDistances(struct point* points, int n, int dimensions, int j)
{	
	swap(&points[n-1], &points[j]);
	calculateDistances(points, n, dimensions);
}

/*
* Function that prints the distances of the points from vantage point
*/
void printDistances(struct point* points, int n, int dimensions)
{
	//Local variables
	int i;
	
	//Print distances
	printf("\n");
	for(int i = 0 ; i < n ; i++)
	{
		printf("Array element: %d \n", i);
		printf("Point's id:  %d \n", points[i].id);
		printf("distance: %.7lf\n", points[i].distance);
		printf("\n");
	}
}

//---------------------QUICKSELECT START -------------------------//

/*
* Quickselect is an algorithm that finds the k-th smallest element in an
* unordered list.   
*	1st case: 
*		n = even:
*			k-th smallest = n/2
*
*	2nd case
*		n = odd:
*			k -th smallest = (n+1)/2
*
*/

// Standard partition process of QuickSort().
// It considers the last element as pivot
// and moves all smaller element to left of
// it and greater elements to right
int partition(struct point* points, int l, int r)
{
	struct point x = points[r]; //Vantage point 
	int i = l;
	
    for (int j = l; j <= r - 1; j++) 
    {
        if (points[j].distance <= x.distance)
        {
            swap(&points[i], &points[j]);
            i++;
        }
    }
    swap(&points[i], &points[r]);
    return i;
}

// This function returns k'th smallest 
// element in points[l..r] using QuickSort 
// based method.  ASSUMPTION: ALL ELEMENTS
// IN *points ARE DISTINCT
struct point *kthSmallest(struct point* points, int l, int r, int k)
{
    // If k is smaller than number of 
    // elements in array
    if (k > 0 && k <= r - l + 1) 
    {
        // Partition the array around last 
        // element and get position of pivot 
        // element in sorted array
        int index = partition(points, l, r);
  
        // If position is same as k
        if (index - l == k - 1)
            return &points[index];
  
        // If position is more, recur 
        // for left subarray
        if (index - l > k - 1) 
            return kthSmallest(points, l, index - 1, k);
  
        // Else recur for right subarray
        return kthSmallest(points, index + 1, r, 
                            k - index + l - 1);
    }
  
    // If k is more than number of 
    // elements in array
    //return INT_MAX;
} 


//---------------------QUICKSELECT END -------------------------//


/*
* Function that sets the inner set
*
* @struct point* points: All the points
* @int n: Number of points
* @double median: The median of all the points  
*
*/
struct point *setInnerSet(struct point* points, int n, double median)
{
	//Set of points that their distance from VP is lesser than medians
	struct point *innerSet;
	
	if(n%2 ==0)
	{
		//Allocate memory for the inner set
		innerSet = allocateArray(n/2);
		
		int j = 0; //innerSet counter
		
		//Set innerSet
		for(int i = 0 ; i < n ; i++)
		{
			if(points[i].distance < median)
			{
				innerSet[j] = points[i];
				j++;
			}
		}
	}	
	else
	{
		//Allocate memory for inner set (consider Vp to be within innerSet)
		innerSet = allocateArray((n+1)/2);
		
		int j = 0; //innerSet counter
		
		for(int i = 0 ; i < n ; i++)
		{
			if(points[i].distance <= median)
			{
				innerSet[j] = points[i];
				j++;
			}
		}	
	}
	return innerSet;
}

/*
* Function that sets the outer set
*
* @struct point* points: All the points
* @int n: Number of points
* @double median: The median of all the points  
*
*/
struct point *setOuterSet(struct point* points, int n, double median)
{
	//Set of points that their distance from VP is greater than medians
	struct point *outerSet;
	
	if(n%2 ==0)
	{
		//Allocate memory for the outer set
		outerSet = allocateArray(n/2);
		
		int j = 0; //outerSet counter
		
		//Set outerSet
		for(int i = 0 ; i < n ; i++)
		{
			if(points[i].distance > median)
			{
				outerSet[j] = points[i];
				j++;
			}
		}
	}	
	else
	{
		//Allocate memory for outer set
		outerSet = allocateArray((n-1)/2);
		
		int j = 0; //innerSet counter
		
		for(int i = 0 ; i < n ; i++)
		{
			if(points[i].distance > median)
			{
				outerSet[j] = points[i];
				j++;
			}
		}	
	}
	return outerSet;
}


/*
* Function that prints the inner and outer set
*/
void printSets(int n, struct point* innerSet, struct point* outerSet)
{
	if(n%2 == 0)
	{
		printf("Inner set:\n");
		for(int i = 0 ; i < n/2 ; i++)
		{
			printf("Inner set element: %d \n", i);
			printf("Point's id:  %d \n", innerSet[i].id);
			printf("distance: %.7lf\n", innerSet[i].distance);
			printf("\n");
		}
		
		printf("Outer set:\n");
		for(int i = 0 ; i < n/2 ; i++)
		{
			printf("Outer set element: %d \n", i);
			printf("Point's id:  %d \n", outerSet[i].id);
			printf("distance: %.7lf\n", outerSet[i].distance);
			printf("\n");
		}
	}
	else
	{
		printf("Inner set:\n");
		for(int i = 0 ; i < n/2+1 ; i++)
		{
			printf("Inner set element: %d \n", i);
			printf("Point's id:  %d \n", innerSet[i].id);
			printf("distance: %.7lf\n", innerSet[i].distance);
			printf("\n");
		}
		
		printf("Outer set:\n");
		for(int i = 0 ; i < n/2 ; i++)
		{
			printf("Outer set element: %d \n", i);
			printf("Point's id:  %d \n", outerSet[i].id);
			printf("distance: %.7lf\n", outerSet[i].distance);
			printf("\n");
		}
	}
}

/* Node
*
* @struct point: The point that the node represents
* @struct node* left: Pointer to the left child of the node
* @struct node* right: Pointer to the right child of the node
* @int height: The height of the node
*
*/
struct Node 
{
    struct point* point;
    struct Node* left; 
    struct Node* right;
    int height;
};

/*
* Function that caclulates the height of a node
*/
int height(struct Node *N) 
{
  	if (N == NULL)
    	return 0;
  	return N->height;
}

/*
* Function that finds the max between two integers
*/
int max(int a, int b) 
{
  	return (a > b) ? a : b;
}

/*
* Function that creates a node
*/
struct Node *newNode(struct point *point) 
{
  	struct Node *node = (struct Node *)malloc(sizeof(struct Node));
  	node->point = point;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

/*
* Function that implements right rotation for the tree
*/
struct Node *rightRotate(struct Node *y) 
{
	struct Node *x = y->left;
  	struct Node *T2 = x->right;

  	x->right = y;
  	y->left = T2;

  	y->height = max(height(y->left), height(y->right)) + 1;
  	x->height = max(height(x->left), height(x->right)) + 1;

  	return x;
}

/*
* Function that implements left rotation for the tree
*/
struct Node *leftRotate(struct Node *x) 
{
  	struct Node *y = x->right;
  	struct Node *T2 = y->left;

  	y->left = x;
  	x->right = T2;

  	x->height = max(height(x->left), height(x->right)) + 1;
  	y->height = max(height(y->left), height(y->right)) + 1;

  	return y;
}

/*
* Function that gets the balance factor for the tree
*/
int getBalance(struct Node *N) 
{
  	if(N == NULL)
    	return 0;
  	return height(N->left) - height(N->right);
}

/*
* Function that inserts a node to the tree
*/
struct Node *insertNode(struct Node *node, struct point *point) 
{
  	// Find the correct position to insertNode the node and insertNode it
	if(node == NULL)
		return newNode(point);

	if(point->distance < node->point->distance)
	    node->left = insertNode(node->left, point);
  	else if(point->distance > node->point->distance)
    	node->right = insertNode(node->right, point);
  	else
    	return node;

  // Update the balance factor of each node and
  // Balance the tree
  	node->height = 1 + max(height(node->left),
               height(node->right));

  	int balance = getBalance(node);
  	if(balance > 1 && point->distance < node->left->point->distance)
    	return rightRotate(node);

  	if(balance < -1 && point->distance > node->right->point->distance)
    	return leftRotate(node);

  	if(balance > 1 && point->distance > node->left->point->distance) 
  	{
    	node->left = leftRotate(node->left);
    	return rightRotate(node);
 	}

	if(balance < -1 && point->distance < node->right->point->distance) 
	{
    	node->right = rightRotate(node->right);
    	return leftRotate(node);
 	}

  	return node;
}

struct Node *minValueNode(struct Node *node) 
{
	struct Node *current = node;

  	while (current->left != NULL)
    	current = current->left;

  	return current;
}

/*
* Function that creates a balanced binary tree
*/
struct Node* createTree(int n, struct point *innerSet, struct point *outerSet)
{
	struct Node *node = NULL;
	
	if(n%2 == 0)
	{
		for(int i = 0 ; i < n/2 ; i++) //Insert inner set
		{
			node = insertNode(node, &innerSet[i]);
		}
		
		for(int i = 0 ; i <n/2 ; i++) //Insert outer set
		{
			node = insertNode(node, &outerSet[i]);
		}
	}
	
	else
	{
		for(int i = 0 ; i < n/2+1 ; i++) //Insert inner set
		{
			node = insertNode(node, &innerSet[i]);
		}
		
		for(int i = 0 ; i <n/2 ; i++) //Insert outer set
		{
			node = insertNode(node, &outerSet[i]);
		}
	}
	return node;
}

/*
* Function that prints a tree
*/
void printPreOrder(struct Node *node) 
{
	static int i = 0;

  	if(node != NULL)
  	{
  		printf("Node: %d contains point: %d that has distance: %.7lf\n\n", i, node->point->id, node->point->distance);
  		i++;
  		printPreOrder(node->left);
  		printPreOrder(node->right);
  	}
  	
  	if(node == NULL)
  	{
  		printf("Node %d is NULL\n\n", i);
  		i++;
  	}
}

/*
* Function that helps to measure the execution time
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


//MAIN
int main()
{

	//Set the seed for random generated numbers
	srand(time(NULL));
	
	//Variables declaration
	int n; // number of points
	int dimensions; //number of dimensions of points
	int k_th; //k-th smallest point (by distance from vantage point)
	struct point *points; //An array of points
	struct point *kthSmallestPoint; // return of kthSmallest
	double kthSmallestPointDistance; //The median
	struct point *innerSet; //Set of points that their distance from VP is lesser than median
	struct point *outerSet; //Set of points that their distance from VP is greater than median
	
	struct timeval begin, end;
    double runtime;
    
    gettimeofday(&begin, 0); //Begin time
	
	//Initialization
	n = 96;
 	dimensions = 256;
	
	//Allocate memory for points
	points = allocateArray(n);
	
	//Initialize the points' array
	initializeArray(points, n, dimensions);
	
	//Print coordinates (just to check)
	//printCoordinates(points, n, dimensions);	
	
	//Calculate distances from vantage point
	calculateDistances(points, n, dimensions);
	
	//Print distances
	printDistances(points, n , dimensions);
	
	/*
	* Calculate all KNN for j-th iteration
	* Print the to check that it works properly
	* Swap vantage point (n-1) and (j) so that we calculate
	* next's all kNNs and so on till the last one
	*/
	for(int j = 0 ; j < n ; j++)
	{
		printf("\n\n");
		printf("Iteration: %d\n", j);
		
		//Select all kNN of vantage point using "quickselect" algorithm
		for(int i = 0 ; i < n ; i++)
		{
			if(n%2 == 0)
			{
				k_th = i;
				kthSmallestPoint = kthSmallest(points, 0, n, k_th+1);
				kthSmallestPointDistance = kthSmallestPoint->distance;
			}
			else
			{
				k_th = i;
				kthSmallestPoint = kthSmallest(points, 0, n, k_th);
				kthSmallestPointDistance = kthSmallestPoint->distance;
			}
			
			printf("%d smallest point's id is %d with distance: %.7lf \n", k_th, kthSmallestPoint->id, kthSmallestPointDistance);
		}
		
		//Reset distances
		resetDistances(points, n, dimensions);
		
		//Swap elements (points[n-1] and points[j])and re-calculate distances from new vantage point
		swapElementsAndCalculateDistances(points, n, dimensions, j);
	}
	
	//Set the inner and outer set
	//innerSet = setInnerSet(points, n, median);
	//outerSet = setOuterSet(points, n, median);
	
	//Print the inner and outer set
	//printSets(n, innerSet, outerSet);
	
	
	//Create the root node of the tree
	//struct Node *node = NULL;
	
	//Create the tree
	//node = createTree(n, innerSet, outerSet);


	gettimeofday(&end, 0); //End time
	
 	runtime = measureTime(begin, end); //Measure execution time
 	
 	printf("\n");
 	printf("Time taken: %lf seconds\n", runtime); //Print execution time	
	
	//Print the tree in preOrder
	//printPreOrder(node);
	
	
	printf("\n");
	return 0;
}
