version.m4: xcc.h
	rm -rf autom4te.cache && grep XCC_VERSION_ $? | awk '{printf("m4_define([%s], %d)\n", $$2, $$3)}' > $@
