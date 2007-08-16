project(plasma-rsibreak)

set(rsibreak_SRCS
    rsibreak.cpp)

kde4_add_plugin(plasma_applet_rsibreak ${rsibreak_SRCS})
target_link_libraries(plasma_applet_rsibreak ${PLASMA_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS plasma_applet_rsibreak DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-rsibreak.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES rsibreak.svg DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/widgets/)
