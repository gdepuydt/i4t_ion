Expr *parse_expr_or(void) {
	//TODO
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
