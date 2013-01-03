## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/gsandia/glade
glade_DATA = 

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GSANDIA_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gsandia

gsandia_SOURCES = \
	callbacks.c \
	callbacks.h \
	main.c \
	sandia-main-window.h \
	sandia-main-window.c \
	sandia-host-browser.h \
	sandia-host-browser.c \
	common.h

gsandia_LDFLAGS = \
	-Wl,--export-dynamic

gsandia_LDADD = $(GSANDIA_LIBS)

EXTRA_DIST = $(glade_DATA)

# Remove glade directory on uninstall
uninstall-local:
	-rm -r $(gladedir)
	-rm -r $(datadir)/gsandia
