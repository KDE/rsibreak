INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/src ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )

############# Find LIBXSS for idle detection and write outcome to config.h ####

include (CheckLibraryExists)
check_library_exists(Xss XScreenSaverQueryInfo "" HAVE_LIBXSS)

# Not having libxss is fatal at the moment.
IF (NOT HAVE_LIBXSS)
     MESSAGE(FATAL_ERROR "Missing libxss library")
ELSE (NOT HAVE_LIBXSS)
    SET (IDLE_DETECTION_LIB "Xss")
ENDIF (NOT HAVE_LIBXSS)

# Write it down to the config.h
CONFIGURE_FILE( ../config.h.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/src/config.h    )

############ librsibreak #####################################################

KDE4_ADD_LIBRARY(librsibreak STATIC rsitimer_dpms.cpp)

############ rsibreak ########################################################

# source files needed
SET(rsibreak_sources
slideshow.cpp
graywidget.cpp
passivepopup.cpp
rsidock.cpp
setup.cpp
setupgeneral.cpp
setuptiming.cpp
main.cpp
rsiwidget.cpp
rsitimer.cpp
rsirelaxpopup.cpp
setupmaximized.cpp
rsitooltip.cpp
rsistatwidget.cpp
rsistats.cpp
rsiglobals.cpp
rsistatitem.cpp
)

QT4_ADD_DBUS_ADAPTOR( rsibreak_sources
org.rsibreak.rsiwidget.xml
rsiwidget.h RSIWidget::RSIWidget
)

# compilation
KDE4_ADD_EXECUTABLE(rsibreak ${rsibreak_sources})

########### linking #####################

TARGET_LINK_LIBRARIES(rsibreak ${KDE4_KIO_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} librsibreak ${IDLE_DETECTION_LIB} )

########### install files ###############

INSTALL(TARGETS rsibreak DESTINATION ${BIN_INSTALL_DIR})
INSTALL(FILES  rsibreak.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
INSTALL( FILES rsibreak.notifyrc DESTINATION ${DATA_INSTALL_DIR}/rsibreak  )
INSTALL( FILES org.rsibreak.rsiwidget.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR} )

#TODO is this still in KDE4? 
#INSTALL_FILES( /share/autostart FILES  rsibreak.desktop )

