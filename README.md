# Dancing-links

A C library that implements the Dancing Links algorithm to solve exact cover problems.
The library aims to have a simple interface and allows defining primary and secondary constraints.
The implementation folows closely the one described in Knuth's [paper](https://arxiv.org/abs/cs/0011047).

## Example

Consider the following [example](https://en.wikipedia.org/wiki/Exact_cover#Detailed_example):

Let  S  = {_A_,  _B_,  _C_,  _D_,  _E_,  _F_} be a collection of subsets of a set  _X_  = {1, 2, 3, 4, 5, 6, 7} such that:

- _A_ = {1, 4, 7}
- _B_ = {1, 4}
- _C_ = {4, 5, 7}
- _D_ = {3, 5, 6}
- _E_ = {2, 3, 6, 7}
- _F_ = {2, 7}

The only solution to the problem is _S*_ = {_B_, _D_, _F_}, let's check it using the library:

``` c
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
```
The program output is the following:

```
Universe:
U = {1, 2, 3, 4, 5, 6, 7}

A = {1, 4, 7}
B = {1, 4}
C = {4, 5, 7}
D = {3, 5, 6}
E = {2, 3, 6, 7}
F = {2, 7}

Solutions:
S1* = {B, D, F}
```
