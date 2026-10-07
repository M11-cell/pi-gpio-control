cmake_minimum_required(VERSION 3.20)
project(led_toggle)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(service_interface REQUIRED)

find_library(WIRINGPI_LIB wiringPi REQUIRED)

add_executable(toggle_client src/led_toggle_client.cpp)

target_include_directories(toggle_client
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(toggle_client 
  PUBLIC
    rclcpp::rclcpp 
    service_interface::service_interface
)

add_executable(toggle_service src/led_toggle_service.cpp)

target_include_directories(toggle_service
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


target_link_libraries(toggle_service 
  PUBLIC
    rclcpp::rclcpp 
    service_interface::service_interface
    ${WIRINGPI_LIB}
)

install(TARGETS
  toggle_client
  toggle_service

  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
