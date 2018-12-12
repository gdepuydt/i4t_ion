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
	struct Package *home_package;
	SymKind kind;
	SymState state;
	uint8_t reachable;
	Decl *decl;
	const char *external_name;
	union {
		struct {
			Type *type;
			Val val;
		};
		struct Package *package;
	};
} Sym;


typedef struct Package {
	const char *path;
	char full_path[MAX_PATH];
	Decl **decls;
	size_t num_decls;
	Map syms_map;
	Sym **syms;
	const char *external_name;
	bool always_reachable;
} Package;

Map package_map;

Package *current_package;

Package *enter_package(Package *new_package) {
	Package *old_package = current_package;
	current_package = new_package;
	return old_package;
}

void leave_package(Package *old_package) {
	current_package = old_package;
}

Sym **sorted_syms;

Type *complete_aggregate(Type *type, Aggregate *aggregate) {
	//TODO
}

void complete_type(Type *type) {
	if (type->kind == TYPE_COMPLETING) {
		fatal_error(type->sym->decl->pos, "Type completion cycle");
		return;
	}
	else if (type->kind != TYPE_INCOMPLETE) {
		return;
	}
	Sym *sym = type->sym;
	Package *old_package = enter_package(sym->home_package);
	Decl *decl = sym->decl;
	if (decl->is_incomplete) {
		fatal_error(decl->pos, "Trying to use incomplete type as complete type");
	}
	type->kind = TYPE_COMPLETING;
	assert(decl->kind == DECL_STRUCT || decl->kind == DECL_UNION);
	complete_aggregate(type, decl->aggregate);
	buf_push(sorted_syms, type->sym);
	leave_package(old_package);
}