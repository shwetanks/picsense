#ifndef _OCULUS_ATTRIBUTE_DEFS_H_
#define _OCULUS_ATTRIBUTE_DEFS_H_

#include <stdbool.h>
#include <stdint.h>

#define ENUM_TO_STRING(x) \
    case x:               \
    return (#x)

struct font_attributes {
    bool is_bold;
    bool is_italic;
    bool is_underlined;
    bool is_monospace;
    bool is_serif;
    bool is_smallcaps;
    int point_size;
    int font_id;
};


enum FONT_TYPE {
    UNKNOWN     = 0,
    SANS_SERIF  = 1,
    SERIF       = 2,
    MONOSPACE   = 3,
    HANDWRITING = 4,
    FONT_MAX
};
//static inline FONT_TYPE operator|=(FONT_TYPE a, FONT_TYPE b) {return static_cast<FONT_TYPE>(static_cast<int>(a) | static_cast<int>(b));}

struct attributes {
//    struct font_attributes *fat;
    char *file_name;
    char *text;
    enum FONT_TYPE font;
    uint8_t confidence;
};

static inline char const* font_name (int i) {
    switch (i) {
        ENUM_TO_STRING(SANS_SERIF);
        ENUM_TO_STRING(SERIF);
        ENUM_TO_STRING(MONOSPACE);
        ENUM_TO_STRING(HANDWRITING);
        ENUM_TO_STRING(UNKNOWN);
    }
    return NULL;
}

enum FONT_CONFIDENCE_RANKS {
    HIGH_CONFIDENCE = 0,
    LOW_CONFIDENCE  = 1,
    CONFIDENCE_MAX
};

static inline char const* font_confidence_str (int i) {
    switch (i) {
        ENUM_TO_STRING(HIGH_CONFIDENCE);
        ENUM_TO_STRING(LOW_CONFIDENCE);
    }
    return NULL;
}

#define GLOBAL_CONFIDENCE_THRESHOLD 50 //50/100:50% per word in textline
#define CONFIDENCE_INTERVAL 6 //8/10:80%

#endif /* _OCULUS_ATTRIBUTE_DEFS_H_ */
