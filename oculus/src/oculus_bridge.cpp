#include "oculus_bridge.h"
#include "cv_ah_functions.h"

struct attributes* OculusBridge::cv_decode(tesseract::TessBaseAPI *handle, const char *file_fullname, char* dump_image) {
    cv_ah_functions cvaffineh;
    Pix *image;
    fprintf(stdout, "processing | %s\n", file_fullname);
    if (0 < cvaffineh.deskew_image(file_fullname, dump_image))
        image = pixRead(dump_image);
    else
        image = pixRead(file_fullname);

    handle->SetImage(image);

    struct attributes* attrib = (struct attributes *) malloc(sizeof(struct attributes));
    attrib->file_name = strdup(file_fullname);
    oc_tesseract::get_attributes(handle, attrib);

    pixDestroy(&image);
    return attrib;
}

