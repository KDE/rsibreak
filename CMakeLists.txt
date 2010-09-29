project( rsibreak )
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package (KDE4 REQUIRED)
include(KDE4Defaults)

MACRO_ENSURE_VERSION( "4.4.0" ${KDE_VERSION} KDE_VERSION_OK)
if(NOT KDE_VERSION_OK)
  macro_optional_find_package(Plasma)
  macro_log_feature(PLASMA_FOUND "Plasma" "Plasma libraries and header files" "http://plasma.kde.org/" FALSE "" "Plasma is needed for rsibreak Plasmoid")
else(NOT KDE_VERSION_OK)
  set(PLASMA_FOUND true)
  set(PLASMA_LIBS ${KDE4_PLASMA_LIBS})
endif(NOT KDE_VERSION_OK)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories ( ${KDE4_INCLUDES} )

add_subdirectory( icons )
add_subdirectory( src )
if (PLASMA_FOUND)
  ADD_SUBDIRECTORY( plasma )
endif (PLASMA_FOUND)
