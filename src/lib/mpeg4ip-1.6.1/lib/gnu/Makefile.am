lib_LTLIBRARIES = libmpeg4ip_gnu.la

INCLUDES = -I$(top_srcdir)/include

libmpeg4ip_gnu_la_SOURCES = \
	getopt.c \
	getopt1.c \
	strsep.c \
	strcasestr.c \
	strcasestr.h

EXTRA_DIST = libmissing60.dsp libmissing.vcproj \
	need_for_win32.c 

AM_CFLAGS = -D_REENTRANT -Werror -Wall -Wcast-align -Wstrict-prototypes -Wmissing-prototypes

