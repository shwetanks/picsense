#ifndef _OCULUS_BRIDGE_H_
#define _OCULUS_BRIDGE_H_

#include "tess_functions.h"

class OculusBridge {
public:
    OculusBridge(const char* ts, const char* lang) :
        tess_training_set(ts),
        language_overrides(lang)
    {};

    static struct attributes* cv_decode(tesseract::TessBaseAPI *handle, const char *filefullname, char* dump_image);
    //static struct attributes* simple_decode();
    const char *tess_training_set;
    const char *language_overrides;

};

#endif /* _OCULUS_BRIDGE_H_ */
