SUBDIR = $(filter-out ./,$(dir $(shell find . -name Makefile) ) )
$(warning $(SUBDIR))

all:
	for i in $(SUBDIR); do ( echo $$i); done
