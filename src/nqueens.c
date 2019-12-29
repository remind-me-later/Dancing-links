#include <stdio.h>
#include <dlx.h>

#define N 8
#define NDIAGONALS (N - 2) * 2 + 1
#define NELEMENTS 2 * (N + NDIAGONALS)
#define NSUBSETS N * N

#define STRINGL(X) ((X) <= 10) ? 3 * (X) : 30 + 4 * ((X) - 10) - 1
#define CPOSITION (N <= 10) ? 3 : 4
#define CFILESRANKS STRINGL(N)
#define CDIAGONALS STRINGL(NDIAGONALS)

int
main(void)
{
	unsigned int i, j;

	char position[N][N][CPOSITION], files[CFILESRANKS], ranks[CFILESRANKS],
	     diagonals[CDIAGONALS], reverse_diagonals[CDIAGONALS];

	unsigned int len = 0;

	/* Generate position names */
	for (i = 0; i < N; ++i)
		for (j = 0; j < N; ++j)
			sprintf(position[i][j], "%c%u", 97 + j, N - i);

	/* Generate file and rank names */
	for (i = 0; i < N - 1; ++i) {
		sprintf(files + len, "F%u,", i);
		len += sprintf(ranks + len, "R%u,", i);
	}
	sprintf(files + len, "F%u", i);
	sprintf(ranks + len, "R%u", i);

	/* Generate diagonal names */
	for (i = len = 0; i < NDIAGONALS - 1; ++i) {
		sprintf(diagonals + len, "A%u,", i);
		len += sprintf(reverse_diagonals + len, "B%u,", i);
	}
	sprintf(diagonals + len, "A%u", i);
	sprintf(reverse_diagonals + len, "B%u", i);

	dlx_universe u = dlx_create_universe(NELEMENTS, NSUBSETS, 32);

	dlx_add_constraints(u, files, 1);
	dlx_add_constraints(u, ranks, 1);

	dlx_add_constraints(u, diagonals, 0);
	dlx_add_constraints(u, reverse_diagonals, 0);

	/* fill corners */
	dlx_add_subset(u, 3, position[0][0],
	               0,
	               N,
	               (4 * N - 4) + (N - 1));


	dlx_add_subset(u, 3, position[N - 1][N - 1],
	               N - 1,
	               N + N - 1,
	               (4 * N - 4) + (N - 1));

	dlx_add_subset(u, 3, position[N - 1][0],
	               0,
	               N + N - 1,
	               (2 * N - 1) + N - 1);

	dlx_add_subset(u, 3, position[0][N - 1],
	               N - 1,
	               N,
	               (2 * N - 1) + N - 1);

	/* Fill center band */
	for (i = 1; i < N - 1; ++i)
		for (j = 0; j < N; ++j) {
			dlx_add_subset(u, 4, position[i][j],
			               j,
			               N + i,
			               (2 * N - 1) + i + j,
			               (4 * N - 4) + (N - 1 - j + i));
		}

	/* Fill up and down bands */
	for (i = 0; i < N; i += N - 1)
		for (j = 1; j < N - 1; ++j) {
			dlx_add_subset(u, 4, position[i][j],
			               j,
			               N + i,
			               (2 * N - 1) + i + j,
			               (4 * N - 4) + (N - 1 - j + i));
		}

	dlx_create_links(u);

	dlx_search(u, 0);

	puts("Universe:");
	dlx_print_universe(u);

	puts("Solutions:");
	dlx_print_solutions(u);

	dlx_delete_universe(u);

	return 0;
}
