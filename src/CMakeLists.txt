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
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigWidgets
    KF6::Crash
    KF6::DBusAddons
    KF6::I18n
    KF6::IdleTime
    KF6::Notifications
    KF6::NotifyConfig
    KF6::TextWidgets
    KF6::XmlGui
    KF6::WindowSystem
    KF6::StatusNotifierItem
    Qt::DBus
    Qt::GuiPrivate
)
target_link_libraries(rsibreak rsibreak_lib)

# install
install( TARGETS rsibreak ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install( PROGRAMS org.kde.rsibreak.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
install( FILES rsibreak.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR}  )
install( FILES org.rsibreak.rsiwidget.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR} )
install( FILES rsibreak_autostart.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR} )
install( FILES org.kde.rsibreak.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )
