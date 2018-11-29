typedef enum SymKind {
	SYM_NONE,
	SYM_VAR,
	SYM_CONST,
	SYM_FUNC,
	SYM_TYPE,
}SymKind;

typedef enum SymState {
	SYM_UNRESOLVED,
	SYM_RESOLVING,
	SYM_RESOLVED,
}SymState;

typedef union Val {
	bool b;
	char c;
	unsigned char uc;
	signed char sc;
	short s;
	unsigned short us;
	int i;
	unsigned u;
	long l;
	unsigned long ul;
	long long ll;
	unsigned long long ull;
}Val;

typedef struct Sym {
	const char *name;
	SymKind kind;
	SymState state;
	Decl *decl;
	Type *type;
	Val val;
}Sym;