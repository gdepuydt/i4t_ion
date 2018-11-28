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




void common_test(void) {
	buf_test();
	intern_test();
	map_test();
	strfmt_test();
}

void main_test(void) {
	common_test();
	lex_test();
}