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

#include "../../../i4t_lib/src/common.c"
#include "lex.c"
#include "type.c"
#include "ast.h"
#include "ast.c"
#include "parse.c"
#include "resolve.c"

#include "test.c"


int main(int argc, char **argv) {
	main_test();
	printf("Finished!\n");
	getchar();
}
