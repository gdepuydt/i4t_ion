Expr *parse_expr(void);
Expr *parse_expr_unary(void);

Typespec *parse_type() {
	//TODO
}


Expr *parse_expr_compound() {
	//TODO
}

Expr *parse_expr_operand(void) {
	SrcPos pos = token.pos;
	if (is_token_kind(TOKEN_INT)) {
		unsigned long long val = token.int_val;
		TokenMod mod = token.mod;
		TokenSuffix suffix = token.suffix;
		next_token();
		return expr_int(pos, val, mod, suffix);
	}
	else if (is_token_kind(TOKEN_FLOAT)) {
		double val = token.float_val;
		TokenSuffix suffix = token.suffix;
		next_token();
		return expr_float(pos, val, suffix);
	}
	else if (is_token_kind(TOKEN_STR)) {
		const char *val = token.str_val;
		TokenMod mod = token.mod;
		next_token();
		return expr_str(pos, val, mod);
	}
	else if (is_token_kind(TOKEN_NAME)) {
		const char *name = token.name;
		next_token();
		if (is_token_kind(TOKEN_LBRACE)) {
			return parse_expr_compound(typespec_name(pos, name));
		}
		else {
			return expr_name(pos, name);
		}
	}
	else if (match_keyword(sizeof_keyword)) {
		expect_token(TOKEN_LPAREN);
		if (match_token(TOKEN_COLON)) {
			Typespec *type = parse_type();
			expect_token(TOKEN_RPAREN);
			return expr_sizeof_type(pos, type);
		}
		else {
			Expr *expr = parse_expr();
			expect_token(TOKEN_RPAREN);
			return expr_sizeof_expr(pos, expr);
		}
	}
	else if (is_token_kind(TOKEN_LBRACE)) {
		return parse_expr_compound(NULL);
	}
	else if (match_token(TOKEN_LPAREN)) {
		if (match_token(TOKEN_COLON)) {
			Typespec *type = parse_type();
			expect_token(TOKEN_RPAREN);
			if (is_token_kind(TOKEN_LBRACE)) {
				return parse_expr_compound(type);
			}
			else {
				return expr_cast(pos, type, parse_expr_unary());
			}
		}
		else {
			Expr *expr = parse_expr();
			expect_token(TOKEN_RPAREN);
			return expr;
		}
	}
	else {
		fatal_error_here("Unexpected token %s in expression", token_info());
		return NULL;
	}
}

Expr *parse_expr_base(void) {
	Expr *expr = parse_expr_operand();
	while (is_token_kind(TOKEN_LPAREN) || is_token_kind(TOKEN_LBRACKET) || is_token_kind(TOKEN_DOT)) {
		SrcPos pos = token.pos;
		if (match_token(TOKEN_LPAREN)) {
			Expr **args = NULL;
			if (!is_token_kind(TOKEN_RPAREN)) {
				buf_push(args, parse_expr());
				while (match_token(TOKEN_COMMA)) {
					buf_push(args, parse_expr());
				}
			}
			expect_token(TOKEN_RPAREN);
			expr = expr_call(pos, expr, args, buf_len(args));
		}
		else if (match_token(TOKEN_LBRACKET)) {
			Expr *index = parse_expr();
			expect_token(TOKEN_RBRACKET);
			expr = expr_index(pos, expr, index);
		}
		else {
			assert(is_token_kind(TOKEN_DOT));
			next_token();
			const char *field = token.name;
			expect_token(TOKEN_NAME);
			expr = expr_field(pos, expr, field);
		}
	}
	return expr;
}

bool is_unary_op(void) {
	return
		is_token_kind(TOKEN_ADD) ||
		is_token_kind(TOKEN_SUB) ||
		is_token_kind(TOKEN_MUL) ||
		is_token_kind(TOKEN_AND) ||
		is_token_kind(TOKEN_NEG) ||
		is_token_kind(TOKEN_NOT);
}

Expr *parse_expr_unary(void) {
	if (is_unary_op()) {
		SrcPos pos = token.pos;
		TokenKind op = token.kind;
		next_token();
		return expr_unary(pos, op, parse_expr_unary());
	}
	else {
		return parse_expr_base();
	}
}

bool is_mul_op(void) {
	return TOKEN_FIRST_MUL <= token.kind && token.kind <= TOKEN_LAST_MUL;
}

Expr *parse_expr_mul(void) {
	Expr *expr = parse_expr_unary();
	while (is_mul_op()) {
		SrcPos pos = token.pos;
		TokenKind op = token.kind;
		next_token();
		expr = expr_binary(pos, op, expr, parse_expr_unary());
	}
	return expr;
}

bool is_add_op(void) {
	return TOKEN_FIRST_ADD <= token.kind && token.kind <= TOKEN_LAST_ADD;
}

Expr *parse_expr_add(void) {
	Expr *expr = parse_expr_mul();
	while (is_add_op()) {
		SrcPos pos = token.pos;
		TokenKind op = token.kind;
		next_token();
		expr = expr_binary(pos, op, expr, parse_expr_mul());
	}
	return expr;
}

bool is_cmp_op(void) {
	return TOKEN_FIRST_CMP <= token.kind && token.kind <= TOKEN_LAST_CMP;
}

Expr *parse_expr_cmp(void) {
	Expr *expr = parse_expr_add();
	while (is_cmp_op()) {
		SrcPos pos = token.pos;
		TokenKind op = token.kind;
		next_token();
		expr = expr_binary(pos, op, expr, parse_expr_add());
	}
	return expr;
}

Expr *parse_expr_and(void) {
	Expr *expr = parse_expr_cmp();
	while (match_token(TOKEN_AND_AND)) {
		SrcPos pos = token.pos;
		expr = expr_binary(pos, TOKEN_AND_AND, expr, parse_expr_cmp());
	}
	return expr;
}

Expr *parse_expr_or(void) {
	Expr *expr = parse_expr_and();
	while (match_token(TOKEN_OR_OR)) {
		SrcPos pos = token.pos;
		expr = expr_binary(pos, TOKEN_OR_OR, expr, parse_expr_and());
	}
	return expr;
}

Expr *parse_expr_ternary(void) {
	SrcPos pos = token.pos;
	Expr *expr = parse_expr_or();
	if (match_token(TOKEN_QUESTION)) {
		Expr *then_expr = parse_expr_ternary();
		expect_token(TOKEN_COLON);
		Expr *else_expr = parse_expr_ternary();
		expr = expr_ternary(pos, expr, then_expr, else_expr);
	}
	return expr;
}


Expr *parse_expr(void) {
	return parse_expr_ternary();
}


NoteList parse_note_list(void) {
	Note *notes = NULL;
	while (match_token(TOKEN_AT)) {
		buf_push(notes, (Note) { .pos = token.pos, .name = parse_name() });
	}
	return note_list(notes, buf_len(notes));
}


EnumItem parse_decl_enum_item(void) {
	SrcPos pos = token.pos;
	const char *name = parse_name();
	Expr *init = NULL;
	if (match_token(TOKEN_ASSIGN)) {
		init = parse_expr();
	}
	return (EnumItem){ pos, name, init };
}


Decl *parse_decl_enum(SrcPos pos) {
	const char *name = parse_name();
	expect_token(TOKEN_LBRACE);
	EnumItem *items = NULL;
	while (!is_token_kind(TOKEN_RBRACE)) {
		buf_push(items, parse_decl_enum_item());
		if (!match_token(TOKEN_COMMA)) {
			break;
		}
	}
}

Decl *parse_decl_opt(void) {
	SrcPos pos = token.pos;
	if (match_keyword(enum_keyword)) {
		return parse_decl_enum(pos);
	}
	//TODO fill in all other declaration types
}


Decl *parse_decl(void) {
	NoteList notes = parse_note_list();
	Decl *decl = parse_decl_opt();
	if (!decl) {
		fatal_error_here("Expected declaration keyword, got %s", token_info());
	}
	decl->notes = notes;
	return decl;
}
