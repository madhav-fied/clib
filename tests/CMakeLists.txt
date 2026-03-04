find_package(PkgConfig REQUIRED)
pkg_check_modules(CHECK REQUIRED IMPORTED_TARGET check)

add_executable(test_vector test_vector.c)
target_link_libraries(test_vector PRIVATE clib PkgConfig::CHECK)
add_test(NAME test_vector COMMAND test_vector)

add_executable(test_stack test_stack.c)
target_link_libraries(test_stack PRIVATE clib PkgConfig::CHECK)
add_test(NAME test_stack COMMAND test_stack)

add_executable(test_queue test_queue.c)
target_link_libraries(test_queue PRIVATE clib PkgConfig::CHECK)
add_test(NAME test_queue COMMAND test_queue)
