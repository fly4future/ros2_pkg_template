cmake_minimum_required(VERSION 3.28)
project(f4f_ros2_template)

# --- Export & Symlink compile_commands.json ---
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SOURCE_JSON "${CMAKE_SOURCE_DIR}/compile_commands.json")
set(BINARY_JSON "${CMAKE_BINARY_DIR}/compile_commands.json")

if(EXISTS ${BINARY_JSON} AND NOT EXISTS ${SOURCE_JSON})
    message(STATUS "Symlinking compile_commands.json to source directory")
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${BINARY_JSON} ${SOURCE_JSON})
endif()

# set the correct standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1 == use rclcpp::Timer implementation
# 0 == use MRS's thread timer implementation
set(USE_ROS_TIMER 0)

if(${USE_ROS_TIMER})
  MESSAGE(WARNING "[f4f_ros2_template]: Compiling with ROS Timers. This can cause high CPU load in runtime.")
endif()

# set the compile options to show code warnings
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options("-Wall" "-Wextra" "-Wpedantic")
endif()

if(ENABLE_COVERAGE)
  message(WARNING "building for coverage, the performance might be limited")

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs")

  add_definitions("-O1")
  add_definitions("-g")
endif()

# Find all dependencies of this package
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(mrs_lib REQUIRED)
find_package(std_srvs REQUIRED)

## --------------------------------------------------------------
## |                           compile                          |
## --------------------------------------------------------------

# Define the shared library created by this package
add_library(F4FROS2Template_ROS2TemplateComponent SHARED)

# Add sources to the shared library
# - Source files should be listed in private section.
# - Header files should be added to a file set.
#   - If you are using header files, see other examples on how to properly
#     handle them:
#     - public headers: `example_plugin_manager`
#     - private headers: `example_waypoint_flier_native`
target_sources(F4FROS2Template_ROS2TemplateComponent
  PRIVATE
    "src/ros2_template.cpp"
)

# Link to the dependencies used by the library
# This call automatically sets up include paths as well as linking.
# Prefer using the `<package>::<lib>` syntax as it provides better error
# handling in CMake. Most packages export targets in the format
# `<package>::<package>` that include the whole library.
# Message libraries unfortunately do not provide this target and users must link
# against: `${<package>_TARGETS}`.
target_link_libraries(F4FROS2Template_ROS2TemplateComponent
  PRIVATE
    rclcpp::rclcpp
    rclcpp_components::component
    mrs_lib::mrs_lib
    ${mrs_msgs_TARGETS}
    ${std_srvs_TARGETS}
)

# Register components defined in this library.
# rclcpp_components_register_nodes(<library> <names of components>...)
rclcpp_components_register_nodes(F4FROS2Template_ROS2TemplateComponent
  "f4f_ros2_template::ROS2Template"
)

## --------------------------------------------------------------
## |                           testing                          |
## --------------------------------------------------------------

if(ENABLE_TESTS)

  message(WARNING "TODO")
  find_package(ament_cmake_clang_format REQUIRED)
  ament_clang_format(CONFIG_FILE "${PROJECT_SOURCE_DIR}/.clang-formatt")
  find_package(ament_cmake_xmllint REQUIRED)
  ament_xmllint()

endif()

## --------------------------------------------------------------
## |                           install                          |
## --------------------------------------------------------------

install(
  TARGETS F4FROS2Template_ROS2TemplateComponent
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY launch
  DESTINATION "share/${PROJECT_NAME}"
)

install(
  DIRECTORY config
  DESTINATION "share/${PROJECT_NAME}"
)

install(
  DIRECTORY tmux
  USE_SOURCE_PERMISSIONS
  DESTINATION "share/${PROJECT_NAME}"
)

ament_export_targets(
  export_${PROJECT_NAME} HAS_LIBRARY_TARGET
)

ament_package()
