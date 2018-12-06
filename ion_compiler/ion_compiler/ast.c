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

Typespec *typespec_new(TypespecKind kind, SrcPos pos) {
	Typespec *t = ast_alloc(sizeof(Typespec));
	t->kind = kind;
	t->pos = pos;
	return t;
}

Typespec *typespec_array(SrcPos pos, Typespec *elem, Expr *size) {
	Typespec *t = typespec_new(TYPESPEC_ARRAY, pos);
	t->base = elem;
	t->num_elems = size;
	return t;
}

Typespec *typespec_func(SrcPos pos, Typespec **args, size_t num_args, Typespec *ret, bool has_varargs) {
	Typespec *t = typespec_new(TYPESPEC_FUNC, pos);
	t->func.args = AST_DUP(args);
	t->func.num_args = num_args;
	t->func.ret = ret;
	t->func.has_varargs = has_varargs;
	return t;
}

Typespec *typespec_name(SrcPos pos, const char *name) {
	Typespec *t = typespec_new(TYPESPEC_NAME, pos);
	t->name = name;
	return t;
}

Typespec *typespec_ptr(SrcPos pos, Typespec *base) {
	Typespec *t = typespec_new(TYPESPEC_PTR, pos);
	t->base = base;
	return t;
}

Typespec *typespec_const(SrcPos pos, Typespec *base) {
	Typespec *t = typespec_new(TYPESPEC_CONST, pos);
	t->base = base;
	return t;
}

Expr *expr_new(ExprKind kind, SrcPos pos) {
	Expr *e = ast_alloc(sizeof(Expr));
	e->kind = kind;
	e->pos = pos;
	return e;
}

Expr *expr_compound(SrcPos pos, Typespec *type, CompoundField *fields, size_t num_fields) {
	Expr *e = expr_new(EXPR_COMPOUND, pos);
	e->compound.type = type;
	e->compound.fields = AST_DUP(fields);
	e->compound.num_fields = num_fields;
	return e;
}

Expr *expr_cast(SrcPos pos, Typespec *type, Expr *expr) {
	Expr *e = expr_new(EXPR_CAST, pos);
	e->cast.type = type;
	e->cast.expr = expr;
	return e;
}

Expr *expr_sizeof_expr(SrcPos pos, Expr *expr) {
	Expr *e = expr_new(EXPR_SIZEOF_EXPR, pos);
	e->sizeof_expr = expr;
	return e;
}

Expr *expr_sizeof_type(SrcPos pos, Typespec *type) {
	Expr *e = expr_new(EXPR_SIZEOF_TYPE, pos);
	e->sizeof_type = type;
	return e;
}

Expr *expr_int(SrcPos pos, unsigned long long val, TokenMod mod, TokenSuffix suffix) {
	Expr *e = expr_new(EXPR_INT, pos);
	e->int_lit.val = val;
	e->int_lit.mod = mod;
	e->int_lit.suffix = suffix;
	return e;
}

Expr *expr_float(SrcPos pos, double val, TokenSuffix suffix) {
	Expr *e = expr_new(EXPR_FLOAT, pos);
	e->float_lit.val = val;
	e->float_lit.suffix = suffix;
	return e;
}

Expr *expr_str(SrcPos pos, const char *val, TokenMod mod) {
	Expr *e = expr_new(EXPR_STR, pos);
	e->str_lit.val = val;
	e->str_lit.mod = mod;
	return e;
}

Expr *expr_name(SrcPos pos, const char *name) {
	Expr *e = expr_new(EXPR_NAME, pos);
	e->name = name;
	return e;
}

Expr *expr_call(SrcPos pos, Expr *expr, Expr **args, size_t num_args) {
	Expr *e = expr_new(EXPR_CALL, pos);
	e->call.expr = expr;
	e->call.args = AST_DUP(args);
	e->call.num_args = num_args;
	return e;
}


Expr *expr_index(SrcPos pos, Expr *expr, Expr *index) {
	Expr *e = expr_new(EXPR_INDEX, pos);
	e->index.expr = expr;
	e->index.index = index;
	return e;
}

Expr *expr_field(SrcPos pos, Expr *expr, const char *name) {
	Expr *e = expr_new(EXPR_FIELD, pos);
	e->field.expr = expr;
	e->field.name = name;
	return e;
}

Expr *expr_unary(SrcPos pos, TokenKind op, Expr *expr) {
	Expr *e = expr_new(EXPR_UNARY, pos);
	e->unary.op = op;
	e->unary.expr = expr;
	return e;
}

Expr *expr_binary(SrcPos pos, TokenKind op, Expr *left, Expr *right) {
	Expr *e = expr_new(EXPR_BINARY, pos);
	e->binary.op = op;
	e->binary.left = left;
	e->binary.right = right;
	return e;
}

Expr *expr_ternary(SrcPos pos, Expr *cond, Expr *then_expr, Expr *else_expr) {
	Expr *e = expr_new(EXPR_TERNARY, pos);
	e->ternary.cond = cond;
	e->ternary.then_expr = then_expr;
	e->ternary.else_expr = else_expr;
	return e;
}

Decl *decl_new(DeclKind kind, SrcPos pos, const char *name) {
	Decl *d = ast_alloc(sizeof(Decl));
	d->kind = kind;
	d->pos = pos;
	d->name = name;
	return d;
}

Decl *decl_enum(SrcPos pos, const char *name, EnumItem *items, size_t num_items) {
	Decl *d = decl_new(DECL_ENUM, pos, name);
	d->enum_decl.items = AST_DUP(items);
	d->enum_decl.num_items = num_items;
	return d;
}

Decl *decl_aggregate(SrcPos pos, DeclKind kind, const char *name, AggregateItem *items, size_t num_items) {
	assert(kind == DECL_STRUCT || kind == DECL_UNION);
	Decl *d = decl_new(kind, pos, name);
	d->aggregate.items = AST_DUP(items);
	d->aggregate.num_items = num_items;
	return d;
}

Decl *decl_const(SrcPos pos, const char *name, Expr *expr) {
	Decl *d = decl_new(DECL_CONST, pos, name);
	d->const_decl.expr = expr;
	return d;
}

Decl *decl_typedef(SrcPos pos, const char *name, Typespec *type) {
	Decl *d = decl_new(DECL_TYPEDEF, pos, name);
	d->typedef_decl.type = type;
	return d;
}

Decl *decl_func(SrcPos pos, const char *name, FuncParam *params, size_t num_params, Typespec *ret_type, bool has_varargs, StmtList block) {
	Decl *d = decl_new(DECL_FUNC, pos, name);
	d->func.params = AST_DUP(params);
	d->func.num_params = num_params;
	d->func.ret_type = ret_type;
	d->func.has_varargs = has_varargs;
	d->func.block = block;
	return d;
}