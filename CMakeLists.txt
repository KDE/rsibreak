cmake_minimum_required (VERSION 3.16.0 FATAL_ERROR)

project( rsibreak )

set(QT_MIN_VERSION "6.7.0")  # Remember to update the QT_DEPRECATED_WARNINGS_SINCE below
set(KF_REQUIRED_VERSION "6.8") # Remember to update the KF_DEPRECATED_WARNINGS_SINCE below
set(KDE_COMPILERSETTINGS_LEVEL "5.82.0")

find_package(ECM ${KF_REQUIRED_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(KDEClangFormat)
include(KDEGitCommitHooks)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS DBus)
find_package(KF6 ${KF_REQUIRED_VERSION} REQUIRED COMPONENTS
    CoreAddons
    Config
    ConfigWidgets
    Crash
    DBusAddons
    DocTools
    I18n
    IdleTime
    Notifications
    NotifyConfig
    TextWidgets
    XmlGui
    WindowSystem
    StatusNotifierItem
    ColorScheme
    )

add_definitions(-DQT_DEPRECATED_WARNINGS_SINCE=0x060800)
add_definitions(-DKF_DEPRECATED_WARNINGS_SINCE=0x060900)

add_subdirectory( icons )
add_subdirectory( doc )
add_subdirectory( src )

include(CTest)

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory( test )
endif()

ki18n_install(po)
kdoctools_install(po)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)


feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
