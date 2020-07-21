#include <stdio.h>
#include <dlx.h>

/* Number of queens */
#define N 3

/*
 * The n queens problem is the puzzle of placing n queens on an n*n chessboard
 * such that no queens attack each other, that is no two queens are on the same
 * row, column or diagonal.
 *
 * Using chess jargon we will denote rows as ranks and columns
 * as files, we also make a distinction between diagonals that cross the board
 * from up left to down right and reverse diagonals that go from up right to 
 * down left, we consider only diagonals that cover more than a single square,
 * an example can be seen in the following illustration for n = 4
 *
 * A1  A2  A3  A4  A5    B5
 *   \                 / 
 *    +---+---+---+---+  B4
 * R4 | \ |   |   | / |  
 *    +---+---+---+---+  B3
 * R3 |   | \ | / |   |
 *    +---+---+---+---+  B2
 * R2 |   | / | \ |   |
 *    +---+---+---+---+  B1
 * R1 | < |   |   | > |
 *    +---+---+---+---+
 *      F1  F2  F3  F4
 * 
 *
 * The first thing we'll do is count the number of files, ranks, diagonals and
 * reverse diagonals, by definition the number of ranks and files is equal to N.
 * Since the board is symmetric we will have the same number of diagonals as 
 * reverse diagonals, to get the numbers of diagonals we note that for:
 *
 * - n = 1 there are 0 diagonals
 *
 *    +---+
 *    |   |
 *    +---+
 *
 * - n = 2 there is 1 diagonal
 *
 *    +---+---+
 *    | \ |   |
 *    +---+---+
 *    |   | \ |
 *    +---+---+
 *
 * - n = 3 there are 3 diagonals
 *    
 *    +---+---+---+
 *    | \ | \ |   |
 *    +---+---+---+
 *    | \ | \ | \ |
 *    +---+---+---+
 *    |   | \ | \ |
 *    +---+---+---+
 *
 * From here on every new square can be formed adding a row and a column at the
 * top and right respectively of the previous square so we get the recursive
 * formula:
 *
 * d(1) = 0	d(2) = 1	d(n) = d(n - 1) + 2
 *
 * We're gonna prove by induction that d(n) = 2(n - 1) - 1 for n >= 3. 
 * We have that for n = 3
 *
 * 	d(3) = d(3 - 1) + 2 = 3 = 2(3 - 1) - 1
 *
 * Suppose the statement proven for n, then
 *
 * 	d(n + 1) = d(n) + 2 = 2n - 1		=>
 * 	d(n) = 2n - 1 - 2 = 2(n - 1) - 1	QED
 *
 * We have found that the number of diagonals is 2(n - 1) - 1
 */

#define NDIAGONALS 2 * (N - 1) - 1

 /*
  * The number of squares is of course n^2
  */

#define NSQUARES N * N

/* Now that we have determined some useful properties of our n chess board we
 * can start trying to solve the problem. To use DLX we need to transform 
 * the n queens problem into an exact cover one. To do this we consider four 
 * types of constraints, when a queen is placed on one of the squares she will 
 * cover one rank, one file and a diagonal, of course if a queen covers a 
 * given file, rank or diagonal no other queen can cover it, 
 * so we have our set of constraints:
 *	
 *	U = {F_1,..., F_N, R_1,..., R_N, A1,..., A_d(N), B1,..., B_d(N)}
 *
 * To denote the queen's positions we will use algebraic notation, denoting
 * the columns with smallcase letters starting from 'a' and rows with numbers
 * starting from 1
 *
 *    +---+---+---+---+
 *  4 |   |   |   |   |
 *    +---+---+---+---+
 *  3 |   |   |   |   |
 *    +---+---+---+---+
 *  2 |   |   |   |   |
 *    +---+---+---+---+
 *  1 |   |   |   |   |
 *    +---+---+---+---+
 *      a   b   c   d
 *
 * With this notation the queen in the corner a4 will cover the first rank and
 * column and the first diagonal, so the subset a4 of U would be
 *
 * 	a4 = {F_1, R_1, A_1}
 *
 * if we choose the position b4 we have
 *
 * 	b3 = {F_2, R_2, A_2, B_2}
 *
 * Et cetera, we note that a queen placed in a corner (a1, a4, d1, d4) only
 * covers three constraints, i.e. only covers one diagonal or reverse
 * diagonal.
 *
 * To solve the puzzle we only need one queen in each row and column, but we
 * need only at most one queen on each diagonal or reverse diagonal, to deal
 * with this we will use the primary and secondary constraints the library
 * provides. Rows and columns will be primary constraints, they need to be
 * fullfilled, while diagonals and reverse diagonals don't but can have no more
 * than one queen covering them.
 *
 * The matrix representation for n = 3 would be
 *  
 *	     F1  F2  F3  R1  R2  R3  A1  A2  A3  B1  B2  B3  (i, j)
 *	   +---+---+---+---+---+---+---+---+---+---+---+---+
 *	a1 | 1         | 1         |           |     1     | (0, 0)
 *	a2 | 1         |     1     | 1         |         1 | (1, 0)
 *	a3 | 1         |         1 |     1     |           | (2, 0)
 *	   +---+---+---+---+---+---+---+---+---+---+---+---+
 *	b1 |     1     | 1         | 1         | 1         | (0, 1)
 *	b2 |     1     |     1     |     1     |     1     | (1, 1)
 *	b3 |     1     |         1 |         1 |         1 | (2, 1)
 *	   +---+---+---+---+---+---+---+---+---+---+---+---+
 *	c1 |         1 | 1         |     1     |           | (0, 2)
 *	c2 |         1 |     1     |         1 | 1         | (1, 2)
 *	c3 |         1 |         1 |           |     1     | (2, 2)
 *	   +---+---+---+---+---+---+---+---+---+---+---+---+
 *
 * It's easy to check that we can get the matrix entry for each position by
 *
 * 	file             'F' = j position
 * 	rank             'R' = i + n position 
 * 	diagonal         'A' = i + j + 2n - 1
 * 	reverse diagonal 'B' = (n - 1 + i - j) + (4n - 4)
 */

#define CFILE(i, j) j
#define CRANK(i, j) i + N
#define CDIAG(i, j) i + j + 2*N - 1
#define CRDIA(i, j) (N - 1 + i - j) + 4*(N - 1)

/*
 * Once we have done this, we can search for solutions by calling the DLX
 * functions. For implementation details see below.
 */

/* 
 * The following is an utility macro to determine the sum of the length of 
 * the names of the constraints. 
 * We assume X < 100, X depends on n so realistically anything over n = 15 
 * will take an absurd amount of time and shouldn't be calculated with 
 * this program.
 * If the input is less than 10 we need three times X, since every name
 * has an identifier (F, R, A, B), the number of the file, rank, etc... and 
 * a null character at the end. If X > 0 the numbers from 9 need 2 characters
 * for printing.
 */
#define STRINGL(X) (((X) <= 10) ? 3 * (X) : 30 + 4 * ((X) - 10)) + 1

/* Number of characters in the position names */
#define CPOSITION (N <= 10) ? 3 : 4

/* Number of characters in the files/ranks and diagonals */
#define CFILESRANKS STRINGL(N)
#define CDIAGONALS STRINGL(NDIAGONALS)

/* Function used to generate constraints, is called 4 times, one for each type
 * of constraint */
void 
gen_constraints(dlx_universe u, int const_type, char id, char *names, int n) {
	/* Index */
	unsigned int i;

	/* Current length of string and temporal variable */
	unsigned int len = 0, tmp;

	for (i = 0; i < n; ++i) {
		/* Generate constraint name, zero terminated */
		tmp = sprintf(names + len, "%c%u%c", id, i + 1, '\0');

		/* Add constraint to universe with the generated name */
		dlx_add_constraint(u, const_type, names + len);

		/* The current length of the string is equal to the previous 
		 * length plus the length of the new name */
		len += tmp;
	}
}

int
main(void)
{
	/* Indices */
	unsigned int i, j;

	/* Position/subset names */
	char position[N][N][CPOSITION];

	/* Files and ranks names */
	char files[CFILESRANKS], ranks[CFILESRANKS];
	     
	/* Diagonals and reverse diagonals names */
	char diagonals[CDIAGONALS], reverse_diagonals[CDIAGONALS];


	/* Generate position names */
	for (i = 0; i < N; ++i)
		for (j = 0; j < N; ++j)
			sprintf(position[i][j], "%c%u%c", 'a' + j, i + 1, '\0');

	/* Create universe */
	dlx_universe u = dlx_create_universe();

	/* Generate files, ranks and diagonals, the order of the constraints
	 * depends on the order of addition so changing the order 
	 * can give incorrect results since the subsets depend on this 
	 * ordering to indicate their elements */
	gen_constraints(u, DLX_PRIMARY,   'F', files, N);
	gen_constraints(u, DLX_PRIMARY,   'R', ranks, N);
	gen_constraints(u, DLX_SECONDARY, 'A', diagonals, NDIAGONALS);
	gen_constraints(u, DLX_SECONDARY, 'B', reverse_diagonals, NDIAGONALS);

	/* 
	 * fill corners, subsets with only 3 elements
	 *
	 * +---+---+---+---+
	 * | x |   |   | x |
	 * +---+---+---+---+
	 * |   |   |   |   |
	 * +---+---+---+---+
	 * |   |   |   |   |
	 * +---+---+---+---+ 
	 * | x |   |   | x |
	 * +---+---+---+---+ 
	 */

	dlx_add_subset(u, 3, position[0][0],
	               CFILE(0, 0),
	               CRANK(0, 0),
	               CRDIA(0, 0));


	dlx_add_subset(u, 3, position[N - 1][N - 1],
	               CFILE(N - 1, N - 1),
	               CRANK(N - 1, N - 1),
	               CRDIA(N - 1, N - 1));

	dlx_add_subset(u, 3, position[N - 1][0],
	               CFILE(N - 1, 0),
	               CRANK(N - 1, 0),
	               CDIAG(N - 1, 0));

	dlx_add_subset(u, 3, position[0][N - 1],
	               CFILE(0, N - 1),
	               CRANK(0, N - 1),
	               CDIAG(0, N - 1));

	/* Fill inner columns 
	 *
	 * +---+---+---+---+
	 * |   | x | x |   |
	 * +---+---+---+---+
	 * |   | x | x |   |
	 * +---+---+---+---+
	 * |   | x | x |   |
	 * +---+---+---+---+ 
	 * |   | x | x |   |
	 * +---+---+---+---+ 
	 */

	for (i = 1; i < N - 1; ++i)
		for (j = 0; j < N; ++j) {
			dlx_add_subset(u, 4, position[i][j],
			               CFILE(i, j), CRANK(i, j),
			               CDIAG(i, j), CRDIA(i, j));
		}

	/* Fill remaining lateral columns
	 *
	 * +---+---+---+---+
	 * |   |   |   |   |
	 * +---+---+---+---+
	 * | x |   |   | x |
	 * +---+---+---+---+
	 * | x |   |   | x |
	 * +---+---+---+---+ 
	 * |   |   |   |   |
	 * +---+---+---+---+ 
	 */

	for (i = 0; i < N; i += N - 1)
		for (j = 1; j < N - 1; ++j) {
			dlx_add_subset(u, 4, position[i][j],
			               CFILE(i, j), CRANK(i, j),
			               CDIAG(i, j), CRDIA(i, j));
		}

	/* Search for all solutions */
	dlx_search(u, 0);

	/* Print universe, subsets and solutions */
	puts("Universe and subsets\n");
	dlx_print_universe(u, "%s", "%s");

	puts("\nSolutions\n");
	dlx_print_solutions(u, "%s");

	/* Cleanup */
	dlx_delete_universe(u);

	return 0;
}
