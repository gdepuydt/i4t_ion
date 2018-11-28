#define _CRT_SECURE_NO_WARNINGS

#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

#include "common.c"
#include "lex.c"
#include "test.c"


int main(int argc, char **argv) {
	main_test();
	printf("Finished!\n");
	getchar();
}

//test to file