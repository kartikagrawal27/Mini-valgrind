#include <stdlib.h>
#include <stdio.h>
#include "mini_valgrind.h"
int main() {
  // your tests here using malloc and free
  // Do NOT modify this line
	char * t = (char*)malloc(10);
	t[0] = 0;
	free(t);
	free(t);
	int i;
	free(&i);
	void * f = malloc(sizeof(int));
	*((int *)f) = 200;
	void * j = malloc(sizeof(int));
	j++;
	free(j);
	int * c = (int*) malloc(sizeof(int));
	*c = 7;
	free(c);
	
  atexit(print_report);
  return 0;
}

