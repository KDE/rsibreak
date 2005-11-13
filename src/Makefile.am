# set the include path for X, qt and KDE
INCLUDES =  -I$(top_srcdir) $(all_includes)

# these are the headers for your project
noinst_HEADERS = rsidock.h rsiwidget.h setup.h setupgeneral.h \
	setuptiming.h rsitimer.h rsipopup.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/rsibreak.pot

KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = rsibreak

# the application source, library search path, and link libraries
rsibreak_SOURCES = rsidock.cpp setup.cpp setupgeneral.cpp setuptiming.cpp \
	main.cpp rsiwidget.cpp rsitimer.cpp rsipopup.cpp
rsibreak_LDFLAGS = $(KDE_RPATH) $(all_libraries)
rsibreak_LDADD = $(LIB_KDEUI) $(LIB_KIO) $(LIBXSS)

# this is where the desktop file will go
xdg_apps_DATA = rsibreak.desktop
autostart_DATA = rsibreak.desktop
autostartdir = $(datadir)/autostart

include ../../admin/Doxyfile.am
