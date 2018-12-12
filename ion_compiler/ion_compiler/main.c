#if _MSC_VER >= 1900 || __STDC_VERSION__ >= 201112L
// Visual Studio 2015 supports enough C99/C11 features for us.
#else
#error "C11 support required or Visual Studio 2015 or later"
#endif
#define __USE_MINGW_ANSI_STDIO 1
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
#include <limits.h>
#include <inttypes.h>

bool flag_verbose;
bool flag_lazy;
bool flag_notypeinfo;
bool flag_fullgen;

#include "../../../i4t_lib/src/common.c"
#include "os.c"
#include "lex.c"
#include "type.c"
#include "ast.h"
#include "ast.c"
#include "parse.c"
#include "targets.c"
#include "resolve.c"
#include "ion.c"

#include "test.c"


int main(int argc, char **argv) {
	main_test();
	printf("Finished!\n");
	getchar();
}
