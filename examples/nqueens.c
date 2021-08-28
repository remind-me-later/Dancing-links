// # N queens puzzle
//
// The n queens problem is the puzzle of placing n queens on a chessboard
// with n^2 squares such that no queens attack each other,
// that is no two queens are on the same row, column or diagonal.
// For example a solution for n = 4:
//
// 	  +---+---+---+---+
// 	4 |   | Q |   |   |
// 	  +---+---+---+---+
// 	3 |   |   |   | Q |
// 	  +---+---+---+---+
// 	2 | Q |   |   |   |
// 	  +---+---+---+---+
// 	1 |   |   | Q |   |
// 	  +---+---+---+---+
// 	    a   b   c   d
//
// ## Notation
//
// We will denote rows as ranks and columns as files.
//
// We make a distinction between the diagonals that cross the board
// from up left to down right, and the ones that go from up right to
// down left, we will call them diagonals and reverse diagonals respectively.
//
// Since diagonals that cover only one square are of no use to use we will not
// consider them.
//
// The notation for these constraints is as follows
//
// | Constraint       | Symbol |
// | ---------------- | :----: |
// | file             |   F    |
// | rank             |   R    |
// | diagonal         |   A    |
// | reverse diagonal |   B    |
//
// An example for n = 4:
//
// 	                                 B5
// 	                               *
// 	                             *   B4
// 	A1  A2  A3  A4  A5         *   *
// 	 *   *   *   *   *       *   *   B3
// 	   *   *   *   *   *   *   *   *
// 	     *   *   +---+---+---+---+   B2
// 	R4 --- * > * | * |   |   | * | *
// 	         *   +---+---+---+---+   B1
// 	R3 ------> * |   | * | * |   | *
// 	             +---+---+---+---+
// 	R2 --------> |   | * | * |   |
// 	             +---+---+---+---+
// 	R1 --------> | * |   |   | * |
// 	             +---+---+---+---+
// 	               ^   ^   ^   ^
// 	               F1  F2  F3  F4
//
// To denote the queens positions we will use both chess's algebraic notation
// and the traditional C bidimensional array notation.
//
// The C array notation is as follows, we will use two indices i and j,
// starting from the first denoting the row and the second the column, starting
// by the upper left corner.
//
// 	     j0  j1  j2  j3
// 	    +---+---+---+---+
// 	i0  |   | Q |   |   |
// 	    +---+---+---+---+
// 	i1  |   |   |   | Q |
// 	    +---+---+---+---+
// 	i2  | Q |   |   |   |
// 	    +---+---+---+---+
// 	i3  |   |   | Q |   |
// 	    +---+---+---+---+
//
// The algebraic notation consists of labeling the columns with letters,
// starting from a, and the rows with numbers from 1
//
// 	  +---+---+---+---+
// 	4 |   | Q |   |   |
// 	  +---+---+---+---+
// 	3 |   |   |   | Q |
// 	  +---+---+---+---+
// 	2 | Q |   |   |   |
// 	  +---+---+---+---+
// 	1 |   |   | Q |   |
// 	  +---+---+---+---+
// 	    a   b   c   d
//
// The relationship between the two is demonstrated by the following diagram,
// it also shows how the elements are allocated in memory (with m = n - 1)
//
// 	        (0, 0)            (0, m)
// 	        +----+----+----+..+----+
// 	(0, 0)  | a1 | b1 | c1 |  | x1 |
// 	        +----+----+----+..+----+
// 	        | a2 | b2 | c2 |  | x2 |
// 	        +----+----+----+..+----+
// 	        :    :    :    :  :    :
// 	        +----+----+----+..+----+
// 	(m, 0)  | am | bm | cm |  | xm |
// 	        +----+----+----+..+----+
// 	        (m, 0)            (m, m)
//
// ## Properties of the generalized chess board
//
// Let's first define the number of queens we are interested in finding
// solutions for we will call this number n

const unsigned int N = 8;

// The first thing we'll do is count the number of files, ranks, diagonals and
// reverse diagonals, by definition the number of ranks and files is equal to n

const unsigned int NUMBER_OF_RANKS_AND_FILES = N;

// With that out of the way let's get to the harder bit.
// Since the board is symmetric we will have the same number of diagonals as
// reverse diagonals, so if we find the number of diagonals we're done.
//
// We note that for:
//
// - n = 1 there are 0 diagonals
//
// 	+---+
// 	|   |
// 	+---+
//
// - n = 2 there is 1 diagonal
//
// 	+---+---+
// 	| * |   |
// 	+---+---+
// 	|   | * |
// 	+---+---+
//
// - n = 3 there are 3 diagonals
//
// 	+---+---+---+
// 	| * | * |   |
// 	+---+---+---+
// 	| * | * | * |
// 	+---+---+---+
// 	|   | * | * |
// 	+---+---+---+
//
// From here on every new square can be formed adding a row and a column at the
// top and right respectively of the previous square so we get the recursive
// formula
//
//     d(1) = 0
//     d(2) = 1
//     d(n) = d(n - 1) + 2
//
// We're gonna prove by induction that d(n) = 2n - 3 for n >= 3.
//
// Let's check our base case, we have that for n = 3
//
//     d(3) = d(3 - 1) + 2 = 3 = 2*3 - 3
//
// Suppose the statement proven for n, then
//
//     d(n + 1) = d(n) + 2 = 2(n + 1) - 3    =>
//     d(n) = 2n - 1 - 2 = 2n - 3            QED
//
// The number of diagonals is then 2n - 3.

const unsigned int NUMBER_OF_DIAGONALS = 2 * N - 3;

// And the number of squares is of course n^2.

const unsigned int NSQUARES = N * N;

// ## Representation as an exact cover problem
//
// Now that we have determined some useful properties of our n chess board we
// can start trying to solve the problem.
//
// Of course our objective is to transform it to an exact cover problem to be
// able to solve it using the dancing links technique.
//
// Our constraints are the rows, columns, diagonals and reverse diagonals,
// when a queen is placed on one no other queen can be placed on the same
// constraint. So our set of constraints or universe is
//
//     U = {F_1,..., F_N, R_1,..., R_N, A1,..., A_d(N), B1,..., B_d(N)}
//
// For example the queen in the corner a4 will cover the first rank and
// column and the first diagonal, so the subset a4 of U would be
//
//     a4 = {F_1, R_1, A_1}
//
// if instead we choose the position b4 we have
//
//     b3 = {F_2, R_2, A_2, B_2}
//
// We note that a queen placed in a corner (a1, a4, d1, d4) only
// covers three constraints, i.e. only covers one diagonal or reverse
// diagonal.
//
// So we see that a problem arises with this choice of constraints,
// we want to find the placement of n queens such that no one is attacked
// but with these constraints we also add the condition that every diagonal
// and reverse diagonal must be covered.
//
// To deal with this we will use secondary constraints, the difference between
// primary and secondary constraints is that the second must be covered _at
// most_ once, while the first must be covered _exactly_ once.
//
// With these new types of constraints at our disposal we see that the
// solutions to our problem are given by setting rows and columns as primary
// constraints, if we place n queens necessarily we will cover all rows and
// columns, while diagonals are secondary constraints.
//
// Here is the exact cover matrix for n = 3, the rows represent the subsets
// and the columns the constraints, the zeroes are omitted
//
// 	     F1  F2  F3  R1  R2  R3  A1  A2  A3  B1  B2  B3
// 	AN  +---+---+---+---+---+---+---+---+---+---+---+---+ (i, j)
// 	a1  | 1         | 1         |           |     1     | (0, 0)
// 	a2  | 1         |     1     | 1         |         1 | (1, 0)
// 	a3  | 1         |         1 |     1     |           | (2, 0)
// 	    +---+---+---+---+---+---+---+---+---+---+---+---+
// 	b1  |     1     | 1         | 1         | 1         | (0, 1)
// 	b2  |     1     |     1     |     1     |     1     | (1, 1)
// 	b3  |     1     |         1 |         1 |         1 | (2, 1)
// 	    +---+---+---+---+---+---+---+---+---+---+---+---+
// 	c1  |         1 | 1         |     1     |           | (0, 2)
// 	c2  |         1 |     1     |         1 | 1         | (1, 2)
// 	c3  |         1 |         1 |           |     1     | (2, 2)
// 	    +---+---+---+---+---+---+---+---+---+---+---+---+
//
// After some careful consideration while looking at the matrix it can be
// seen that the matrix entry for a position is given by
//
// | Constraint       | Symbol | Entry number               |
// | ---------------- | :----: | -------------------------- |
// | file             |   F    | j                          |
// | rank             |   R    | i + n                      |
// | diagonal         |   A    | i + j + 2n - 1             |
// | reverse diagonal |   B    | (n - 1 + i - j) + (4n - 4) |
//
// The proof is left as an exercise for the reader.

unsigned int file_index(unsigned int i, unsigned int j) {
    (void)i; // unused
    return j;
}

unsigned int rank_index(unsigned int i, unsigned int j) {
    (void)j; // unused
    return i + N;
}

unsigned int diagonal_index(unsigned int i, unsigned int j) {
    return i + j + 2 * N - 1;
}

unsigned int reverse_diagonal_index(unsigned int i, unsigned int j) {
    return i - j + 5 * (N - 1);
}

// ## Implementation
//
// Now that we know how to solve our problem let's go and actually solve it.
// First we include `stdio.h` to print and of course `dlx.h` to find solutions
// for the exact cover problem.

#include <dlx.h>
#include <stdio.h>
#include <string.h>

// The following is an utility macro to determine the sum of the length of
// the names of the constraints, or more simply put the space all of our
// identifiers will take up.
//
// We will denote the numbers of constraints by X and for simplicity we assume
// X < 100, since X depends on n we realistically anything
// over n = 15 will take an absurd amount of time to compute and shouldn't be
// calculated with this program.
//
// Every identifier is composed of a constraint symbol (F, R, A, B) and the
// number of the file, rank, etc..., so if the input is less than 10 we need
// three times X (we need to account for the null character too), and after
// we go over X > 10 every constraint needs and additional character
// to be printed.

unsigned int STRING_LENGTH(unsigned int X) {
    if (X <= 10) {
	return 3 * X + 1;
    } else {
	return 30 + 4 * (X - 10) + 1;
    }
}

// An analogous reasoning holds to compute the number of characters in the
// position names

const unsigned int POSITION_NAME_LENGTH = N / 10 + 4;

// With the previous macros now it's easy to calculate the length of th names
// of the files, ranks and diagonal identifiers

#define FILES_AND_RANKS_NAME_LENGTH STRING_LENGTH(NUMBER_OF_RANKS_AND_FILES)
#define DIAGONALS_NAME_LENGTH STRING_LENGTH(NUMBER_OF_DIAGONALS)

// And a function for printing solutions
void print_solution(dlx_solution_iterator iter);

// With everything set we can finally start

int main(void) {
    // Position names are stored individually for ease of access
    // when creating the subsets
    char pos[N][N][POSITION_NAME_LENGTH];

    // Generate position names
    for (unsigned int i = 0; i < N; ++i) {
	for (unsigned int j = 0; j < N; ++j) {
	    sprintf(pos[i][j], "%c%u%c", 'a' + j, i + 1, '\0');
	}
    }

    // Create universe and
    // generate files, ranks and diagonals and add them to the universe,
    // the order of the constraints depends on the order of addition
    // so changing the order can give incorrect results since the subsets
    // depend on this ordering to indicate their elements
    dlx_universe u = dlx_universe_new(
	&print_solution, NUMBER_OF_RANKS_AND_FILES * 2, NUMBER_OF_DIAGONALS * 2,
	NSQUARES);

    // Fill corners clockwise starting from the top left square,
    // we need to do this because corners are the only subsets with only 3
    // elements, while all the others have 4
    //
    // 	  +---+---+---+---+
    // 	4 | x |   |   | x |
    // 	  +---+---+---+---+
    // 	3 |   |   |   |   |
    // 	  +---+---+---+---+
    // 	2 |   |   |   |   |
    // 	  +---+---+---+---+
    // 	1 | x |   |   | x |
    // 	  +---+---+---+---+
    // 	    a   b   c   d
    //
    // We add them one by one

    dlx_universe_add_subset(
	u, 3, pos[N - 1][0], file_index(N - 1, 0), rank_index(N - 1, 0),
	diagonal_index(N - 1, 0));

    dlx_universe_add_subset(
	u, 3, pos[N - 1][N - 1], file_index(N - 1, N - 1),
	rank_index(N - 1, N - 1), reverse_diagonal_index(N - 1, N - 1));

    dlx_universe_add_subset(
	u, 3, pos[0][N - 1], file_index(0, N - 1), rank_index(0, N - 1),
	diagonal_index(0, N - 1));

    dlx_universe_add_subset(
	u, 3, pos[0][0], file_index(0, 0), rank_index(0, 0),
	reverse_diagonal_index(0, 0));

    // Fill inner columns
    //
    // 	  +---+---+---+---+
    // 	4 |   | x | x |   |
    // 	  +---+---+---+---+
    // 	3 |   | x | x |   |
    // 	  +---+---+---+---+
    // 	2 |   | x | x |   |
    // 	  +---+---+---+---+
    // 	1 |   | x | x |   |
    // 	  +---+---+---+---+
    // 	    a   b   c   d
    //
    // We use loops to add the remaining subsets

    for (unsigned int i = 1; i < N - 1; ++i) {
	for (unsigned int j = 0; j < N; ++j) {
	    dlx_universe_add_subset(
		u, 4, pos[i][j], file_index(i, j), rank_index(i, j),
		diagonal_index(i, j), reverse_diagonal_index(i, j));
	}
    }

    // Fill remaining lateral columns
    //
    // 	  +---+---+---+---+
    // 	4 |   |   |   |   |
    // 	  +---+---+---+---+
    // 	3 | x |   |   | x |
    // 	  +---+---+---+---+
    // 	2 | x |   |   | x |
    // 	  +---+---+---+---+
    // 	1 |   |   |   |   |
    // 	  +---+---+---+---+
    // 	    a   b   c   d

    for (unsigned int i = 0; i < N; i += N - 1) {
	for (unsigned int j = 1; j < N - 1; ++j) {
	    dlx_universe_add_subset(
		u, 4, pos[i][j], file_index(i, j), rank_index(i, j),
		diagonal_index(i, j), reverse_diagonal_index(i, j));
	}
    }

    // We can now use the algorithm to search for solutions and print them
    printf("Solutions to the %u-queens problem:\n\n", N);

    // Search for all solutions
    dlx_universe_search(u, 0);

    // Cleanup
    dlx_universe_free(u);
}

void print_solution(dlx_solution_iterator iter) {
    static unsigned int sol_number = 1;

    printf("Solution %u: ", sol_number);
    sol_number += 1;

    while (dlx_solution_iterator_remaining(iter) > 1) {
	printf("%s, ", (char *)dlx_solution_iterator_next(iter));
    }

    printf("%s.\n\n", (char *)dlx_solution_iterator_next(iter));

    dlx_solution_iterator_rewind(iter);

    char board[N][N];

    memset(board, ' ', N * N);

    for (char *next; (next = dlx_solution_iterator_next(iter)) != NULL;) {
	unsigned int row = (unsigned int)(next[0] - 'a');
	unsigned int col = (unsigned int)atoi(next + 1) - 1;
	board[row][col] = 'Q';
    }

    for (unsigned int i = 0; i < N; ++i) {
	printf("   +---");

	for (unsigned int j = 1; j < N; ++j) {
	    printf("+---");
	}

	printf("+\n");

	printf("%*u | %c ", 2, N - i, board[i][0]);

	for (unsigned int j = 1; j < N; ++j) {
	    printf("| %c ", board[i][j]);
	}

	printf("|\n");
    }

    printf("   +---");

    for (unsigned int j = 1; j < N; ++j) {
	printf("+---");
    }

    printf("+\n");

    printf("     a ");

    for (unsigned int j = 1; j < N; ++j) {
	printf("  %c ", 'a' + j);
    }

    printf("\n\n");
}
