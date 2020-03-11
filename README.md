# Dancing-links

A C library that implements Knuth's Algorithm X to solve exact cover problems.
The library aims to have a simple interface and allows defining primary and secondary constraints.
The implementation folows closely the one described in Knuth's [paper](https://arxiv.org/abs/cs/0011047).

## Making

Executing the make command should compile everything, putting the executable examples in the bin folder and the library in the lib folder.
```
make
```
The debug and profile targets compile the program to be analyzed by Valgrind and gprof respectively.

## Examples

### Simple example
The first executable [example](https://en.wikipedia.org/wiki/Exact_cover#Detailed_example) is the following:

Let  S  = {_A_,  _B_,  _C_,  _D_,  _E_,  _F_} be a collection of subsets of a set  _X_  = {1, 2, 3, 4, 5, 6, 7} such that:

- _A_ = {1, 4, 7}
- _B_ = {1, 4}
- _C_ = {4, 5, 7}
- _D_ = {3, 5, 6}
- _E_ = {2, 3, 6, 7}
- _F_ = {2, 7}

The only solution to the problem is _S*_ = {_B_, _D_, _F_}, let's check it using the library. The source code of example.c is:

``` c
#include <stdio.h>
#include <dlx.h>

int
main(void)
{
	/* Create a string array with the names 
	 * of the elements in our universe. */
	char constraints[] = "1, 2, 3, 4, 5, 6, 7";

	/* Setup universe. */
	dlx_universe u = dlx_create_universe(7, 6, 1);

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

	/* Look for solutions, specifying number 
	 * of required solutions (0 for all) */
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
```
Executing the program yields the following result:
```
$ ./bin/example
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
### N-Queens problem

The second example nqueens.c finds solutions for the n-queens problem. Executing it with N = 4 produces:
```
$ ./bin/nqueens
Universe:
U = {F0, F1, F2, F3, R0, R1, R2, R3, A0, A1, A2, A3, A4, B0, B1, B2, B3, B4}
a4 = {F0, R0, B2}
d1 = {F3, R3, B2}
a1 = {F0, R3, A2}
d4 = {F3, R0, A2}
a3 = {F0, R1, A0, B3}
b3 = {F1, R1, A1, B2}
c3 = {F2, R1, A2, B1}
d3 = {F3, R1, A3, B0}
a2 = {F0, R2, A1, B4}
b2 = {F1, R2, A2, B3}
c2 = {F2, R2, A3, B2}
d2 = {F3, R2, A4, B1}
b4 = {F1, R0, A0, B1}
c4 = {F2, R0, A1, B0}
b1 = {F1, R3, A3, B4}
c1 = {F2, R3, A4, B3}
Solutions:
S1* = {a2, b4, c1, d3}
S2* = {a3, b1, c4, d2}
```
