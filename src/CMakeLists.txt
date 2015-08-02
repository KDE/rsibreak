include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} )

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
rsistatwidget.cpp
rsistats.cpp
rsitimer.cpp
rsiglobals.cpp
rsistatitem.cpp
breakbase.cpp
plasmaeffect.cpp
breakcontrol.cpp
)

QT5_ADD_DBUS_ADAPTOR( rsibreak_sources
org.rsibreak.rsiwidget.xml
rsiwidget.h RSIObject
)

# compilation
add_executable(rsibreak ${rsibreak_sources})

# linking
target_link_libraries(rsibreak KF5::NotifyConfig KF5::WindowSystem KF5::Notifications KF5::I18n KF5::ConfigCore KF5::ConfigWidgets Qt5::DBus Qt5::X11Extras KF5::TextWidgets KF5::XmlGui KF5::IconThemes KF5::IdleTime KF5::DBusAddons X11 )

# install
install( TARGETS rsibreak ${INSTALL_TARGETS_DEFAULT_ARGS})
install( PROGRAMS  rsibreak.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES rsibreak.notifyrc DESTINATION ${DATA_INSTALL_DIR}/rsibreak  )
install( FILES org.rsibreak.rsiwidget.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR} )
install( FILES rsibreak.desktop DESTINATION ${AUTOSTART_INSTALL_DIR} )
