#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "searches.h"

#include <stdio.h>


int *BinarySearch(const int *arr, size_t len, int to_find)
{
	assert(NULL != arr);
	assert(0 < len);

	len /= 2;
	arr += len;

	while (0 < len)
	{
		len /=2;

		if (to_find > *arr)
		{
			arr += len;
		}
		else if (to_find < *arr)
		{
			arr -= len;
		}
		else
		{
			return (int *)arr;
		}
	}

	if (to_find >= *arr)
	{
		return (int *)arr;
	}
	else if (to_find == *(arr - 1))
	{
		return (int *)(arr - 1);
	}

	return NULL;
}

int *BinarySearchRec(const int *arr, size_t len, int to_find)
{
	assert(NULL != arr);

	if (1 == len)
	{
		if (*arr == to_find)
		{
			return (int *)arr;
		}
		else
		{
			return NULL;
		}
	}

	len /= 2;

	if (*(arr + len) > to_find)
	{
		return (int *)(BinarySearchRec(arr, len, to_find));
	}
	else if (*arr < to_find)
	{
		return (int *)(BinarySearchRec(arr + len, len, to_find));
	}

	return (int *)arr;
}

int *JumpSearch(const int *arr, size_t len, int to_find)
{
	int sqrt_len = 0;
	int *end = (int *)arr + len;

	assert(NULL != arr);
	assert(0 < len);

	sqrt_len = sqrt(len);

	while (*arr < to_find && arr < end)
	{
		arr += sqrt_len;
	}

	arr -= sqrt_len;

	while (0 < sqrt_len)
	{
		if (*arr == to_find)
		{
			return (int *)arr;
		}

		++arr;
		--sqrt_len;
	}

	if (*arr != to_find)
	{
		return NULL;
	}

	return (int *)arr;
}