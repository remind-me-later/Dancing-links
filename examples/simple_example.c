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

#include <dlx.h>
#include <stdio.h>

// We first make a fucntion to print the solutions given
// by the library, it receives an array of void pointers
// and the size of the array

void print_solution(void **sol, unsigned int size) {
	char **      solution = (char **)sol;
	unsigned int i;

	printf("S* = {");

	for (i = 0; i + 1 < size; ++i)
		printf("%s, ", solution[i]);

	printf("%s}\n", solution[i]);
}

// First we create an array with the names of the elements in our 
// universe `X` and a new universe, then we add the constraints and 
// the subsets to the created universe.  Finally we call `dlx_search` 
// to search for solutions.

int main(void) {
	dlx_univ_t X = dlx_create_universe(&print_solution);

	dlx_add_constraints(X, DLX_PRIMARY, 7);

	// Add the subsets of S, specifying the number of elements,
	// the name, and the position of the elements in the universe
	dlx_add_subset(X, 3, "A", 0, 3, 6);
	dlx_add_subset(X, 2, "B", 0, 3);
	dlx_add_subset(X, 3, "C", 3, 4, 6);
	dlx_add_subset(X, 3, "D", 2, 4, 5);
	dlx_add_subset(X, 4, "E", 1, 2, 5, 6);
	dlx_add_subset(X, 2, "F", 1, 6);

	// Print problem
	puts("Let X := {1, 2, 3, 4, 5, 6, 7}\nand S := {A, B, C, D, E, F}");
	puts("\nWhere");
	puts("\tA := {1, 4, 7}");
	puts("\tB := {1, 4}");
	puts("\tC := {4, 5, 7}");
	puts("\tD := {3, 5, 6}");
	puts("\tE := {2, 3, 6, 7}");
	puts("\tF := {2, 7}");

	printf("\nThe only exact cover of X is ");

	// Look for solutions, specifying the number of required solutions 
	dlx_search(X, DLX_ALL);

	dlx_delete_universe(X);

	return 0;
}
