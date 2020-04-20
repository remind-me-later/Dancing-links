#ifndef VECT_H
#define VECT_H

/*
 * vect.h -- type-safe generic dynamic array
 * made by sixthgear. BSD Licenced.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VECT_CAPACITY 256

/* vector type for use in type-unsafe functions */
/* macros will generate type-safe functions for each type called in */
/* VECT_GENERATE_TYPE */

typedef struct {
	unsigned int size;
	unsigned int capacity;
	size_t data_size;
	void *data;
} vect;

void
vect_err(const char *error)
{
	fprintf(stderr, "%s\n", error);
	exit(1);
}
                                                                    
vect *
vect_init(size_t data_size, unsigned int capacity)
{
	vect *v = malloc(sizeof(vect));	
	if (v == NULL)
		vect_err("vect_init: allocation of vect failed.");		
	if (capacity == 0)
		capacity = VECT_CAPACITY;			
	v->data = malloc(data_size * capacity);	
	if (v->data == NULL)
		vect_err("vect_init: allocation of vect data failed.");
	v->data_size = data_size;
	v->size = 0;
	v->capacity = capacity;
	return v;
}

void
vect_free(void *v)
{
	free(((vect *)v)->data);
	free(((vect *)v));
}

void
vect_chk_bounds(vect *v, unsigned int pos) {
	if (pos < v->size)
		vect_err("vect_set: out of bounds.");
}

unsigned int
vect_rem(vect *v, unsigned int pos)
{
	vect_chk_bounds((vect*)v, pos);
	char *dest = (char *) v->data + (v->data_size * pos);
	char *src = dest + v->data_size;
	size_t num_bytes = v->data_size * (v->size - pos - 1);
	memmove(dest, src, num_bytes);
	v->size--;	
	return pos;
}

unsigned int
vect_ins(vect *v, unsigned int pos)
{
	vect_chk_bounds((vect*)v, pos);
	char *src = (char *) v->data + (v->data_size * pos);
	char *dest = src + v->data_size;
	size_t num_bytes = v->data_size * (v->size - pos);
	memmove(dest, src, num_bytes);
	v->size++;
	return pos;
}

/* vector type for use in type-safe functions */
/* functions for each type must be generated by VECT_GENERATE_TYPE(type) */
/* or VECT_GENERATE_NAME(type, name) for pointer types or multiple qualifiers */

#define vect_at(v, pos) v->data[pos]

#define vect_push(v, value)                                   		       \
{                                                                              \
	if (v->size == v->capacity) {					       \
		v->capacity *= 2;					       \
		v->data = realloc(v->data, v->data_size * v->capacity);	       \
		if (v->data == NULL)					       \
			vect_err("vect_resize: resize failed.");	       \
	}								       \
	v->data[v->size++] = value;                                 	       \
}

#define vect_pop(v) v->data[--v->size]

#define VECT_GENERATE_TYPE(TYPE) VECT_GENERATE_NAME(TYPE, TYPE) 
#define VECT_GENERATE_NAME(TYPE, NAME)					       \
typedef struct {	                                                       \
	unsigned int size;                                                     \
	unsigned int capacity;                                                 \
	size_t data_size;                                                      \
	TYPE *data;                                                            \
} vect_##NAME;                                                                 \
vect_##NAME *                                                                  \
vect_init_##NAME(unsigned int capacity)                                        \
{                                                                              \
	return (vect_##NAME *) vect_init(sizeof(TYPE), capacity);              \
}                                                                              \
void                                                                           \
vect_rem_##NAME(vect_##NAME *v, unsigned int pos)                              \
{                                                                              \
	vect_rem((vect *) v, pos);                             		       \
}                                                                              \
void                                                                           \
vect_ins_##NAME(vect_##NAME *v, unsigned int pos, TYPE value)                  \
{                                                                              \
	v->data[vect_ins((vect *) v, pos)] = value;                            \
}                                                                              \

#endif
