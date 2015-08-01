project( rsibreak )

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.3.0")

find_package(ECM 1.7.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS DBus X11Extras)
find_package(KF5 REQUIRED COMPONENTS 
    Config
    ConfigWidgets
    DBusAddons
    DocTools
    IconThemes
    I18n
    IdleTime
    Notifications
    NotifyConfig
    TextWidgets
    XmlGui
    WindowSystem
    )

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)

add_subdirectory( icons )
add_subdirectory( doc )
add_subdirectory( src )
# TODO
# if (PLASMA_FOUND)
#   ADD_SUBDIRECTORY( plasma )
# endif (PLASMA_FOUND)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
