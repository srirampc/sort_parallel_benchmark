cmake_minimum_required(VERSION 3.1)

project(srtbench CXX)

option (BRIEF "Use brief benchmark." OFF) 

# add TBB
set(TBB_ROOT "${PROJECT_SOURCE_DIR}/ext/tbb/")
include(${TBB_ROOT}/cmake/TBBBuild.cmake)
tbb_build(TBB_ROOT ${TBB_ROOT} CONFIG_DIR TBB_DIR MAKE_ARGS)
find_package(TBB REQUIRED tbb)
find_package (Threads)

# Settings for GCC.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# TBB
	#append_cxx_compiler_flags("-fopenmp" "GCC" CMAKE_CXX_FLAGS)
	set(BRIEF_SRC_DIR "Linux64_GCC/src/brief/")
	set(DETAILED_SRC_DIR "Linux64_GCC/src/detailed/")
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -march=native -w -fexceptions -fopenmp -O3 ")
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(BRIEF_SRC_DIR "Linux64_Clang/src/brief/")
	set(DETAILED_SRC_DIR "Linux64_Clang/src/detailed/")
else()
	message(SEND_ERROR "Using unknown cxx compiler.")
endif()

include_directories("${PROJECT_SOURCE_DIR}/include")

if (BRIEF)
	add_executable(benchmark ${BRIEF_SRC_DIR}/benchmark.cpp)
	target_link_libraries(benchmark  ${TBB_IMPORTED_TARGETS})
	target_link_libraries(benchmark  omp)
	target_link_libraries(benchmark  ${CMAKE_THREAD_LIBS_INIT})
	add_executable(file_generator ${BRIEF_SRC_DIR}/file_generator.cpp)
else()
	add_executable(parallel_numbers ${DETAILED_SRC_DIR}/parallel_numbers.cpp)
	target_link_libraries(parallel_numbers  ${TBB_IMPORTED_TARGETS})
	target_link_libraries(parallel_numbers  ${CMAKE_THREAD_LIBS_INIT})
	add_executable(parallel_strings ${DETAILED_SRC_DIR}/parallel_strings.cpp)
	target_link_libraries(parallel_strings  ${TBB_IMPORTED_TARGETS})
	target_link_libraries(parallel_strings  ${CMAKE_THREAD_LIBS_INIT})
	add_executable(parallel_objects ${DETAILED_SRC_DIR}/parallel_objects.cpp)
	target_link_libraries(parallel_objects  ${TBB_IMPORTED_TARGETS})
	target_link_libraries(parallel_objects  ${CMAKE_THREAD_LIBS_INIT})
	add_executable(file_generator ${DETAILED_SRC_DIR}/file_generator.cpp)
endif()
