#include "tess_functions.h"
#include "oculus_helpers.h"
#include "oculus_defs.h"

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

    double confidence = 0;

    char txt[1024];

    bool is_bold;
    bool is_italic;
    bool is_underlined;
    bool is_monospace;
    bool is_serif;
    bool is_smallcaps;

    int point_size;
    int font_id;
    int i;
    int font_relative = 0;
    uint32_t num_words;

    FONT_TYPE fonts[] = {UNKNOWN, SANS_SERIF, SERIF, MONOSPACE, HANDWRITING};
    uint16_t font_freq[FONT_MAX];
    for (i = 0; i < FONT_MAX; ++i)
        font_freq[i] = 0;

    attrib->font = UNKNOWN;
    attrib->confidence = 0;

    if (ri != 0) {
        do {
            int ft = UNKNOWN;
            const char* word = ri->GetUTF8Text(level);

            if (word != 0 && is_nearly_alphanum(word)) {
                ++num_words;
                if (ri->Confidence(level) >= GLOBAL_CONFIDENCE_THRESHOLD)
                    confidence += ri->Confidence(level);

                ri->WordFontAttributes (&is_bold, &is_italic,
                                        &is_underlined, &is_monospace,
                                        &is_serif, &is_smallcaps,
                                        &point_size, &font_id);

                ft = (1ULL << (is_serif ? SERIF : SANS_SERIF));
                ++font_freq[ft];

                if (is_monospace)
                    ++font_freq[MONOSPACE];

                strncat(txt, word, strlen(word));
                strncat(txt, " ", 1);
                delete[] word;
            }
        } while (ri->Next(level));

        attrib->text = txt;
        if (0 < confidence && 0 < num_words)
            attrib->confidence = ((int)((double)confidence / num_words)) % 10;

        i = 0;

        for (; i < FONT_MAX; ++i) {
            if ((0 < font_freq[i]) && (font_relative < font_freq[i])) {
                font_relative = font_freq[i];
                attrib->font = fonts[i];
            }
        }
    }
    fprintf(stdout, "processed %s to with %u words "
            "\n\t for attribute set \n \t\t|| "
            "\n\t\t marked confidence:%f | "
            "\n\t\t attrib confidence: %u| "
            "\n\t\t text: %s| "
            "\n\t\t font: %s at average weight: %d\n",
            attrib->file_name,
            num_words,
            confidence,
            attrib->confidence,
            attrib->text,
            font_name(attrib->font),
            font_relative);

    delete ri;
}
