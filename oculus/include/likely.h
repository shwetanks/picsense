#ifndef _LIKELY_H_
#define _LIKELY__H_

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#endif // _LIKELY_H_
