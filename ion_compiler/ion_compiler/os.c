#ifndef MAX_PATH
#if defined _MAX_PATH
#define MAX_PATH _MAX_PATH
#elif defined PATH_MAX
#define MAX_PATH PATH_MAX
#else
#error "No suitable MAX_PATH surrogate"
#endif
#endif