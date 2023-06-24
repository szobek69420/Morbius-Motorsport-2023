#include "meinmath.h"

static void swap(float* distance1, float* distance2, int* index1, int* index2)
{
	long temp;
	temp = *((long*)distance1);
	(*distance1) = (*distance2);
	(*distance2) = *((float*)(&temp));

	temp = *index1;
	(*index1) = (*index2);
	(*index2) = temp;
}

void quicksort_physics(float* dist, int* indices, int low, int high)//custom quicksort a physics-hez, nem art, ha inline
{
	if (low >= high) return;

	int i;
	float pivot = *(dist + high);
	int smaller = low - 1;

	for (i = low; i <= high; i++)
	{
		if (*(dist + i) <= pivot)
		{
			smaller++;
			swap(dist + i, dist + smaller, indices + i, indices + smaller);
		}
	}

	quicksort_physics(dist, indices, low, smaller - 1);
	quicksort_physics(dist, indices, smaller + 1, high);

	return;
}

int cm_abs_int(int szam)
{
	if (szam < 0)
		return -szam;
	return szam;
}