# STL Algorithms Practice (No Raw Loops)

add_executable(sorting_problems sorting_problems.cpp)
set_target_properties(sorting_problems PROPERTIES CXX_STANDARD 23)

add_executable(searching_problems searching_problems.cpp)
set_target_properties(searching_problems PROPERTIES CXX_STANDARD 23)

add_executable(transform_copy_problems transform_copy_problems.cpp)
set_target_properties(transform_copy_problems PROPERTIES CXX_STANDARD 23)

add_executable(numeric_algorithms numeric_algorithms.cpp)
set_target_properties(numeric_algorithms PROPERTIES CXX_STANDARD 23)
