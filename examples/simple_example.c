// # Simple example
//
// An example taken from the
// [Wikipedia](https://en.wikipedia.org/wiki/Exact_cover#Detailed_example)
// exact cover page.
//
// Let `S = {A, B, C, D, E, F}` be a collection of subsets of a set
// `X = {1, 2, 3, 4, 5, 6, 7}` where
//
// - `A = {1, 4, 7}`
// - `B = {1, 4}`
// - `C = {4, 5, 7}`
// - `D = {3, 5, 6}`
// - `E = {2, 3, 6, 7}`
// - `F = {2, 7}`
//
// Let's use the library to find all the exact covers of `X`.
// First, of course, we need to include the library and `stdio.h` to print

#include <stdio.h>
#include <dlx.h>

// The code is pretty simple, first we create an array with the names
// of the elements in our universe `X` and ask for a new universe, then
// we add the constraints and the subsets to the created universe and finally
// we call `dlx_search` to search for solutions. For printing we use the functions
// provided by the library.

int main(void) {
	char *constraints[] = {"1", "2", "3", "4", "5", "6", "7"};

	dlx_universe X = dlx_create_universe();

	dlx_add_constraint(X, DLX_PRIMARY, constraints[0]);
	dlx_add_constraint(X, DLX_PRIMARY, constraints[1]);
	dlx_add_constraint(X, DLX_PRIMARY, constraints[2]);
	dlx_add_constraint(X, DLX_PRIMARY, constraints[3]);
	dlx_add_constraint(X, DLX_PRIMARY, constraints[4]);
	dlx_add_constraint(X, DLX_PRIMARY, constraints[5]);
	dlx_add_constraint(X, DLX_PRIMARY, constraints[6]);

	// Add the subsets of S, specifying the number of elements,
	// the name, and the position of the elements in the universe
	dlx_add_subset(X, 3, "A", 0, 3, 6);
	dlx_add_subset(X, 2, "B", 0, 3);
	dlx_add_subset(X, 3, "C", 3, 4, 6);
	dlx_add_subset(X, 3, "D", 2, 4, 5);
	dlx_add_subset(X, 4, "E", 1, 2, 5, 6);
	dlx_add_subset(X, 2, "F", 1, 6);

	// Look for solutions, specifying the number of
	// required solutions or 0 to look for all
	dlx_search(X, 0);

	puts("Universe:\n");
	dlx_print_universe(X, "%s", "%s");

	puts("\nSolutions:\n");
	dlx_print_solutions(X, "%s");

	dlx_delete_universe(X);

	return 0;
}
