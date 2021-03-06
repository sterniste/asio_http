cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(src CXX)

file(GLOB CC_FILES *.cc)

add_compile_options(-std=c++14 -g -Wall)

find_package(Threads REQUIRED)

include_directories(${BOOST_INCLUDE_DIR})
add_library(boost_chrono STATIC IMPORTED)
set_target_properties(boost_chrono PROPERTIES IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_chrono-mt.a)
add_library(boost_system STATIC IMPORTED)
set_target_properties(boost_system PROPERTIES IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_system-mt.a)
add_library(boost_thread STATIC IMPORTED)
set_target_properties(boost_thread PROPERTIES IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_thread-mt.a)

add_executable(asio_http ${CC_FILES})
if (BUILD_IWYU)
 set_property(TARGET asio_http PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()

target_link_libraries(asio_http boost_chrono boost_system boost_thread ${CMAKE_THREAD_LIBS_INIT} ws2_32 wsock32)
