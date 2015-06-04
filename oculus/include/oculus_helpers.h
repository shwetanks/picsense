#ifndef _OCULUS_HELPERS_H_
#define _OCULUS_HELPERS_H_

#include <string.h>
#include <ctype.h>
#include <stdint.h>

/* true if 80% alphanum */
inline bool is_nearly_alphanum (const char* word) {
  uint32_t count = 0, i = 0;
  for (; i < strlen(word); i++) {
    if (isascii(word[i]))
      count++;
  }
  return ((count/strlen(word)) > 0.8) && (strlen(word) > 2);
}

#endif /* _OCULUS_HELPERS_H_ */
