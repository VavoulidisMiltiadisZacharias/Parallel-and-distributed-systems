/* 
*   Matrix Market I/O example program
*
*   Read a real (non-complex) sparse matrix from a Matrix Market (v. 2.0) file.
*   and copies it to stdout.  This porgram does nothing useful, but
*   illustrates common usage of the Matrix Matrix I/O routines.
*   (See http://math.nist.gov/MatrixMarket for details.)
*
*   Usage:  a.out [filename] > output
*
*       
*   NOTES:
*
*   1) Matrix Market files are always 1-based, i.e. the index of the first
*      element of a matrix is (1,1), not (0,0) as in C.  ADJUST THESE
*      OFFSETS ACCORDINGLY offsets accordingly when reading and writing 
*      to files.
*
*   2) ANSI C requires one to use the "l" format modifier when reading
*      double precision floating point numbers in scanf() and
*      its variants.  For example, use "%lf", "%lg", or "%le"
*      when reading doubles, otherwise errors will occur.
*/

#include <stdio.h>
#include <stdlib.h>
#include "mmio.h"
#include <stdint.h>
#include <time.h>
#include <sys/time.h>


void coo2csc(
  uint32_t       * const row,       /*!< CSC row start indices */
  uint32_t       * const col,       /*!< CSC column indices */
  uint32_t const * const row_coo,   /*!< COO row indices */
  uint32_t const * const col_coo,   /*!< COO column indices */
  uint32_t const         nnz,       /*!< Number of nonzero elements */
  uint32_t const         n,         /*!< Number of rows/columns */
  uint32_t const         isOneBased /*!< Whether COO is 0- or 1-based */
) {

  // ----- cannot assume that input is already 0!
  for (uint32_t l = 0; l < n+1; l++) col[l] = 0;


  // ----- find the correct column sizes
  for (uint32_t l = 0; l < nnz; l++)
    col[col_coo[l] - isOneBased]++;

  // ----- cumulative sum
  for (uint32_t i = 0, cumsum = 0; i < n; i++) {
    uint32_t temp = col[i];
    col[i] = cumsum;
    cumsum += temp;
  }
  col[n] = nnz;
  // ----- copy the row indices to the correct place
  for (uint32_t l = 0; l < nnz; l++) {
    uint32_t col_l;
    col_l = col_coo[l] - isOneBased;

    uint32_t dst = col[col_l];
    row[dst] = row_coo[l] - isOneBased;

    col[col_l]++;
  }
  // ----- revert the column pointers
  for (uint32_t i = 0, last = 0; i < n; i++) {
    uint32_t temp = col[i];
    col[i] = last;
    last = temp;
  }

}



	/*Function that calculates the numebr of triangles
	  of a graph given csc sparce matrix format 	
	  CAREFUL!!! This functions count 3 x realNumberOfTriangles
	  So i just divide by 3 within the returned value
	*/
	int triangles_counting(int *row, int* col, int N)
	{
	
		int c = 0; //Need this during searching procedure 
		int x = 0; //Search for this element
		
		//Start - End bounds
		int a = 0;
		int b = 0;
	
		//triangles counter
		int triangles = 0;
		
		for(int i = 0 ; i < N ; i++)
		{
			c = col[i+1] - col[i];
			//printf("c:%d\n", c);
			
			for(int j = 1 ; j < c ; j++)
			{
				x = row[j-1+col[i]];
				for(int m = 0 ; m < c-j ; m++)
				{
					a = col[row[col[i]+j+m]]; //start
					b = col[row[col[i]+j+m]+1]; //end
					//printf("i:%d  c:%d  j:%d  x:%d  a:%d  b:%d\n", i,c,j,x,a,b);
					for(int k = a ; k < b ; k++)
					{
						if(row[k] == x)
							triangles++;
					}
				}
			}
		}
		return triangles;
	}
	


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

int main(int argc, char *argv[])
{
    int ret_code;
    MM_typecode matcode;
    FILE *f;
    int M, N, nz;   
    int i, k, h, r,*I, *J;
	
	struct timeval begin, end;
    double runtime;
    
    gettimeofday(&begin, 0);  
		
    if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
		exit(1);
	}
    else    
    { 
        if ((f = fopen(argv[1], "r")) == NULL) 
            exit(1);
    }

    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }


    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */

    if (mm_is_complex(matcode) && mm_is_matrix(matcode) && 
            mm_is_sparse(matcode) )
    {
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
        exit(1);
    }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) !=0)
        exit(1);


    /* reseve memory for matrices */

    I = (int *) malloc(nz * sizeof(int));
    J = (int *) malloc(nz * sizeof(int));
   


    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    for (i=0; i<nz; i++)
    {
        fscanf(f, "%d %d\n", &I[i], &J[i]);
        I[i]--;  /* adjust from 1-based to 0-based */
        J[i]--;
    }
	
    if (f !=stdin) fclose(f);

    /************************/
    /* now write out matrix */
    /************************/

    mm_write_banner(stdout, matcode);
    mm_write_mtx_crd_size(stdout, M, N, nz);
    
    uint32_t* col = (uint32_t*)malloc(((N+1)*sizeof(uint32_t)));
    uint32_t* row = (uint32_t*)malloc((2*nz*sizeof(uint32_t)));
    
    
    //Create "full" arrays to create full csc format(and not half of it)
    I = (int*)realloc(I, sizeof(int)*2*nz);
    J = (int*)realloc(J, sizeof(int)*2*nz);
    for(int i = nz; i < 2*nz;  i++)
    {
        I[i] = J[i-nz];
    }
    
    for(int i = nz; i< 2*nz; i++)
    {
        J[i] = I[i-nz];
    }
   
   
   	//Convert initial format to csc format
    coo2csc(row, col, J, I, 2*nz, N, 0);   
    
    
    //Call the function to count the triangles
 	int triangles = triangles_counting(row, col ,N);
 	printf("\n");
 	printf("Triangles: %d\n", triangles/3); //divide by 3
 	
 	
 	gettimeofday(&end, 0);
 	runtime = measureTime(begin, end);
 	printf("Time taken: %lf seconds\n", runtime);
 	
	//Free memory
    free(I);
    free(J);
    free(row);
    free(col);
	return 0;
}
