#ifndef _FONTLI_INIT_DEFS_H_
#define _FONTLI_INIT_DEFS_H_

struct fontli_config {
    const char *tesseract_custom_data;
    char *image_set;
    const char *language_override;
    const char *out_dir;
};

#endif /* _FONTLI_INIT_DEFS_H_ */
