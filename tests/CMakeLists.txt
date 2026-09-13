# GoogleTest Test Suite

add_executable(test_move_semantics test_move_semantics.cpp)
target_link_libraries(test_move_semantics GTest::gtest_main)
set_target_properties(test_move_semantics PROPERTIES CXX_STANDARD 23)

add_executable(test_templates test_templates.cpp)
target_link_libraries(test_templates GTest::gtest_main)
set_target_properties(test_templates PROPERTIES CXX_STANDARD 23)

add_executable(test_algorithms test_algorithms.cpp)
target_link_libraries(test_algorithms GTest::gtest_main)
set_target_properties(test_algorithms PROPERTIES CXX_STANDARD 23)

add_executable(test_concepts test_concepts.cpp)
target_link_libraries(test_concepts GTest::gtest_main)
set_target_properties(test_concepts PROPERTIES CXX_STANDARD 23)

# Register tests with CTest
gtest_discover_tests(test_move_semantics)
gtest_discover_tests(test_templates)
gtest_discover_tests(test_algorithms)
gtest_discover_tests(test_concepts)
