version.m4: xcc.h
	grep XCC_VERSION_ $? | awk '{printf("m4_define([%s], %d)\n", $$2, $$3)}' > $@
