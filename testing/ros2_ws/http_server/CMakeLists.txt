cmake_minimum_required(VERSION 3.8)
project(http_server)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDDARD 23)
set(CMAKE_C_STANDDARD_REQUIRED ON)

add_executable( http_server 
                main.cpp
                mongoose.c )

target_include_directories(http_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# function(copy_file FILE)
#     # add_custom_command(
#     #     TARGET http_tcp PRE_BUILD
#     #     COMMENT "Copping ${file} to exec destination"
#     #     COMMAND ${CMAKE_COMMAND} -E copy 
#     #         "${CMAKE_CURRENT_SOURCE_DIR}/${file}"
#     #         "${EXECUTABLE_OUTPUT_PATH}/"
#     # )
#     configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${FILE}" "${EXECUTABLE_OUTPUT_PATH}/" COPYONLY)
# endfunction()
#
# copy_file(index.html)
# copy_file(prog.js)
# copy_file(style.css)

ament_target_dependencies(http_server rclcpp std_msgs)

install(TARGETS
  http_server
  DESTINATION lib/${PROJECT_NAME})

ament_package()
