typedef enum Os {
	OS_WIN32,
	OS_LINUX,
	OS_OSX,
	NUM_OSES,
} Os;

const char *os_names[NUM_OSES] = {
	 [OS_WIN32] = "win32",
	[OS_LINUX] = "linux",
	[OS_OSX] = "osx",
};

typedef enum Arch {
	ARCH_X64,
	ARCH_X86,
	NUM_ARCHES,
} Arch;

const char *arch_names[NUM_ARCHES] = {
	[ARCH_X64] = "x64",
	[ARCH_X86] = "x86",
};

int target_os;
int target_arch;

int get_os(const char *name) {
	for (int i = 0; i < NUM_OSES; i++) {
		if (strcmp(os_names[i], name) == 0) {
			return i;
		}
	}
	return -1;
}

int get_arch(const char *name) {
	for (int i = 0; i < NUM_ARCHES; i++) {
		if (strcmp(arch_names[i], name) == 0) {
			return i;
		}
	}
	return -1;
}

#define DEFAULT_TYPE_METRICS \
    [TYPE_BOOL] = {.size = 1, .align = 1}, \
    [TYPE_CHAR] = {.size = 1, .align = 1, .max = 0x7f, .sign = true}, \
    [TYPE_SCHAR] = {.size = 1, .align = 1, .max = 0x7f, .sign = true}, \
    [TYPE_UCHAR] = {.size = 1, .align = 1, .max = 0xff}, \
    [TYPE_SHORT] = {.size = 2, .align = 2, .max = 0x7fff, .sign = true}, \
    [TYPE_USHORT] = {.size = 2, .align = 2, .max = 0xffff}, \
    [TYPE_INT] = {.size = 4, .align = 4, .max = 0x7fffffff, .sign = true}, \
    [TYPE_UINT] = {.size = 4, .align = 4, .max = 0xffffffff}, \
    [TYPE_LLONG] = {.size = 8, .align = 8, .max = 0x7fffffffffffffff, .sign = true}, \
    [TYPE_ULLONG] = {.size = 8, .align = 8, .max = 0xffffffffffffffff}, \
    [TYPE_FLOAT] = {.size = 4, .align = 4}, \
    [TYPE_DOUBLE] = {.size = 8, .align = 8}

TypeMetrics win32_x86_metrics[NUM_TYPE_KINDS] = {
	 DEFAULT_TYPE_METRICS,
	[TYPE_PTR] = {.size = 4,.align = 4},
	[TYPE_LONG] = {.size = 4,.align = 4,.max = 0x7fffffff,.sign = true},
	[TYPE_ULONG] = {.size = 4,.align = 4,.max = 0x7fffffff,.sign = true},
};

TypeMetrics win32_x64_metrics[NUM_TYPE_KINDS] = {
	DEFAULT_TYPE_METRICS,
	[TYPE_PTR] = {.size = 8,.align = 8},
	[TYPE_LONG] = {.size = 4,.align = 4,.max = 0x7fffffff,.sign = true},
	[TYPE_ULONG] = {.size = 4,.align = 4,.max = 0x7fffffff,.sign = true},
};

TypeMetrics ilp32_metrics[NUM_TYPE_KINDS] = {
	DEFAULT_TYPE_METRICS,
	[TYPE_PTR] = {.size = 4,.align = 4},
	[TYPE_LONG] = {.size = 4,.align = 4,.max = 0x7fffffff,.sign = true},
	[TYPE_ULONG] = {.size = 4,.align = 4,.max = 0x7fffffff,.sign = true},
};

TypeMetrics lp64_metrics[NUM_TYPE_KINDS] = {
	DEFAULT_TYPE_METRICS,
	[TYPE_PTR] = {.size = 8,.align = 8},
	[TYPE_LONG] = {.size = 8,.align = 8,.max = 0x7fffffffffffffff,.sign = true},
	[TYPE_ULONG] = {.size = 8,.align = 8,.max = 0xffffffffffffffff,.sign = true},
};

void init_target(void) {
	type_metrics = NULL;
	switch (target_os) {
	case OS_WIN32:
		switch (target_arch) {
		case ARCH_X86:
			type_metrics = win32_x86_metrics;
			break;
		case ARCH_X64:
			type_metrics = win32_x64_metrics;
			break;
		default:
			break;
		}
		break;
	case OS_LINUX:
		switch (target_arch) {
		case ARCH_X86:
			type_metrics = ilp32_metrics;
			break;
		case ARCH_X64:
			type_metrics = lp64_metrics;
			break;
		default:
			break;
		}
		break;
	case OS_OSX:
		switch (target_arch) {
		case ARCH_X64:
			type_metrics = lp64_metrics;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	if (!type_metrics) {
		printf("Unsupported os/arch combination: %s/%s\n", os_names[target_os], arch_names[target_arch]);
		exit(1);
	}
	if (type_metrics[TYPE_PTR].size == 4) {
		type_uintptr = type_uint;
		type_usize = type_uint;
		type_ssize = type_int;
	}
	else {
		assert(type_metrics[TYPE_PTR].size == 8);
		type_uintptr = type_ullong;
		type_usize = type_ullong;
		type_ssize = type_llong;
	}
}

//TODO



