FIND_PACKAGE(KDE3 REQUIRED)
ADD_DEFINITIONS(${QT_DEFINITIONS} ${KDE3_DEFINITIONS})
LINK_DIRECTORIES(${KDE3_LIB_DIR})
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} 
                     ${CMAKE_CURRENT_BINARY_DIR} 
                     ${KDE3_INCLUDE_DIR} 
                     ${QT_INCLUDE_DIR}             )

############# Find LIBXSS for idle detection and write outcome to config.h ####

# FIND_LIBRARY does not seem to find it, but FIND_FILE does. How to do that?
FIND_FILE(HAVE_LIBXSS libXss.so /usr/lib /usr/local/lib)

# Not having libxss is fatal at the moment.
IF (NOT HAVE_LIBXSS)
     MESSAGE(FATAL_ERROR "Missing libxss library")
ENDIF (NOT HAVE_LIBXSS)

# Write it down to the config.h
CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/config.h.cmake 
                ${CMAKE_BINARY_DIR}/src/config.h    ) 

############ librsibreak #####################################################

ADD_LIBRARY(librsibreak STATIC rsitimer_dpms.cpp)

############ rsibreak ######################################################## 

# source files needed
SET(rsibreak_sources
rsidock.cpp
setup.cpp
setupgeneral.cpp
setuptiming.cpp
main.cpp
rsiwidget.cpp
rsitimer.cpp
rsirelaxpopup.cpp
setuppopup.cpp
setupmaximized.cpp
rsitooltip.cpp
dcopiface.cpp
rsistatwidget.cpp
rsistats.cpp
rsiglobals.cpp
rsistatitem.cpp
)

# moc files needed
KDE3_AUTOMOC(${rsibreak_sources})

# dcop files needed
SET( rsibreak_DCOP_SKEL_SRCS dcopiface.h)
KDE3_ADD_DCOP_SKELS(rsibreak_sources ${rsibreak_DCOP_SKEL_SRCS})

# compilation
KDE3_ADD_EXECUTABLE(rsibreak ${rsibreak_sources})

########### linking #####################

TARGET_LINK_LIBRARIES(rsibreak kio kdeui librsibreak ${HAVE_LIBXSS})

########### install files ###############

INSTALL_TARGETS(/bin rsibreak )
INSTALL_FILES( /share/applications/kde FILES  rsibreak.desktop )
INSTALL_FILES( /share/autostart FILES  rsibreak.desktop )
KDE3_INSTALL_ICONS( hicolor )

