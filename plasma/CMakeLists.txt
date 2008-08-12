project(plasma-rsibreak)

set(rsibreak_applet_SRCS
    rsibreak.cpp)

set(rsibreak_engine_SRCS
    engine.cpp)

kde4_add_plugin(plasma_applet_rsibreak ${rsibreak_applet_SRCS})
target_link_libraries(plasma_applet_rsibreak ${PLASMA_LIBS} ${KDE4_KIO_LIBS})

kde4_add_plugin(plasma_engine_rsibreak ${rsibreak_engine_SRCS})
target_link_libraries(plasma_engine_rsibreak ${PLASMA_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS plasma_applet_rsibreak DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-rsibreak.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES rsibreak.svg DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/widgets/)

install(TARGETS plasma_engine_rsibreak DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-engine-rsibreak.desktop DESTINATION ${SERVICES_INSTALL_DIR})
