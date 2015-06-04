all: 
	@$(MAKE) -s $(BUILD_LIBS) $(BUILD_PROJECTS) $(BUILD_DIRS)

.PHONY: $(BUILD_LIBS) $(BUILD_PROJECTS) $(BUILD_DIRS)

clean: $(BUILD_PROJECTS:%=%__clean) $(BUILD_LIBS:%=%__clean) $(BUILD_DIRS:%=%__clean)

$(BUILD_PROJECTS): $(BUILD_LIBS) $(BUILD_DIRS)

$(BUILD_DIRS:%=%):
	@echo "(SUBDIR) [$@]"
	@$(MAKE) -s -C $@

$(BUILD_DIRS:%=%__clean):
	@echo "(SUBDIR) [$(@:%__clean=%)]"
	@$(MAKE) -s -C $(@:%__clean=%) clean

$(BUILD_PROJECTS) $(BUILD_LIBS):
	@echo "(PROJECT) [$(@)]"
	@$(MAKE) -s -C $(@)/src

$(BUILD_PROJECTS:%=%__clean) $(BUILD_LIBS:%=%__clean):
	@echo "(SUBDIR) [$(@:%__clean=%)]"
	@$(MAKE) -s -C $(@:%__clean=%)/src clean

%_package:
	@echo "(PACKAGE) [$(@:%_package=%)]"
	@$(MAKE) -s -C $(@:%_package=%)/src
	./$(@:%_package=%)/scripts/package.sh
