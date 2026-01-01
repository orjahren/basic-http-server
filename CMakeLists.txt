cmake_minimum_required(VERSION 3.20)
project(AppC LANGUAGES C)

enable_testing()

add_executable(basic-http-server
  src/main.c
  src/utils.c
)

# Require C11 (use c_std_99 for C99, c_std_17 for C17)
target_compile_features(basic-http-server PRIVATE c_std_11)

# Include headers under include/
target_include_directories(basic-http-server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Example: extra warnings
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(basic-http-server PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
  target_compile_options(basic-http-server PRIVATE /W4)
endif()

find_program(CURL curl REQUIRED)

add_test(
  NAME smoke.root
  COMMAND ${CURL} --fail --silent --show-error http://127.0.0.1:8080/
)
# You still need a way to start/stop the server. One option is a small script:
# - start server in background, wait for port, curl, then kill.


add_executable(client_test
  src/test/client.c
  src/utils.c
)
target_include_directories(client_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_test(NAME client.loopback COMMAND client_test)
