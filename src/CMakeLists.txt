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

QT_ADD_DBUS_ADAPTOR( rsibreak_sources
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
    KF5::I18n
    KF5::IdleTime
    KF5::Notifications
    KF5::NotifyConfig
    KF5::TextWidgets
    KF5::XmlGui
    KF5::WindowSystem
    Qt::DBus
)
target_link_libraries(rsibreak rsibreak_lib)

# install
install( TARGETS rsibreak ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install( PROGRAMS org.kde.rsibreak.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
install( FILES rsibreak.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR}  )
install( FILES org.rsibreak.rsiwidget.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR} )
install( FILES rsibreak_autostart.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR} )
install( FILES org.kde.rsibreak.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )
