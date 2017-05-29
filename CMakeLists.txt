cmake_minimum_required(VERSION 2.8) 

project(graleplummerinfo)
set(VERSION 1.0.0)

include(${PROJECT_SOURCE_DIR}/cmake/Macros.cmake)
get_install_directory(LIBRARY_INSTALL_DIR)

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	# Test if the compiler already supports C++11
	check_cxx_source_compiles("#include <map>\nusing namespace std;\nint main(void) \n {\nmap<int, double> m;\n auto it = m.begin();\n return 0; \n }" C11AVAIL)
	if (NOT C11AVAIL)
		message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
	endif()
endif()

find_package(GRALE REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

add_additional_stuff(EXTRA_INCLUDES EXTRA_LIBS)
include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(plummerinfo src/main.cpp)

target_link_libraries(plummerinfo ${EXTRA_LIBS} ${GRALE_LIBRARIES})
install(TARGETS plummerinfo DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

include_directories(${GRALE_INCLUDE_DIRS})

