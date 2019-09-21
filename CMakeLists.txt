project( rsibreak )

cmake_minimum_required (VERSION 3.5.0 FATAL_ERROR)
set (QT_MIN_VERSION "5.6.0")

find_package(ECM 5.18.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS DBus)
find_package(KF5 REQUIRED COMPONENTS 
    Config
    ConfigWidgets
    Crash
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
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)

add_subdirectory( icons )
add_subdirectory( doc )
add_subdirectory( src )

include(CTest)

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory( test )
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
