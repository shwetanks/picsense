ifndef OPTFLAGS
OPTFLAGS=-O3
endif

ifndef DEPTH
DEPTH=..
endif

ifndef OBJ_SUB_DIR
OBJ_DIR=../obj
else
OBJ_DIR=../obj/$(OBJ_SUB_DIR)
endif


OCULUS_INCLUDES=-I$(DEPTH)/../oculus/include
OCULUS_LIB=-L$(DEPTH)/../oculus/lib $(OCLIBS:%=-l%)

LDFLAGS+= -pthread -lrt $(OCULUS_LIB) -L../lib $(LIBS)
CFLAGS+= $(OPTFLAGS) $(INCLUDES:%=-I%) -gdwarf-2 -W -Wall -Werror $(OCULUS_INCLUDES)
CXXFLAGS+= $(OPTFLAGS) $(INCLUDES:%=-I%) -gdwarf-2 -W -Wall -Werror $(OCULUS_INCLUDES)

OBJ=$(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEP=$(SRC:%.cpp=$(OBJ_DIR)/%.d)

DIRS=$(OBJ_DIR)/.dir ../bin/.dir ../lib/.dir

ifeq ($(TARGET:%.a=%).a,$(TARGET))
LIB_OBJ:=$(OBJ)
TARGET_FILE=../lib/lib$(TARGET)
else ifeq ($(TARGET:%.so=%).so,$(TARGET))
LIB_OBJ:=$(OBJ)
TARGET_FILE=../lib/lib$(TARGET)
else
TARGET_FILE=../bin/$(TARGET)
endif

all: $(TARGET_FILE)

$(DIRS):
	@echo "  (MKDIR) -p $(@:%/.dir=%)"
	@-mkdir -p $(@:%/.dir=%)
	@touch $@

$(OBJ_DIR)/%.o: %.cpp $(OBJ_DIR)/%.d
	@echo "  (C++)  $*.cpp  [ -c $(CXXFLAGS) $*.cpp -o $(OBJ_DIR)/$*.o ]"
	@$(CXX) -c $(CXXFLAGS) $*.cpp -o $(OBJ_DIR)/$*.o

$(OBJ_DIR)/%.d: %.cpp
	@echo "  (DEP)  $*.cpp"
	@$(CXX) -MM $(INCLUDES:%=-I%) $(OCULUS_INCLUDES) $*.cpp | sed -e 's|.*:|$(OBJ_DIR)/$*.o:|' > $@

$(OBJ): $(DIRS)

$(DEP): $(DIRS)

../lib/%.so: $(LIB_OBJ)
	@echo "  (SO)  $(@:../lib/%=%) [ -shared $(CXXFLAGS) -o $@ $^ ]"
	@$(CXX) -shared $(CXXFLAGS) -o $@ $^

../lib/%: $(LIB_OBJ)
	@echo "  (AR)   $(@:../lib/%=%)  [ rcs $@ $^ ]"
	@ar rcs $@ $^

../bin/%: $(OBJ) $(OCLIBS:%=$(DEPTH)/../oculus/lib/lib%.a) $(EXTRA_DEPS)
	@echo "  (LD)   $(@:../bin/%=%)  [ $(OBJ) -o $@ -o $@ $(LDFLAGS) ]"
	@$(CXX) $(OBJ) -o $@ $(LDFLAGS)

$(DIRS:%=RM_%):
	@echo "  (RM)   $(@:RM_%/.dir=%)/*"
	@-rm -rf $(@:RM_%/.dir=%)/*

clean: $(DIRS:%=RM_%)

etags:
	@echo "  (ETAGS)"
	@find . -name "*.[ch]" -o -name "*.cpp" -o -name "*.inl" | cut -c 3- | xargs etags --lang=c++ -I

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP)
endif
