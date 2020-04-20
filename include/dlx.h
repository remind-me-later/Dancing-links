#ifndef __DLX_H__
#define __DLX_H__

#define DLX_PRIMARY 1
#define DLX_SECONDARY 0

/* Objects */

typedef struct Universe *dlx_universe;

/* Functions */

struct Universe *dlx_create_universe();

void dlx_delete_universe(struct Universe *universe);

void dlx_add_constraint(struct Universe *u, char primary, void *ref);

void dlx_add_subset(struct Universe *universe, unsigned int size,
                    void *ref, ...);

void dlx_create_links(struct Universe *u);

void dlx_search(struct Universe *universe, unsigned int nsol);

void *dlx_pop_solution(struct Universe *universe);

void dlx_print_universe(struct Universe *universe, char *cformat,
                        char *ssformat);

void dlx_print_solutions(struct Universe *u, char *format);

#endif
