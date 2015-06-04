ifdef PROJECTS
all:
	@$(MAKE) -s $(PROJECTS)

clean:
	@$(MAKE) -s $(PROJECTS:%=%__clean)

.PHONY: $(PROJECTS)

$(PROJECTS):
	@echo "(PROJECT) [$(@)]"
	@$(MAKE) -s -C $(@)/src

$(PROJECTS:%=%__clean):
	@echo "(PROJECT) [$(@:%__clean=%)]"
	@$(MAKE) -s -C $(@:%__clean=%)/src clean

else

ifndef SOURCES
SOURCES=src
endif
project_ project_clean:
	@echo "(MAKE) -C $(SOURCES) -s $(@:project_%=%)"
	@$(MAKE) -C $(SOURCES) -s $(@:project_%=%)

clean: project_clean

endif
