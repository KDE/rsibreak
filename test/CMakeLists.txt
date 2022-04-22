include_directories( ${PROJECT_SOURCE_DIR}/src )

set( rsibreaktest_src
    test_runner.cpp
    rsitimer_test.cpp
    rsitimercounter_test.cpp
)

find_library(rsibreak_lib rsibreak_lib)
find_package( Qt${QT_MAJOR_VERSION}Test REQUIRED )

add_executable( rsibreak_tests ${rsibreaktest_src} )

target_link_libraries( rsibreak_tests Qt::Test rsibreak_lib )

add_test(NAME rsibreak_tests COMMAND rsibreak_tests)
