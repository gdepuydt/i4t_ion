//
//Common test
//

void buf_test(void) {

	//#define NULL (void *)0 
	int *buf = NULL;
	assert(buf_len(buf) == 0);
	int n = 1024;
	for (int i = 0; i < n; i++) {
		buf_push(buf, i);
	}
	buf_free(buf);
	assert(buf == NULL);
	assert(buf_len(buf) == 0);

	char *str = NULL;
	buf_printf(str, "One: %d\n", 1);
	assert(strcmp(str, "One: 1\n") == 0);
	buf_printf(str, "Two: 0x%x\n", 0x12345678);
	assert(strcmp(str, "One: 1\nTwo: 0x12345678\n") == 0);
}

void intern_test(void) {
	char a[] = "hello";
	str_intern(a);
	assert(strcmp(a, str_intern(a)) == 0);
	assert(str_intern(a) == str_intern(a));
	assert(str_intern(str_intern(a)) == str_intern(a));
	char b[] = "hello";
	assert(a != b);
	assert(str_intern(a) == str_intern(b));
	char c[] = "hello!";
	assert(str_intern(a) != str_intern(c));
	char d[] = "hell";
	assert(str_intern(a) != str_intern(d));
}

void map_test(void) {
	Map map = { 0 };
	enum { N = 1024 };
	for (size_t i = 1; i < N; i++) {
		map_put(&map, (void *)i, (void *)(i + 1));
	}
	for (size_t i = 1; i < N; i++) {
		void *val = map_get(&map, (void *)i);
		assert(val == (void *)(i + 1));
	}
}

void strfmt_test() {
	char *str1 = strf("%d %d", 1, 2);
	assert(strcmp(str1, "1 2") == 0);
	char *str2 = strf("%s %s", str1, str1);
	assert(strcmp(str2, "1 2 1 2") == 0);
	char *str3 = strf("%s asdf %s", str2, str2);
	assert(strcmp(str3, "1 2 1 2 asdf 1 2 1 2") == 0);
}

//
//lex_test
//

void keyword_test() {
	init_keywords();
	assert(is_keyword_name(first_keyword));
	assert(is_keyword_name(last_keyword));
	for (const char **it = keywords; it != buf_end(keywords); it++) {
		assert(is_keyword_name(*it));
	}
	assert(!is_keyword_name(str_intern("foo")));
}


#define assert_token_int(x) assert(token.int_val == (x) && match_token(TOKEN_INT))
#define assert_token_eof() assert(is_token_kind(0));
#define assert_token_float(x) assert(token.float_val == (x) && match_token(TOKEN_FLOAT))
#define assert_token_str(x) assert(strcmp(token.str_val, (x)) == 0 && match_token(TOKEN_STR))
#define assert_token(x) assert(match_token(x))
#define assert_token_name(x) assert(token.name == str_intern(x) && match_token(TOKEN_NAME))



void lex_test(void) {
	keyword_test();
	assert(str_intern("func") == func_keyword);

	//Integer literal tests
	init_stream(NULL, "0 2147483647 0x7fffffff 042 0b1111");
	assert_token_int(0);
	assert_token_int(2147483647);
	assert(token.mod == MOD_HEX);
	assert_token_int(0x7fffffff);
	assert(token.mod == MOD_OCT);
	assert_token_int(042);
	assert(token.mod == MOD_BIN);
	assert_token_int(0xF);
	assert_token_eof();

	// Float literal tests
	init_stream(NULL, "3.14 .123 42. 3e10");
	assert_token_float(3.14);
	assert_token_float(.123);
	assert_token_float(42.);
	assert_token_float(3e10);
	assert_token_eof();

	// Char literal tests
	init_stream(NULL, "'a' '\\n'");
	assert_token_int('a');
	assert_token_int('\n');
	assert_token_eof();

	// String literal tests
	init_stream(NULL, "\"foo\" \"a\\nb\"");
	assert_token_str("foo");
	assert_token_str("a\nb");
	assert_token_eof();

	// Operator tests
	init_stream(NULL, ": := + += ++ < <= << <<=");
	assert_token(TOKEN_COLON);
	assert_token(TOKEN_COLON_ASSIGN);
	assert_token(TOKEN_ADD);
	assert_token(TOKEN_ADD_ASSIGN);
	assert_token(TOKEN_INC);
	assert_token(TOKEN_LT);
	assert_token(TOKEN_LTEQ);
	assert_token(TOKEN_LSHIFT);
	assert_token(TOKEN_LSHIFT_ASSIGN);
	assert_token_eof();

	// Misc tests
	init_stream(NULL, "XY+(XY)_HELLO1,234+994");
	assert_token_name("XY");
	assert_token(TOKEN_ADD);
	assert_token(TOKEN_LPAREN);
	assert_token_name("XY");
	assert_token(TOKEN_RPAREN);
	assert_token_name("_HELLO1");
	assert_token(TOKEN_COMMA);
	assert_token_int(234);
	assert_token(TOKEN_ADD);
	assert_token_int(994);
	assert_token_eof();
}

#undef assert_token
#undef assert_token_name
#undef assert_token_int
#undef assert_token_float
#undef assert_token_str
#undef assert_token_eof

void parse_test(void) {
	const char *decls[] = {
		"var x: char[256] = {1, 2, 3, ['a'] = 4};",
		"struct Vector { x, y: float; }",
		"var v = Vector{x = 1.0, y = -1.0};",
		"var v: Vector = {1.0, -1.0};",
		"const n = sizeof(:int*[16]);",
		"const n = sizeof(1+2);",
		"var x = b == 1 ? 1+2 : 3-4;",
		"func fact(n: int): int { trace(\"fact\"); if (n == 0) { return 1; } else { return n * fact(n-1); } }",
		"func fact(n: int): int { p := 1; for (i := 1; i <= n; i++) { p *= i; } return p; }",
		"var foo = a ? a&b + c<<d + e*f == +u-v-w + *g/h(x,y) + -i%k[x] && m <= n*(p+q)/r : 0;",
		"func f(x: int): bool { switch (x) { case 0: case 1: return true; case 2: default: return false; } }",
		"enum Color { RED = 3, GREEN, BLUE = 0 }",
		"const pi = 3.14;",
		"union IntOrFloat { i: int; f: float; }",
		"typedef Vectors = Vector[1+2];",
		"func f() { do { print(42); } while(1); }",
		"typedef T = (func(int):int)[16];",
		"func f() { enum E { A, B, C } return; }",
		"func f() { if (1) { return 1; } else if (2) { return 2; } else { return 3; } }",
	};

	for (const char **it = decls; it != decls + sizeof(decls) / sizeof(*decls); it++) {
		init_stream(NULL, *it);
		Decl *decl = parse_decl();
		//print_decl(decl);
		//printf("\n");
	}
}


void common_test(void) {
	buf_test();
	intern_test();
	map_test();
	strfmt_test();
}

void main_test(void) {
	common_test();
	lex_test();
	//parse_test();
}