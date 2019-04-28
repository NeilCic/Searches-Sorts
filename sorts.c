#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "sorts.h"

#define NEXT(x) (void *)((char *)(x) + elem_size)
#define PREV(x) (void *)((char *)(x) - elem_size)
#define ELEMENTOFFSET i * elem_size
#define ARRAY_VALUE_RANGE (max) - (min) + 1
#define EXTRACTARROFFSETVALUE ext_value((char *)arr + ELEMENTOFFSET, param)
#define ABS(x) ((x) < 0)? -x : x
#define PARAM ((parameters_t *)param)
#define I_OFFSET(x) ((char *)(x) + (i - 1) * elem_size)


static void Swap(void *a, void *b, void *swapper, size_t elem_size)
{
	memcpy(swapper, a, elem_size);
	memcpy(a, b, elem_size);
	memcpy(b, swapper, elem_size);
}

static void SwapInt(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

/*goes over the array and swaps pair when necessary*/
void BubbleSort(void *arr, const size_t elem_num, const size_t elem_size,
									is_before_t is_before, void *const params)
{
	char *element = NULL, *swapper = NULL;
	size_t i = 0, j = 0;

	assert(arr);
	assert(elem_num > 0);
	assert(elem_size > 0);
	assert(is_before);

	swapper = malloc(elem_size);
	if (!swapper)
	{
		return;
	}

	for (; i < elem_num - 1; ++i)
	{
		element = arr;
		for (j = 0; j < elem_num - 1; ++j)
		{
			if (!is_before(element, NEXT(element), params))
			{
				Swap(element, NEXT(element), swapper, elem_size);
			}

			element = NEXT(element);
		}
	}

	free (swapper);
}

/*starting from start + 1, and sorts each element between those they've passed*/
void InsertionSort(void *arr, const size_t elem_num, const size_t elem_size, 
									is_before_t is_before, void *const params)
{
	void *element = NULL, *swapper = NULL;
	size_t i = 1;

	assert(arr);
	assert(elem_num > 0);
	assert(elem_size > 0);
	assert(is_before);

	swapper = malloc(elem_size);
	if (!swapper)
	{
		return;
	}

	for (; i < elem_num; ++i)
	{
		element = (void *)((char *)arr + elem_size * i);

		while (is_before(element, PREV(element), params) && element > arr)
		{
			Swap(element, PREV(element), swapper, elem_size);
			element = PREV(element);
		}
	}

	free(swapper);
}

/*finds minimum and swaps with element, then goes over next element ...*/
void SelectionSort(void *arr, const size_t elem_num, const size_t elem_size, 
									is_before_t is_before, void *const params)
{
	void *element = NULL, *end = NULL, *min = 0, *swapper = NULL;
	size_t i = 0;

	assert(arr);
	assert(elem_num > 0);
	assert(elem_size > 0);
	assert(is_before);

	swapper = malloc(elem_size);
	if (!swapper)
	{
		return;
	}

	end = (void *)((char *)(arr) + elem_size * elem_num);

	for (; i < elem_num; ++i)
	{
		element = (void *)((char *)arr + elem_size * i);
		min = element;

		while (element < end)
		{
			if (is_before(element, min, params))
			{
				min = element;
			}
			
			element = NEXT(element);
		}

		Swap(min, element, swapper, elem_size);
	}

	free(swapper);
}

static void LUTinit(void *arr, size_t elem_num, size_t elem_size, int min,
					extract_func_t ext_value, void* param, int *count_arr)
{
	unsigned int i = 0;

	for (i = 0; i < elem_num; ++i)
	{
		++count_arr[ext_value((char *)arr + ELEMENTOFFSET, param) - min];
	}
}

static void AGGREGATELUT(int *count_arr, int min, int max)
{
	unsigned int i = 0;

	for (i = 1; i < (unsigned int)ARRAY_VALUE_RANGE; ++i)
	{
		count_arr[i] +=	count_arr[i - 1];
	}
}

static void SortIntoNewArray(void *arr, size_t elem_num, size_t elem_size,
			int min, extract_func_t ext_value, void* param, int *count_arr,
			void * sorted_arr)
{
	long offset = 0;
	unsigned int i = 0;

	for (i = elem_num; i > 0; --i) /*sorting into new array*/
	{
		offset = count_arr[ext_value(I_OFFSET(arr), param) - min];
		memcpy(((char *)sorted_arr + (offset - 1) * elem_size),
													I_OFFSET(arr), elem_size);
		--count_arr[ext_value(I_OFFSET(arr), param) - min];
	}
}

/*assuming int arrays only*/
int CountingSort(void *arr, const size_t elem_num, const size_t elem_size,
				  int max, int min, extract_func_t ext_value, void *param)
{
	int *count_arr = NULL; 
	void *sorted_arr = NULL;

	assert(arr);
	assert(elem_num > 0);
	assert(elem_size > 0);
	assert(ext_value);

	count_arr = calloc(ARRAY_VALUE_RANGE, sizeof(int));
	if (!count_arr)
	{
		return -1;
	}
	sorted_arr = calloc(elem_num, elem_size);
	if (!sorted_arr)
	{
		free(count_arr);
		return -1;
	}

	LUTinit(arr, elem_num, elem_size, min, ext_value, param, count_arr);

	AGGREGATELUT(count_arr, min, max);

	SortIntoNewArray(arr, elem_num, elem_size, min, ext_value,
								param, count_arr, sorted_arr);

	memcpy(arr, sorted_arr, elem_num * elem_size);

	free(count_arr);
	free(sorted_arr);

	return 0;
}

/*performs counting sort without knowing min and max of array, finds by itself*/
int AutoCountingSort(void *arr, const size_t elem_num, const size_t elem_size,
										extract_func_t ext_value, void *param)
{
	void *element = NULL, *end = NULL;
	long min = 0, max = 0;

	assert(arr);
	assert(elem_num > 0);
	assert(elem_size > 0);
	assert(ext_value);

	end = (void *)((char *)(arr) + elem_size * elem_num);

	element = arr;
	while (element < end) /*find min*/
	{
		if (ext_value(element, param) < min)
		{
			min = ext_value(element, param);
		}
		element = NEXT(element);
	}
	
	element = arr;
	while (element < end) /*find max*/
	{
		if (ext_value(element, param) > max)
		{
			max = ext_value(element, param);
		}
		element = NEXT(element);
	}

	return CountingSort(arr, elem_num, elem_size, max, min, ext_value, param);
}

typedef struct parameters_t
{
	size_t value_chunk_size;
	void *user_param;
	extract_func_t ext_value;
	unsigned int digit_isolator;
}parameters_t;

static int ext_valueRADIX(void const *element, void *param)
{
	int value = PARAM->ext_value(element, PARAM->user_param);

	return (value / PARAM->digit_isolator %	(int)PARAM->value_chunk_size);
}

void FindMaxAbsoluteNumber(void *arr, size_t elem_num, size_t elem_size,
				extract_func_t ext_value, void *param, unsigned int *max_digits)
{
	unsigned int i = 0, current_value = 0;

	for (i = 0; i < elem_num; ++i) /*finding max absolute number*/
	{
		current_value = ABS(ext_value(((char *)arr + i * elem_size), param));
		if (*max_digits < current_value)
		{
			*max_digits = current_value;
		}
	}
}

void SendToCountSort(void *arr, size_t elem_num, size_t elem_size,
			extract_func_t ext_value, void *param, unsigned int *max_digits,
			size_t value_chunk_size)
{
	parameters_t parameters = {0};

	parameters.value_chunk_size = value_chunk_size;
	parameters.user_param = param;
	parameters.ext_value = ext_value;
	parameters.digit_isolator = 1;

	for (; *max_digits > 0 ; *max_digits /= 10)
	{
		AutoCountingSort(arr, elem_num, elem_size, ext_valueRADIX, &parameters);
		parameters.digit_isolator *=  value_chunk_size;
	}
}

int RadixSort(void *arr, size_t elem_num, size_t elem_size,
		extract_func_t ext_value, void *const param, size_t value_chunk_size)
{
	unsigned int max_digits = 0;

	assert(arr);
	assert(elem_num > 0);
	assert(elem_size > 0);
	assert(ext_value);

	FindMaxAbsoluteNumber(arr, elem_num, elem_size, ext_value, param, 
																&max_digits);

	SendToCountSort(arr, elem_num, elem_size, ext_value, param, &max_digits,
															value_chunk_size);

	return 0;
}


static void *MergeArrays(void *arr, void* arr1, size_t len1, void *arr2,
			size_t len2, size_t elem_size, is_before_t is_before, void *param)
{
	if (len1 > 0 && len2 > 0)
	{
		if (is_before(arr1, arr2, param))
		{
			memcpy(arr, arr1, elem_size);
			len1 -= 1;
			MergeArrays(((char *)arr + elem_size), ((char *)arr1 + elem_size), 
				len1, arr2, len2, elem_size, is_before, param);
		}
		else
		{
			memcpy(arr, arr2, elem_size);
			len2 -= 1;
			MergeArrays(((char *)arr + elem_size), arr1, len1, 
				((char *)arr2 + elem_size), len2, elem_size,
				is_before, param);
		}
	}
	else if (len1 > 0 && len2 == 0)
	{
		memcpy(arr, arr1, len1 * elem_size);
		len1 = 0;
	}
	else if (len2 > 0 && len1 == 0)
	{
		memcpy(arr, arr2, len2 * elem_size);
		len2 = 0;
	}

	return arr;
}

static void *RecursiveMerge(void *arr, const size_t elem_num, 
		const size_t elem_size, is_before_t is_before, void *const param)
{
	void *arr1 = NULL, *arr2 = NULL, *swapper = NULL;
	size_t len1 = 0, len2 = 0;
	if (elem_num == 2)
	{
		if (is_before(((char *)arr + elem_size), arr, param))
		{
			swapper = malloc(elem_size);
			if (NULL == swapper)
			{
				return NULL;
			}
			Swap((char *)arr + elem_size, arr, swapper, elem_size);
			free(swapper);
		}
		
		return arr;
	}
	else if (elem_num == 1)
	{
		return arr;
	}

	if (0 != (elem_num % 2))
	{
		len1 = ((elem_num / 2) + 1);
		arr1 = malloc(len1 * elem_size);
		if (NULL == arr1)
		{
			return NULL;
		}
		
		memcpy(arr1, arr, (len1 * elem_size));

		arr1 = RecursiveMerge(arr1, len1, elem_size, is_before, param);
		if (NULL == arr1)
		{
			return NULL;
		}
	}
	else
	{
		len1 = ((elem_num / 2));
		arr1 = malloc(len1 * elem_size);
		if (NULL == arr1)
		{
			return NULL;
		}
		
		memcpy(arr1, arr, len1 * elem_size);
		
		arr1 = RecursiveMerge(arr1, len1, elem_size, is_before, param);
		if (NULL == arr1)
		{
			return NULL;
		}
	}

	len2 = (elem_num / 2);
	arr2 = malloc(len2 * elem_size);
	if (NULL == arr2)
	{
		free(arr1);
		return NULL;
	}
	
	memcpy(arr2, ((char *)arr + len1 * elem_size), len2 * elem_size);

	arr2 = RecursiveMerge(arr2, len2, elem_size, is_before, param);
	if (NULL == arr2)
	{
		return NULL;
	}

	return MergeArrays(arr, arr1, len1, arr2, len2, elem_size, is_before, param);
}


int MergeSort(void *arr, const size_t elem_num, const size_t elem_size,
								is_before_t is_before, void *const param)
{
	assert(arr);
	assert(elem_num);
	assert(elem_size);
	assert(is_before);

	if (0 == RecursiveMerge(arr, elem_num, elem_size, is_before, param))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void QuickSort(int *arr, size_t len)
{
	int *right_index = arr + len - 1;
	int *left_index = arr;

	assert(NULL != arr);
	assert(len > 0);

	while (left_index < right_index)
	{
		while (left_index < right_index)
		{
			if (*left_index > *right_index)
			{
				SwapInt(right_index, left_index);
				--right_index;
				break;
			}

			++left_index;
		}

		while (right_index > left_index)
		{
			if (*left_index > *right_index)
			{
				SwapInt(left_index, right_index);
				++left_index;
				break;
			}

			--right_index;
		}
	}

	if (1 < left_index - arr)
	{
		QuickSort(arr, left_index - arr);
	}


	if (1 < arr + len - right_index)
	{
		QuickSort(right_index, arr + len - right_index);
	}
}