#include "dlx.h"

#include <stdio.h>

#define N 8

int
main(void)
{
	unsigned int i, j;

	char *str[8][8] = {
		{"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"},
		{"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7"},
		{"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6"},
		{"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5"},
		{"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4"},
		{"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3"},
		{"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2"},
		{"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"}
	};

	char *primary_constraints[] = {
		"F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7",
		"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7"
	};

	char *secondary_constraints[] = {
		"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "A10", "A11", "A12",
		"B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "B10", "B11", "B12",
	};

	dlx_universe u = dlx_create_universe();

	dlx_add_primary_constraints(u, 2 * N, primary_constraints);
	dlx_add_secondary_constraints(u, (N - 2) * 4 + 2, secondary_constraints);

	/* fill corners */
	dlx_add_subset(u, str[0][0], 3,
	               0,
	               N,
	               (4 * N - 4) + (N - 1));


	dlx_add_subset(u, str[N - 1][N - 1], 3,
	               N - 1,
	               N + N - 1,
	               (4 * N - 4) + (N - 1));

	dlx_add_subset(u, str[N - 1][0], 3,
	               0,
	               N + N - 1,
	               (2 * N - 1) + N - 1);

	dlx_add_subset(u, str[0][N - 1], 3,
	               N - 1,
	               N,
	               (2 * N - 1) + N - 1);

	/* Fill center band */
	for (i = 1; i < N - 1; ++i)
		for (j = 0; j < N; ++j) {
			dlx_add_subset(u, str[i][j], 4,
			               j,
			               N + i,
			               (2 * N - 1) + i + j,
			               (4 * N - 4) + (N - 1 - j + i));
		}

	/* Fill up and down bands */
	for (i = 0; i < N; i += N - 1)
		for (j = 1; j < N - 1; ++j) {
			dlx_add_subset(u, str[i][j], 4,
			               j,
			               N + i,
			               (2 * N - 1) + i + j,
			               (4 * N - 4) + (N - 1 - j + i));
		}

	//dlx_print_universe(u);
	//putchar('\n');

	dlx_search_all(u);

	return 0;
}
