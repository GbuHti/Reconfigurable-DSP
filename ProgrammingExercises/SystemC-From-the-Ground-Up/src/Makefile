SUBDIRS = $(filter-out ./,$(dir $(shell find . -name Makefile)))

.PHONY: all clean check run show

all:
	@for i in $(SUBDIRS); do (cd $$i && $(MAKE)); done

check:
	@for i in $(SUBDIRS); do (cd $$i && $(MAKE) check); done

run:
	@for i in $(SUBDIRS); do (cd $$i && $(MAKE) run); done

clean:
	@for i in $(SUBDIRS); do (cd $$i && $(MAKE) clean); done


show:
	@for i in $(SUBDIRS); do echo "$$i"; done

#EOF
