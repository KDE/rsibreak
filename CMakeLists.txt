project( rsibreak )
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

find_package (KDE4 REQUIRED)

include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories ( ${KDE4_INCLUDES} )
ADD_SUBDIRECTORY( src ) 
ADD_SUBDIRECTORY( icons ) 
