#ifndef _FONTLI_H_
#define _FONTLI_H_

#include "fontli_init_defs.h"
#include "oculus.h"

class fontli {

public:
    enum { PAGEMASK = 4095, PAGESIZE };
    static int parse_config(int argc, char *argv[]);
    int fontli_execute();

public:
    static struct fontli_config* config();
    static struct fontli_config default_config;
/*
private:
    int categorize(tesseract::TessBaseAPI* handle, const char *filename);*/
};

#endif /* _FONTLI_H_ */

