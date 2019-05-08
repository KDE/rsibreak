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
rsiwidget.cpp
rsirelaxpopup.cpp
setupmaximized.cpp
rsistatwidget.cpp
rsistats.cpp
rsitimer.cpp
rsitimercounter.cpp
rsiglobals.cpp
rsistatitem.cpp
breakbase.cpp
plasmaeffect.cpp
breakcontrol.cpp
rsiidletime.cpp
notificator.cpp
)

QT5_ADD_DBUS_ADAPTOR( rsibreak_sources
org.rsibreak.rsiwidget.xml
rsiwidget.h RSIObject
)

# compilation
add_library(rsibreak_lib STATIC ${rsibreak_sources})
add_executable(rsibreak main.cpp)

# linking
target_link_libraries(rsibreak_lib
    KF5::ConfigCore
    KF5::ConfigWidgets
    KF5::Crash
    KF5::DBusAddons
    KF5::IconThemes
    KF5::I18n
    KF5::IdleTime
    KF5::Notifications
    KF5::NotifyConfig
    KF5::TextWidgets
    KF5::XmlGui
    KF5::WindowSystem
    Qt5::DBus
)
target_link_libraries(rsibreak rsibreak_lib)

# install
install( TARGETS rsibreak ${INSTALL_TARGETS_DEFAULT_ARGS})
install( PROGRAMS org.kde.rsibreak.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES rsibreak.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR}  )
install( FILES org.rsibreak.rsiwidget.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR} )
install( FILES rsibreak_autostart.desktop DESTINATION ${AUTOSTART_INSTALL_DIR} )
install( FILES org.kde.rsibreak.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )
