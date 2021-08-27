#ifndef __DLX_H__
#define __DLX_H__

#include <stdlib.h>

#define DLX_ALL 0

/* Objects */

typedef struct dlx_universe_struct *dlx_universe;

/* Functions */
dlx_universe dlx_universe_new(
    void (*solution_handler)(void **, size_t),
    size_t number_of_primary_constraints,
    size_t number_of_secondary_constraints, size_t number_of_subsets);

void dlx_universe_delete(dlx_universe universe);

void dlx_universe_add_subset(
    dlx_universe universe, size_t subset_size, void *subset_label, ...);

void dlx_universe_search(
    dlx_universe universe, unsigned int desired_number_of_solutions);

#endif
