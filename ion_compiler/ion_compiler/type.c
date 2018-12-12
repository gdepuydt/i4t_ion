typedef struct Type Type;
typedef struct Sym Sym;

typedef enum TypeKind {
	TYPE_NONE,
	TYPE_INCOMPLETE,
	TYPE_COMPLETING,
	TYPE_VOID,
	TYPE_BOOL,
	TYPE_CHAR,
	TYPE_SCHAR,
	TYPE_UCHAR,
	TYPE_SHORT,
	TYPE_USHORT,
	TYPE_INT,
	TYPE_UINT,
	TYPE_LONG,
	TYPE_ULONG,
	TYPE_LLONG,
	TYPE_ULLONG,
	TYPE_ENUM,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_PTR,
	TYPE_FUNC,
	TYPE_ARRAY,
	TYPE_STRUCT,
	TYPE_UNION,
	TYPE_CONST,
	NUM_TYPE_KINDS,
}TypeKind;

typedef struct TypeField {
	const char *name;
	Type *type;
	size_t offset;
}TypeField;

struct Type {
	TypeKind kind;
	size_t size;
	size_t align;
	Sym *sym;
	Type *base;
	int typeid;
	bool nonmodifiable;
	union {
		size_t num_elems;
		struct {
			TypeField *fields;
			size_t num_fields;
		} aggregate;
		struct {
			Type **params;
			size_t num_params;
			bool has_varargs;
			Type *ret;
		} func;
	};
};


int next_typeid = 1;
Map typeid_map;

void register_typeid(Type *type) {
	map_put(&typeid_map, (void*)(uintptr_t)type->typeid, type);
}

Type *type_alloc(TypeKind kind) {
	Type *type = xcalloc(1, sizeof(Type));
	type->kind = kind;
	type->typeid = next_typeid++;
	register_typeid(type);
	return type;
}

Map cached_ptr_types;

typedef struct TypeMetrics {
	size_t size;
	size_t align;
	bool sign;
	unsigned long long max;
}TypeMetrics;

TypeMetrics *type_metrics;

Type *type_void = &(Type) { TYPE_VOID };
Type *type_bool = &(Type) { TYPE_BOOL };
Type *type_char = &(Type) { TYPE_CHAR };
Type *type_uchar = &(Type) { TYPE_UCHAR };
Type *type_schar = &(Type) { TYPE_SCHAR };
Type *type_short = &(Type) { TYPE_SHORT };
Type *type_ushort = &(Type) { TYPE_USHORT };
Type *type_int = &(Type) { TYPE_INT };
Type *type_uint = &(Type) { TYPE_UINT };
Type *type_long = &(Type) { TYPE_LONG };
Type *type_ulong = &(Type) { TYPE_ULONG };
Type *type_llong = &(Type) { TYPE_LLONG };
Type *type_ullong = &(Type) { TYPE_ULLONG };
Type *type_float = &(Type) { TYPE_FLOAT };
Type *type_double = &(Type) { TYPE_DOUBLE };

//these are set in target.c depending on the os and architecture
Type *type_uintptr;
Type *type_usize;
Type *type_ssize;

//type_metrics is platform dependent and implemented in the targets.c file
Type *type_ptr(Type *base) {
	Type *type = map_get(&cached_ptr_types, base);
	if (!type) {
		type = type_alloc(TYPE_PTR);
		type->size = type_metrics[TYPE_PTR].size;
		type->align = type_metrics[TYPE_PTR].align;
		type->base = base;
		map_put(&cached_ptr_types, base, type);
	}
	return type;

}

typedef struct CachedArrayType {
	Type *type;
	struct CachedArrayType *next;
} CachedArrayType;

Map cached_array_types;

Type *type_array(Type *base, size_t num_elems) {
	uint64_t hash = hash_mix(hash_ptr(base), hash_uint64(num_elems));
	uint64_t key = hash ? hash : 1;
	CachedArrayType *cached = map_get_from_uint64(&cached_array_types, key);
	for (CachedArrayType *it = cached; it; it = it->next) {
		Type *type = it->type;
		if (type->base == base && type->num_elems == num_elems) {
			return type;
		}
	}
	complete_type(base);
	Type *type = type_alloc(TYPE_ARRAY);
	type->nonmodifiable = base->nonmodifiable;
	type->size = num_elems * type_sizeof(base);
	type->align = type_alignof(base);
	type->base = base;
	type->num_elems = num_elems;
	CachedArrayType *new_cached = xmalloc(sizeof(CachedArrayType));
	new_cached->type = type;
	new_cached->next = cached;
	map_put_from_uint64(&cached_array_types, key, new_cached);
	return type;
}

