#include <dlx.h>
#include <stdio.h>

/* Number of rows and columns */
#define N               9
#define NSUBSETS        729
#define NCONSTRAINTS    324
#define CONSLEN         (81 * 5)
#define SUBSET_NAME_LEN 7

void print_sudoku(char sudoku[N][N]) {
	unsigned int i, j;

	printf("\n     1 2 3   4 5 6   7 8 9");
	printf("\n   +-------+-------+-------+\n");

	for (i = 0; i < N; ++i) {
		printf(" %u | ", i + 1);

		for (j = 0; j < N; ++j) {
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

void print_solution(char **solution, unsigned int size) {
	char         sudoku[N][N];
	unsigned int i;
	unsigned int row, col, number;

	for (i = 0; i < size; ++i) {
		sscanf(solution[i], "r%uc%u#%u", &row, &col, &number);
		/*
		printf("%i: %s; row: %u, col: %u, num %u\n", i, sols[i], row,
		       col, number);
			   */

		sudoku[row - 1][col - 1] = number + '0';
	}

	print_sudoku(sudoku);
}

void print_solutions(dlx_univ_t u) {
	unsigned int number_of_solutions;
	unsigned int size;
	char **solution;

	number_of_solutions = dlx_found_solutions(u);

	while(number_of_solutions--) {
		solution = dlx_pop_solution(u, &size);
		print_solution(solution, size);
	}
}

int main(void) {
	/* Indices */
	unsigned int i, j, k;

	/* Constraint and subset names */
	char position_number[N][N][N][SUBSET_NAME_LEN], cells[CONSLEN],
	    rows[CONSLEN], cols[CONSLEN], blocks[CONSLEN];

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

	for (i = 0; i < N; ++i) {
		for (j = 0; j < N; ++j) {
			for (k = 0; k < N; ++k) {
				sprintf(position_number[i][j][k],
				        "r%uc%u#%u%c", i + 1, j + 1, k + 1,
				        '\0');
			}
		}
	}

	/* Create universe */
	dlx_univ_t u = dlx_create_universe();

	/* Current length of string and temporal variable */
	unsigned int len = 0, tmp;

	/*
	 * Generate constraints: cell, column, row and block.
	 * The name of each constraint is formatted as "C2#4" where
	 * the number after 'C' is the column
	 * (X, R, C, B denote cell, row, column and block respectively)
	 * and the one after '#' the value
	 */
	for (i = 0; i < N; ++i) {
		for (j = 0; j < N; ++j) {
			tmp = sprintf(cells + len, "X%u#%u%c", i + 1, j + 1,
			              '\0');

			dlx_add_constraint(u, DLX_PRIMARY, cells + len);

			len += tmp;
		}
	}

	len = 0;

	for (i = 0; i < N; ++i) {
		for (j = 0; j < N; ++j) {
			tmp = sprintf(rows + len, "R%u#%u%c", i + 1, j + 1,
			              '\0');

			dlx_add_constraint(u, DLX_PRIMARY, rows + len);

			len += tmp;
		}
	}

	len = 0;

	for (i = 0; i < N; ++i) {
		for (j = 0; j < N; ++j) {
			tmp = sprintf(cols + len, "C%u#%u%c", i + 1, j + 1,
			              '\0');

			dlx_add_constraint(u, DLX_PRIMARY, cols + len);

			len += tmp;
		}
	}

	len = 0;

	for (i = 0; i < N; ++i) {
		for (j = 0; j < N; ++j) {
			tmp = sprintf(blocks + len, "B%u#%u%c", i + 1, j + 1,
			              '\0');

			dlx_add_constraint(u, DLX_PRIMARY, blocks + len);

			len += tmp;
		}
	}

	/* Add subsets, aka positions */
	for (i = 0; i < N; ++i) {
		for (j = 0; j < N; ++j) {
			for (k = 0; k < N; ++k) {
				dlx_add_subset(
				    u, 4, position_number[i][j][k], i * 9 + j,
				    81 + i * 9 + k, 162 + j * 9 + k,
				    243 + (i / 3) * 27 + (j / 3) * 9 + k);
			}
		}
	}

	dlx_search(u, 15);

	/*
	puts("Universe:");
	dlx_print_universe(u, "%s", "%s");
	putchar('\n');

	puts("Solutions:");
	dlx_print_solutions(u, "%s");
	*/

	print_solutions(u);

	dlx_delete_universe(u);

	return 0;
}
