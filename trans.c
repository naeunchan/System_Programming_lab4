/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	int row;
	int col;
	int inner_r;
	int inner_c;
	int d0 = 0; //diagonal
	int d1,d2,d3,d4,d5,d6,d7;

	if(N == 32)
	{
		for(col = 0; col < N; col+=8)
		{
			for(row = 0; row < N; row+=8)
			{
				for(inner_r = row; inner_r < row+8; inner_r++)
				{
					for(inner_c = col; inner_c < col+8; inner_c++)
					{
						if(inner_r != inner_c)
							B[inner_c][inner_r] = A[inner_r][inner_c];
						else //diagonal part
						{
							d1 = A[inner_r][inner_c];
							d0 = inner_r;		
						}
					}
					if(row == col)
						B[d0][d0] = d1;
				}
			}
		}
	}
	else if(N == 64)
	{
		for(row = 0; row < N; row+=8)
		{
			for(col = 0; col < M; col+=8)
			{
				for(inner_r = col; inner_r < col+4; ++inner_r)
				{
					d0 = A[inner_r][row];
					d1 = A[inner_r][row+1];
					d2 = A[inner_r][row+2];
					d3 = A[inner_r][row+3];
					d4 = A[inner_r][row+4];
					d5 = A[inner_r][row+5];
					d6 = A[inner_r][row+6];
					d7 = A[inner_r][row+7];

					B[row][inner_r] = d0;
					B[row+1][inner_r] = d1;
					B[row+2][inner_r] = d2;
					B[row+3][inner_r] = d3;
					B[row][inner_r+4] = d4;
					B[row+1][inner_r+4] = d5;
					B[row+2][inner_r+4] = d6;
					B[row+3][inner_r+4] = d7;
				}
				for(inner_r = row+4; inner_r < row+8; ++inner_r)
				{
					d0 = B[inner_r-4][col+4];
					d1 = B[inner_r-4][col+5];
					d2 = B[inner_r-4][col+6];
					d3 = B[inner_r-4][col+7];

					B[inner_r-4][col+4] = A[col+4][inner_r-4];
					B[inner_r-4][col+5] = A[col+5][inner_r-4];
					B[inner_r-4][col+6] = A[col+6][inner_r-4];
					B[inner_r-4][col+7] = A[col+7][inner_r-4];

					B[inner_r][col] = d0;
					B[inner_r][col+1] = d1;
					B[inner_r][col+2] = d2;
					B[inner_r][col+3] = d3;

					B[inner_r][col+4] = A[col+4][inner_r];
					B[inner_r][col+5] = A[col+5][inner_r];
					B[inner_r][col+6] = A[col+6][inner_r];
					B[inner_r][col+7] = A[col+7][inner_r];
				}
			}
		}
		/*
		for(row = 0; row < N; row += 8)
		{
			for(col = 0; col < N; col += 8)
			{
				for(inner_r = 0; inner_r < 8; inner_r++)
				{
					d0 = A[col+inner_r][row];
					d1 = A[col+inner_r][row+1];
					d2 = A[col+inner_r][row+2];
					d3 = A[col+inner_r][row+3];

					if(inner_r == 0)
					{
						d4 = A[col+inner_r][row+4];
						d5 = A[col+inner_r][row+5];
						d6 = A[col+inner_r][row+6];
						d7 = A[col+inner_r][row+7];
					}
					B[row][col+inner_r] = d0;
					B[row][col+inner_r+64] = d1;
					B[row][col+inner_r+128] = d2;
					B[row][col+inner_r+192] = d3;
				}

				for(inner_r = 7; inner_r > 0; inner_r--)
				{
					d0 = A[col+inner_r][row+4];
					d1 = A[col+inner_r][row+5];
					d2 = A[col+inner_r][row+6];
					d3 = A[col+inner_r][row+7];
					B[row+4][col+inner_r] = d0;
					B[row+4][col+inner_r+64] = d1;
					B[row+4][col+inner_r+128] = d2;
					B[row+4][col+inner_r+192] = d3;
				}
				B[row+4][col] = d4;
				B[row+4][col+64] = d5;
				B[row+4][col+128] = d6;
				B[row+4][col+192] = d7;
			}
		}*/
	}
	else
	{
		for(row = 0; row < N; row+=16)
		{
			for(col = 0; col < M; col+=4)
			{
				for(inner_r = row; (inner_r < row+16)&&(inner_r < N); ++inner_r)
				{
					for(inner_c = col; (inner_c < col+4)&&(inner_c < M); ++inner_c)
					{
						if(inner_r-row == inner_c-col)
							d0 = A[inner_r][inner_c];
						else
							B[inner_c][inner_r] = A[inner_r][inner_c];
					}
					for(inner_c = col; (inner_c < col+4)&&(inner_c < M); ++inner_c)
					{
						if(inner_r-row == inner_c-col)
							B[inner_c][inner_r] = d0;
					}
				}
			}
		}
	}
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

