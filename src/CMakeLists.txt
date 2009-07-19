INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/src ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} )

MACRO_ENSURE_VERSION( "4.3.60" ${KDE_VERSION} USE_KIDLE)
if(NOT USE_KIDLE)

  ############# Find LIBXSS for idle detection and write outcome to config.h ####

  include (CheckLibraryExists)
  check_library_exists(Xss XScreenSaverQueryInfo "" HAVE_LIBXSS)

  # Not having libxss is fatal at the moment.
  IF (NOT HAVE_LIBXSS)
     MESSAGE(FATAL_ERROR "Missing libxss library")
  ELSE (NOT HAVE_LIBXSS)
    SET (IDLE_DETECTION_LIB "Xss")
  ENDIF (NOT HAVE_LIBXSS)

  # Write it down to the config-rsibreak.h
  CONFIGURE_FILE( ../config-rsibreak.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/config-rsibreak.h    )
endif(NOT USE_KIDLE)


############ rsibreak ########################################################

# source files needed
SET(rsibreak_sources
slideshoweffect.cpp
popupeffect.cpp
grayeffect.cpp
passivepopup.cpp
rsidock.cpp
setup.cpp
setupgeneral.cpp
setuptiming.cpp
main.cpp
rsiwidget.cpp
rsirelaxpopup.cpp
setupmaximized.cpp
rsitooltip.cpp
rsistatwidget.cpp
rsistats.cpp
rsiglobals.cpp
rsistatitem.cpp
breakbase.cpp
plasmaeffect.cpp
breakcontrol.cpp
)

if(NOT USE_KIDLE)
  SET(rsibreak_sources_timer rsitimer.cpp)
else(NOT USE_KIDLE)
  SET(rsibreak_sources_timer rsitimer_kidle.cpp)
endif(NOT USE_KIDLE)

QT4_ADD_DBUS_ADAPTOR( rsibreak_sources
org.rsibreak.rsiwidget.xml
rsiwidget.h RSIObject::RSIObject
)

# compilation
KDE4_ADD_EXECUTABLE(rsibreak ${rsibreak_sources} ${rsibreak_sources_timer})


# linking
TARGET_LINK_LIBRARIES(rsibreak ${KDE4_KIO_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} )
if(NOT USE_KIDLE)
  KDE4_ADD_LIBRARY(librsibreak STATIC rsitimer_dpms.cpp)
  TARGET_LINK_LIBRARIES(rsibreak librsibreak ${IDLE_DETECTION_LIB} )
else(NOT USE_KIDLE)
  TARGET_LINK_LIBRARIES(rsibreak kidle )
endif(NOT USE_KIDLE)

# install
INSTALL( TARGETS rsibreak ${INSTALL_TARGETS_DEFAULT_ARGS})
INSTALL( FILES  rsibreak.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
INSTALL( FILES rsibreak.notifyrc DESTINATION ${DATA_INSTALL_DIR}/rsibreak  )
INSTALL( FILES org.rsibreak.rsiwidget.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR} )
INSTALL( FILES rsibreak.desktop DESTINATION ${AUTOSTART_INSTALL_DIR} )
