cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(src CXX)

file(GLOB CC_FILES *.cc)

find_package(Threads REQUIRED)

include_directories(${BOOST_INCLUDE_DIR})
add_library(boost_chrono SHARED IMPORTED)
set_target_properties(boost_chrono PROPERTIES IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_chrono-mt.so)
add_library(boost_system SHARED IMPORTED)
set_target_properties(boost_system PROPERTIES IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_system-mt.so)
add_library(boost_thread SHARED IMPORTED)
set_target_properties(boost_thread PROPERTIES IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_thread-mt.so)

add_executable(asio_http ${CC_FILES})
set_property(TARGET asio_http PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})

target_link_libraries(asio_http boost_chrono boost_filesystem boost_system boost_thread ${CMAKE_THREAD_LIBS_INIT} -ldl)
