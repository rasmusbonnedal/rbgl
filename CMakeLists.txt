cmake_minimum_required(VERSION 2.8)

find_package(OpenGL REQUIRED)
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_SEARCH_MODULE(GLFW REQUIRED glfw3)
LINK_DIRECTORIES(${GLFW_LIBRARY_DIRS})

#add_executable(test_tigr
#    test_tigr.cpp
#    tigr/tigr.c
#    tigr/tigr.h
#)
#target_include_directories(test_tigr PUBLIC tigr)

#add_executable(test_glfw
#    test_glfw.c
#)
#target_include_directories(test_glfw PUBLIC ${GLFW_INCLUDE_DIRS})
#target_link_libraries(test_glfw ${GLFW_LIBRARIES})

if(APPLE)
    set(CMAKE_C_FLAGS "-pedantic-errors -Wall")
    set(CMAKE_CXX_FLAGS "-pedantic-errors -Wall")
endif()

add_executable(test_main_c
    test_main.c
    test_main_impl.c
    rb_gl_window.h
)
target_compile_options(test_main_c PUBLIC -std=c89)

add_executable(test_menu
    test_menu.c
    rb_gl_window.h
)

#add_executable(test_menu_objc
#    test_menu.m
#)

add_executable(test_main_cpp
    test_main.cpp
    test_main_impl.cpp
    rb_gl_window.h
)

if(APPLE)
   find_library(COCOA_LIBRARY Cocoa)
   find_library(APPKIT_LIBRARY AppKit)
   target_link_libraries(test_main_c ${APPKIT_LIBRARY})
   target_link_libraries(test_main_cpp ${APPKIT_LIBRARY})
#   target_link_libraries(test_tigr ${COCOA_LIBRARY})
#   target_link_libraries(test_glfw ${COCOA_LIBRARY})
   target_link_libraries(test_menu ${COCOA_LIBRARY})
#   target_link_libraries(test_menu_objc ${COCOA_LIBRARY})
endif()
target_link_libraries(test_main_c ${OPENGL_LIBRARY})
target_link_libraries(test_main_cpp ${OPENGL_LIBRARY})
#target_link_libraries(test_tigr ${OPENGL_LIBRARY})
#target_link_libraries(test_glfw ${OPENGL_LIBRARY})
