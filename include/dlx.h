#ifndef __DLX_H__
#define __DLX_H__

#include <stdbool.h>

#define DLX_PRIMARY true
#define DLX_SECONDARY false

#define DLX_ALL 0

/* Objects */

typedef struct Universe *dlx_univ_t;

/* Functions */
dlx_univ_t dlx_create_universe(void (*sol_handler)(void**, unsigned int));

void dlx_delete_universe(dlx_univ_t u);

void dlx_add_constraint(dlx_univ_t u, bool primary, void *ref);

void dlx_add_subset(dlx_univ_t u, unsigned int size, void *ref, ...);

void dlx_search(dlx_univ_t u, unsigned int nsol);

#endif
