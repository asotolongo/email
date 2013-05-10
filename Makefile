# contrib/email/Makefile

MODULE_big = email
OBJS = email.o 

EXTENSION = email
DATA = email--0.1.sql

REGRESS = email

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/email
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
