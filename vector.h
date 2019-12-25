#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdlib.h>

#define CREATE_VECTOR_1(TYPE) CREATE_VECTOR_2(TYPE. TYPE)

#define CREATE_VECTOR_2(TYPE, NAME)                                     \
	typedef struct {                                                        \
		TYPE* at;                                                       \
		size_t len, capacity;                                           \
	} NAME##_vector;                                                        \
	\
	struct Vector_##NAME                                                    \
	new_##NAME##_vector(unsigned int capacity)                              \
	{                                                                       \
		struct Vector_##NAME vector;                                    \
		\
		vector.at = malloc(capacity * sizeof(*vector.at));              \
		vector.capacity = capacity;                                     \
		vector.len = 0;                                                 \
		\
		return vector;                                                  \
	}                                                                       \
	\
	void                                                                    \
	NAME##_vector_resize(NAME##_vector *vector)                             \
	{                                                                       \
		vector->capacity *= 2;                                          \
		vector->at = realloc(vector->at,                                \
		                     vector->capacity * sizeof(*vector->at));   \
	}                                                                       \
	\
	void                                                                    \
	NAME##_vector_push(NAME##_Vector *vector, TYPE data)                    \
	{                                                                       \
		if (vector->len + 1 == vector->capacity)                        \
			NAME##_vector_resize(vector);                           \
		\
		vector->at[vector->len++] = data;                               \
	}                                                                       \
	\
	TYPE                                                                    \
	NAME##_vector_pop(NAME##_Vector *vector)                                \
	{                                                                       \
		return vector->len == 0 ? NULL : vector->at[--vector->len];     \
	}                                                                       \

#endif
