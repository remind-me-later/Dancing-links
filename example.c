#include "dlx.h"

#include <stdio.h>

int
main(void)
{
	char *constraints[] = {"1", "2", "3", "4", "5", "6", "7"};

	dlx_universe u = dlx_create_universe();

	dlx_add_primary_constraints(u, 7, constraints);

	dlx_add_subset(u, "A", 3, 0, 3, 6);
	dlx_add_subset(u, "B", 2, 0, 3);
	dlx_add_subset(u, "C", 3, 3, 4, 6);
	dlx_add_subset(u, "D", 3, 2, 4, 5);
	dlx_add_subset(u, "E", 4, 1, 2, 5, 6);
	dlx_add_subset(u, "F", 2, 1, 6);

	dlx_print_universe(u);

	dlx_search_all(u);

	return 0;
}
