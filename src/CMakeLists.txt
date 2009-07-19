include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} )

macro_ensure_version( "4.3.60" ${KDE_VERSION} USE_KIDLE)
if(NOT USE_KIDLE)

  ############# Find LIBXSS for idle detection and write outcome to config.h ####

  include (CheckLibraryExists)
  check_library_exists(Xss XScreenSaverQueryInfo "" HAVE_LIBXSS)

  # Not having libxss is fatal at the moment.
  if (NOT HAVE_LIBXSS)
     message(FATAL_ERROR "Missing libxss library")
  else (NOT HAVE_LIBXSS)
    set (IDLE_DETECTION_LIB "Xss")
  endif (NOT HAVE_LIBXSS)

endif(NOT USE_KIDLE)

# Write it down to the config-rsibreak.h
configure_file(../config-rsibreak.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-rsibreak.h)

############ rsibreak ########################################################

# source files needed
set(rsibreak_sources
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
  set(rsibreak_sources_timer rsitimer.cpp)
else(NOT USE_KIDLE)
  set(rsibreak_sources_timer rsitimer_kidle.cpp)
endif(NOT USE_KIDLE)

QT4_ADD_DBUS_ADAPTOR( rsibreak_sources
org.rsibreak.rsiwidget.xml
rsiwidget.h RSIObject::RSIObject
)

# compilation
kde4_add_executable(rsibreak ${rsibreak_sources} ${rsibreak_sources_timer})


# linking
target_link_libraries(rsibreak ${KDE4_KIO_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} )
if(NOT USE_KIDLE)
  kde4_add_library(librsibreak STATIC rsitimer_dpms.cpp)
  target_link_libraries(rsibreak librsibreak ${IDLE_DETECTION_LIB} )
else(NOT USE_KIDLE)
  target_link_libraries(rsibreak kidle )
endif(NOT USE_KIDLE)

# install
install( TARGETS rsibreak ${INSTALL_TARGETS_DEFAULT_ARGS})
install( FILES  rsibreak.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES rsibreak.notifyrc DESTINATION ${DATA_INSTALL_DIR}/rsibreak  )
install( FILES org.rsibreak.rsiwidget.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR} )
install( FILES rsibreak.desktop DESTINATION ${AUTOSTART_INSTALL_DIR} )
