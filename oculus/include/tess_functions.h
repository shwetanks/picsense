#ifndef _PICSENSE_TESS_FUNCTIONS_H_
#define _PICSENSE_TESS_FUNCTIONS_H_

#include "../../depends/include/tesseract/baseapi.h"
#include "../../depends/include/leptonica/allheaders.h"
#include "oculus_attribute_defs.h"

class oc_tesseract {

public:
    static tesseract::TessBaseAPI* tess_image_handle(const char* training_data, const char* lang, tesseract::PageSegMode mode);
    static void get_attributes(tesseract::TessBaseAPI* handle, struct attributes* attribs);
};

#endif /* _PICSENSE_TESS_FUNCTIONS_H_ */
