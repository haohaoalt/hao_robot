cmake_minimum_required(VERSION 2.8.3)
project(slamoptolidar_ros)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#add_definitions(-std=c++11) # Use C++11

find_package(catkin REQUIRED COMPONENTS
  rosconsole
  roscpp
  sensor_msgs
)

#add_subdirectory(sdk)

set(SDK_PATH "./sdk/")

FILE(GLOB SDK_SRC 
  "${SDK_PATH}/src/impl/unix/*.cpp"
  "${SDK_PATH}/src/*.cpp"
  "${SDK_PATH}/src/*.c"
)

catkin_package()

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/sdk/include
#   ${PROJECT_SOURCE_DIR}/sdk/src
)

add_executable(slamoptolidar_node src/slamoptolidar_node.cpp sdk/include/slidar.hpp  ${SDK_SRC})

target_link_libraries(slamoptolidar_node ${catkin_LIBRARIES} )


install(TARGETS slamoptolidar_node 
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)


install(DIRECTORY launch 
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
  USE_SOURCE_PERMISSIONS
)


