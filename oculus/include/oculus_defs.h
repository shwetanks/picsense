#ifndef _OCULUS_DEFS_H_
#define _OCULUS_DEFS_H_

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define _INLINE_ static inline

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

#ifndef UNUSED_ARG
#define UNUSED_ARG __attribute__((unused))
#endif


#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#endif // _OCULUS_DEFS_H_
