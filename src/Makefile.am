# set the include path for X, qt and KDE
INCLUDES =  -I$(top_srcdir) $(all_includes)

# these are the headers for your project
noinst_HEADERS = rsidock.h rsiwidget.h setup.h setupgeneral.h \
	setuptiming.h setupmaximized.h rsitimer.h rsirelaxpopup.h \
	rsitooltip.h rsistatwidget.h rsistats.h rsiglobals.h rsistatitem.h \
	rsitimer_dpms.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/rsibreak.pot

noinst_LTLIBRARIES = librsibreak.la
librsibreak_la_SOURCES = rsitimer_dpms.cpp

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = rsibreak

# the application source, library search path, and link libraries
rsibreak_SOURCES = rsidock.cpp setup.cpp setupgeneral.cpp setuptiming.cpp \
	main.cpp rsiwidget.cpp rsitimer.cpp rsirelaxpopup.cpp \
	setupmaximized.cpp rsitooltip.cpp dcopiface.cpp dcopiface.skel \
	rsistatwidget.cpp rsistats.cpp rsiglobals.cpp rsistatitem.cpp 
rsibreak_LDFLAGS = $(KDE_RPATH) $(all_libraries)
rsibreak_LDADD = $(LIB_KDEUI) $(LIB_KIO) $(LIBXSS) \
				 librsibreak.la

# this is where the desktop file will go
xdg_apps_DATA = rsibreak.desktop
autostart_DATA = rsibreak.desktop
autostartdir = $(datadir)/autostart

rcdir = $(kde_datadir)/rsibreak
rc_DATA = eventsrc

include ../../admin/Doxyfile.am
