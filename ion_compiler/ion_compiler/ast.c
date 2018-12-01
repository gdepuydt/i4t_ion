Arena ast_arena;

const char *parse_name(void) {
	const char *name = token.name;
	expect_token(TOKEN_NAME);
	return name;
}

void *ast_alloc(size_t size) {
	assert(size != 0);
	void *ptr = arena_alloc(&ast_arena, size);
	memset(ptr, 0, size);
	return ptr;
}


void *ast_dup(const void *src, size_t size) {
	if (size == 0) {
		return NULL;
	}
	void *ptr = arena_alloc(&ast_arena, size);
	memcpy(ptr, src, size);
	return ptr;
}

#define AST_DUP(x) ast_dup(x, num_##x * sizeof(*x))

NoteList note_list(Note *notes, size_t num_notes) {
	return (NoteList) { AST_DUP(notes), num_notes };
}


Expr *expr_new(ExprKind kind, SrcPos pos) {
	Expr *e = ast_alloc(sizeof(Expr));
	e->kind = kind;
	e->pos = pos;
	return e;
}


Expr *expr_ternary(SrcPos pos, Expr *cond, Expr *then_expr, Expr *else_expr) {
	Expr *e = expr_new(EXPR_TERNARY, pos);
	e->ternary.cond = cond;
	e->ternary.then_expr = then_expr;
	e->ternary.else_expr = else_expr;
	return e;
}