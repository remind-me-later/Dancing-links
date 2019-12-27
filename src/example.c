#include <stdio.h>
#include "dlx.h"

int
main(void)
{
	/* Create a string array with the names 
	 * of the elements in our universe. */
	char *constraints[] = {"1", "2", "3", "4", "5", "6", "7"};

	/* Setup universe. */
	dlx_universe u = dlx_create_universe();

	/* Add constraints */
	dlx_add_primary_constraints(u, 7, constraints);

	/* Add subsets, specifying a name, the number of elements
	 * and the position of the elements in the universe*/
	dlx_add_subset(u, "A", 3, 0, 3, 6);
	dlx_add_subset(u, "B", 2, 0, 3);
	dlx_add_subset(u, "C", 3, 3, 4, 6);
	dlx_add_subset(u, "D", 3, 2, 4, 5);
	dlx_add_subset(u, "E", 4, 1, 2, 5, 6);
	dlx_add_subset(u, "F", 2, 1, 6);

	/* Print universe */
	puts("Universe:");
	dlx_print_universe(u);

	/* Search and print all solutions */
	puts("\nSolutions:");
	dlx_search_all(u);

	return 0;
}
