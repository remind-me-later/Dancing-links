#include <dlx.h>
#include <stdio.h>

/* Number of rows and columns */
const unsigned char N = 9;

void print_sudoku(char sudoku[N][N]) {
    printf("     1 2 3   4 5 6   7 8 9\n");
    printf("   +-------+-------+-------+\n");

    for (unsigned int i = 0; i < N; ++i) {
	printf(" %u | ", i + 1);

	for (unsigned int j = 0; j < N; ++j) {
	    printf("%c ", sudoku[i][j]);

	    if (j % 3 == 2) {
		printf("| ");
	    }
	}

	if (i % 3 == 2) {
	    printf("\n   +-------+-------+-------+\n");
	} else {
	    putchar('\n');
	}
    }
}

void print_solution(dlx_solution_iterator iter) {
    char sudoku[N][N];
    static unsigned int sol_number = 1;
    unsigned int *next;

    printf("Grid %u:\n\n", sol_number++);

    while ((next = dlx_solution_iterator_next(iter)) != NULL) {
	unsigned int row = (*next / 100) % 10;
	unsigned int col = (*next / 10) % 10;
	unsigned int number = *next % 10;

	sudoku[row][col] = (char)number + '0';
    }

    print_sudoku(sudoku);

    printf("\n");
}

int main(void) {
    /* Constraint and subset names */
    unsigned int position_number[N][N][N];

    /*
     * Generate position/number names with the format "r2c7#2"
     * where 'r' indicates the row, 'c' the column and '#' the value,
     * so the previous string would be:
     *
     *           1 2 3   4 5 6   7 8 9
     *         +-------+-------+-------+
     *       1 |       |       |       |
     *       2 |       |       | 2     |
     *       3 |       |       |       |
     *         +-------+-------+-------+
     *       4 |       |       |       |
     *       5 |       |       |       |
     *       6 |       |       |       |
     *         +-------+-------+-------+
     *       7 |       |       |       |
     *       8 |       |       |       |
     *       9 |       |       |       |
     *         +-------+-------+-------+
     */

    for (unsigned int i = 0; i < N; ++i) {
	for (unsigned int j = 0; j < N; ++j) {
	    for (unsigned int k = 0; k < N; ++k) {
		position_number[i][j][k] = i * 100 + j * 10 + k + 1;
	    }
	}
    }

    /* Create universe */
    dlx_universe universe =
	dlx_universe_new(&print_solution, N * N * 4, 0, N * N * N);

    /*
     * Generate constraints: cell, column, row and block.
     * The name of each constraint is formatted as "C2#4" where
     * the number after 'C' is the column
     * (X, R, C, B denote cell, row, column and block respectively)
     * and the one after '#' the value
     */

    /* Add subsets, aka positions */
    for (unsigned int i = 0; i < N; ++i) {
	for (unsigned int j = 0; j < N; ++j) {
	    for (unsigned int k = 0; k < N; ++k) {
		dlx_universe_add_subset(
		    universe, 4, &position_number[i][j][k], i * 9 + j,
		    81 + i * 9 + k, 162 + j * 9 + k,
		    243 + (i / 3) * 27 + (j / 3) * 9 + k);
	    }
	}
    }

    dlx_universe_search(universe, 10);

    dlx_universe_free(universe);

    return 0;
}
