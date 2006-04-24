ADD_SUBDIRECTORY( src ) 
ADD_SUBDIRECTORY( icons ) 

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR}  )


########### install files ###############




#original Makefile.am contents follow:

#SUBDIRS = src
#include ../admin/Doxyfile.am
