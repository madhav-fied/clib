# Test executables
add_executable(test_vector test_vector.c)
target_link_libraries(test_vector PRIVATE clib)

add_test(NAME test_vector COMMAND test_vector)
