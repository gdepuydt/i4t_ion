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


#define assert_token_int(x) (x) //TODO


void lex_test(void) {
	keyword_test();
	assert(str_intern("func") == func_keyword);

	//Integer literal tests
	init_stream(NULL, "0 2147483647 0x7fffffff 042 0b1111");
	assert_token_int(0);
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