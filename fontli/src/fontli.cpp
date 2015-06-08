#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <fcntl.h>

#include "fontli.h"

const char TESS_DATA[] = "data";
char IMAGE_SET[] = "images";
const char ENG[]       = "eng";
const char OUT_DATA[]  = "out";

fontli_config fontli::default_config = {
    /* tesseract_custom_data */          TESS_DATA,
    /* images to process */              IMAGE_SET,
    /* + delimited language overrides */ ENG,
    /* output data dir */                OUT_DATA
};

fontli_config* fontli::config() {
    static struct fontli_config* config = NULL;
    if (unlikely(NULL == config)) {
        size_t sz = sizeof(fontli_config);
        if (sz < PAGESIZE)
            sz = PAGESIZE;
        sz += PAGEMASK;
        sz &= ~PAGEMASK;
        void *mem = mmap(NULL, sz, PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
        if (MAP_FAILED == mem) {
            perror("mmap conf_init");
            abort();
        }
        config = (fontli_config *)mem;
        memcpy(config, &default_config, sizeof(fontli_config));
    }
    return config;
}


static struct option longopts[] = {
    {"tess-data", 1, 0, 'd'},
    {"image-set", 1, 0, 'i'},
    {"lang-override", 2, 0, 'l'},
    {"out-data", 1, 0, 'o'},
    {"help", 0, 0, 1},
    {0, 0, 0, 0}
};

void usage (char *arg0) {
    if (arg0 == NULL) {
        printf("fontli: font recognition\n");
        return;
    }

    size_t num_options = sizeof(longopts)/sizeof(struct option);

    printf("Usage:\n\t%s ", arg0);
    for (size_t i=0; i < num_options; i++) {
        if (NULL == longopts[i].name && 0 == longopts[i].val)
            continue;
        printf("\n\t\t[");
        if (NULL != longopts[i].name)
            printf("--%s", longopts[i].name);
        if (NULL != longopts[i].name && isalpha(longopts[i].val))
            printf("/");
        if (isalpha(longopts[i].val))
            printf("-%c", longopts[i].val);
        if (0 != longopts[i].has_arg)
            printf(" <val>");
        printf("]");
    }
    printf("\n");
    exit (0);
}

int fontli::parse_config (int argc, char *argv[]) {

    while (1) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "d:i:o:l::", longopts, &option_index);
        if (c == -1)
            break;
        switch (c) {
        case 'd':
            fontli::config()->tesseract_custom_data = optarg;
            break;
        case 'i':
            fontli::config()->image_set = optarg;
            break;
        case 'l':
            fontli::config()->language_override = optarg;
            break;
        case 'o':
            fontli::config()->out_dir = optarg;
            break;
        default:
            usage(argv[0]);
            break;
        }
    }
    return 0;
}

int mkdir_all (char *name) {
    char *cur = name;
    while (*cur) {
        ++cur;
        char* p = strchrnul(cur, '/');
        char c= *p;
        *p = 0;
        if (0 > mkdir(name, 0755) && errno != EEXIST)
            return -1;
        cur = p;
        *p = c;
    }
    return 0;
}

int move_file (char* file_fullname, char* dst_filename) {
    if (rename(file_fullname, dst_filename)) {
        fprintf(stderr, "\nfile move failed|<%s> to <%s>\n", file_fullname, dst_filename);
        return -1;
    }
    return 0;
}

void classify_by_attributes (struct attributes* attrib, char* file_fullname, char* file_basename) {
    char target[256];
    if (attrib->confidence >= CONFIDENCE_INTERVAL) {
        sprintf(target,
                "%s/%s/%s/%s",
                fontli::config()->out_dir,
                font_confidence_str(static_cast<int>(HIGH_CONFIDENCE)),
                font_name(static_cast<int>(attrib->font)),
                file_basename);
    } else {
        sprintf(target,
                "%s/%s/%s/%s",
                fontli::config()->out_dir,
                font_confidence_str(static_cast<int>(LOW_CONFIDENCE)),
                font_name(static_cast<int>(attrib->font)),
                file_basename);
    }
    move_file(file_fullname, target);
    fprintf(stdout, "at %d confidence %s moved to %s\n", attrib->confidence, file_fullname, dirname(target));
}

int fontli::fontli_execute() {

    bool source_is_dir = false;

    struct stat s;
    if (0 == stat(fontli::config()->image_set, &s) && (s.st_mode & S_IFDIR))
        source_is_dir = true;
    else if (s.st_mode & S_IFREG)
        source_is_dir = false;
    else
        exit (EXIT_FAILURE);

    OculusBridge ob (fontli::config()->tesseract_custom_data, fontli::config()->language_override);
    tesseract::TessBaseAPI* handle = oc_tesseract::tess_image_handle (
                       ob.tess_training_set,
                       ob.language_overrides,
                       tesseract::PSM_SINGLE_BLOCK);

    char dump_dir[] = "transformed_images";
    if (0 > mkdir_all(dump_dir))
        return -1;

    char required_dirs[256];
    for (int cr = 0; cr < CONFIDENCE_MAX; cr++) {
        for (int ft = 0; ft < FONT_MAX; ft++) {
            sprintf(required_dirs, "%s/%s/%s", fontli::config()->out_dir, font_confidence_str(cr), font_name(ft));
            if (0 > mkdir_all(required_dirs))
                return -1;
            required_dirs[0] = 0;
        }
    }
        

    if (source_is_dir) {
        DIR *dp;
        struct dirent *dirp;
        if ((dp = opendir(fontli::config()->image_set)) == NULL)
            exit (EXIT_FAILURE);

        while ((dirp = readdir(dp)) != NULL) {
            struct stat st;
            char qualified_name[256];
            sprintf(qualified_name, "%s/%s", fontli::config()->image_set, dirp->d_name);
            if (stat(qualified_name, &st) == -1)
                continue;
            if ((st.st_mode & S_IFMT) == S_IFDIR)
                continue;

            char tr_name[256];
            sprintf(tr_name, "%s/tr_%s", dump_dir, basename(qualified_name));
            struct attributes *attrib = ob.cv_decode(handle, qualified_name, tr_name);
            classify_by_attributes(attrib, qualified_name, dirp->d_name);
            fflush(stdout);
        }
    } else {
        char tr_name[256];
        char *filename = basename(fontli::config()->image_set);
        sprintf(tr_name, "%s/tr_%s", dump_dir, filename);
        struct attributes *attrib = ob.cv_decode(handle, fontli::config()->image_set, tr_name);
        classify_by_attributes(attrib, fontli::config()->image_set, filename);
        fflush(stdout);
    }

    handle->End();
    return 0;
}

int main (int argc, char* argv[]) {
    fontli fnt;
    fnt.parse_config(argc, argv);
    fnt.fontli_execute();
}
