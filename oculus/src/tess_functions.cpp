#include "tess_functions.h"
#include "oculus_helpers.h"

#include <stdint.h>



tesseract::TessBaseAPI*
oc_tesseract::tess_image_handle (const char* tess_training_set, const char* lang, tesseract::PageSegMode mode) {

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if (api->Init(tess_training_set, lang, tesseract::OEM_DEFAULT)) {
        fprintf(stderr, "tesseract init failed");
        return NULL;
    }
    api->SetPageSegMode(mode);
    return api;
}

void oc_tesseract::get_attributes(tesseract::TessBaseAPI* handle, struct attributes* attrib) {

    handle->Recognize(0);
    tesseract::ResultIterator* ri = handle->GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_WORD;

    uint8_t confidence = 0;
    FONT_TYPE font_type = UNKNOWN;
    char txt[1024];

    if (ri != 0) {
        do {
            const char* word = ri->GetUTF8Text(level);
            if (ri->Confidence(level) < GLOBAL_CONFIDENCE_THRESHOLD)
                confidence = 0;
            else
                confidence = 1;

            attrib->confidence += confidence;

            if (word != 0) { // && is_nearly_alphanum(word)) {
                ri->WordFontAttributes (&attrib->fat->is_bold, &attrib->fat->is_italic,
                                        &attrib->fat->is_underlined, &attrib->fat->is_monospace,
                                        &attrib->fat->is_serif, &attrib->fat->is_smallcaps,
                                        &attrib->fat->point_size, &attrib->fat->font_id);

                font_type = font_type | ( attrib->fat->is_serif ? SERIF : SANS_SERIF);
                if (attrib->fat->is_monospace)
                    font_type = font_type | MONOSPACE;

                strncat(txt, word, strlen(word));
                strncat(txt, " ", 1);
            }
            delete[] word;
        } while (ri->Next(level));

        attrib->text = txt;
        attrib->font = font_type;
    }
}
