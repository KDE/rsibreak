include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} )

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
setupnotifications.cpp
main.cpp
rsiwidget.cpp
rsirelaxpopup.cpp
setupmaximized.cpp
rsitooltip.cpp
rsistatwidget.cpp
rsistats.cpp
rsitimer.cpp
rsiglobals.cpp
rsistatitem.cpp
breakbase.cpp
plasmaeffect.cpp
breakcontrol.cpp
)

QT4_ADD_DBUS_ADAPTOR( rsibreak_sources
org.rsibreak.rsiwidget.xml
rsiwidget.h RSIObject
)

# compilation
kde4_add_executable(rsibreak ${rsibreak_sources} ${rsibreak_sources_timer})

# linking
target_link_libraries(rsibreak ${KDE4_KIO_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} )
target_link_libraries(rsibreak ${KDE4_KUTILS_LIBRARY} ${X11_X11_LIB})

# install
install( TARGETS rsibreak ${INSTALL_TARGETS_DEFAULT_ARGS})
install( PROGRAMS  rsibreak.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES rsibreak.notifyrc DESTINATION ${DATA_INSTALL_DIR}/rsibreak  )
install( FILES org.rsibreak.rsiwidget.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR} )
install( FILES rsibreak.desktop DESTINATION ${AUTOSTART_INSTALL_DIR} )
