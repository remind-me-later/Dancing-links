#ifndef __DLX_H__
#define __DLX_H__

#include <stdlib.h>

#define DLX_ALL 0

/* Objects */

typedef struct dlx_universe *dlx_universe;
typedef struct dlx_solution_iterator *dlx_solution_iterator;

/* Functions */
dlx_universe dlx_universe_new(
    void (*solution_handler)(dlx_solution_iterator iter),
    size_t number_of_primary_constraints,
    size_t number_of_secondary_constraints, size_t number_of_subsets);

void dlx_universe_free(dlx_universe universe);

void dlx_universe_add_subset(
    dlx_universe universe, size_t subset_size, void *subset_label, ...);

void dlx_universe_search(
    dlx_universe universe, unsigned int desired_number_of_solutions);

void dlx_solution_iterator_rewind(struct dlx_solution_iterator *iter);

void *dlx_solution_iterator_next(struct dlx_solution_iterator *iter);

size_t dlx_solution_iterator_remaining(struct dlx_solution_iterator *iter);

#endif
