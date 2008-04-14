project( rsibreak )
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package (KDE4 REQUIRED)
include(KDE4Defaults)
macro_optional_find_package(Plasma)


add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions (-mmmx)
include_directories ( ${KDE4_INCLUDES} ${BLITZ_INCLUDES} )
ADD_SUBDIRECTORY( icons ) 
ADD_SUBDIRECTORY( src ) 
#Laurent: needs to adapt to new plasma api
if (PLASMA_FOUND)
  #ADD_SUBDIRECTORY( plasma ) 
endif (PLASMA_FOUND)
