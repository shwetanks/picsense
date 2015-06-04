#include "oculus_bridge.h"
#include "cv_functions.h"

struct attributes* OculusBridge::cv_decode(tesseract::TessBaseAPI *handle, const char *filefullname, char* dump_image) {
    cv_functions cvf;
    cv::Mat mat = cvf.deskew_image(filefullname, dump_image);

    Pix *image = pixRead(dump_image);
    handle->SetImage(image);

    struct attributes* attrib = (struct attributes *) malloc(sizeof(struct attributes));
    oc_tesseract::get_attributes(handle, attrib);

    pixDestroy(&image);
    return attrib;
}

