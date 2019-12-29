#include <stdio.h>
#include <dlx.h>

int
main(void)
{
	/* Create a string array with the names 
	 * of the elements in our universe. */
	char constraints[] = "1, 2, 3, 4, 5, 6, 7";

	/* Setup universe. */
	dlx_universe u = dlx_create_universe();

	/* Add constraints */
	dlx_add_constraints(u, constraints, 1);

	/* Add subsets, specifying the number of elements,
	 * the name, and the position of the elements in the universe*/
	dlx_add_subset(u, 3, "A", 0, 3, 6);
	dlx_add_subset(u, 2, "B", 0, 3);
	dlx_add_subset(u, 3, "C", 3, 4, 6);
	dlx_add_subset(u, 3, "D", 2, 4, 5);
	dlx_add_subset(u, 4, "E", 1, 2, 5, 6);
	dlx_add_subset(u, 2, "F", 1, 6);

	/* Setup links */
	dlx_create_links(u);

	/* Look for all solutions */
	dlx_search(u, 0);

	/* Print universe */
	puts("Universe:");
	dlx_print_universe(u);

	/* Print solutions */
	puts("\nSolutions:");
	dlx_print_solutions(u);

	/* clean up */
	dlx_delete_universe(u);

	return 0;
}
