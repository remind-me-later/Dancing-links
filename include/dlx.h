#ifndef __DLX_H__
#define __DLX_H__

#define DLX_PRIMARY 1
#define DLX_SECONDARY 0

/* Objects */

typedef struct Universe *dlx_univ_t;

/* Functions */

dlx_univ_t dlx_create_universe();

void dlx_delete_universe(dlx_univ_t u);

void dlx_add_constraint(dlx_univ_t u, char primary, void *ref);

void dlx_add_subset(dlx_univ_t u, unsigned int size, void *ref, ...);

void dlx_search(dlx_univ_t u, unsigned int nsol);

void *dlx_pop_solution(dlx_univ_t u, unsigned int *size);

void dlx_print_universe(dlx_univ_t u, char *cfmt, char *ssfmt);

void dlx_print_solutions(dlx_univ_t u, char *fmt);

unsigned int dlx_found_solutions(struct Universe *u);

#endif
