project(plasma-rsibreak)

set(rsibreak_SRCS
    rsibreak.cpp)

kde4_add_plugin(plasma_applet_rsibreak ${rsibreak_SRCS})
target_link_libraries(plasma_applet_rsibreak plasma ${KDE4_KIO_LIBS})
set_target_properties(plasma_applet_rsibreak PROPERTIES VERSION 1.0.0 SOVERSION 1)

install(TARGETS plasma_applet_rsibreak DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-rsibreak.desktop DESTINATION ${SERVICES_INSTALL_DIR})
