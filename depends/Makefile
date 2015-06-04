PWD=$(shell pwd)

ifndef OPTFLAGS
OPTFLAGS=-O3
endif

ifndef DEBUG
ENABLE_DEBUG=0
endif

TESSERACT_VERSION=3.02
LEPTONICA_VERSION=1.72
MAKE_VERBOSE=1

CFLAGS+=$(OPTFLAGS) -g -Wall -Wno-uninitialized
TESSERACT_FLAGS+=

ifeq ($(ENABLE_DEBUG), 1)
CFLAGS+=-DDEBUG
TESSERACT_FLAGS+=--enable-debug
else
CFLAGS+=-DNDEBUG
endif

TESS_DIR=tesseract-$(TESSERACT_VERSION)
LEPT_DIR=leptonica-$(LEPTONICA_VERSION)

buildall:
	@$(MAKE) lib/liblept.a
	@$(MAKE) lib/libtesseract.a

$(LEPT_DIR)/Makefile:
	cd $(LEPT_DIR); ./configure --prefix=$(PWD)

lib/liblept.a:
	@$(MAKE) $(LEPT_DIR)/Makefile
	@$(MAKE) -C $(LEPT_DIR) V=$(MAKE_VERBOSE) CFLAGS="$(CFLAGS)" install

$(TESS_DIR)/Makefile:
	cd $(TESS_DIR); ./autogen.sh
	cd $(TESS_DIR); ./configure LIBLEPT_HEADERSDIR=$(PWD)/include/leptonica LDFLAGS="-L$(PWD)/lib" --prefix=$(PWD) $(TESSERACT_FLAGS) 

lib/libtesseract.a:
	@$(MAKE) $(TESS_DIR)/Makefile
	@$(MAKE) -C $(TESS_DIR) V=$(MAKE_VERBOSE) CXXFLAGS="$(CFLAGS)" install

clean:
	rm -rf bin/ include/ lib/
	rm -rf $(LEPT_DIR)/Makefile
	rm -rf $(TESS_DIR)/Makefile
