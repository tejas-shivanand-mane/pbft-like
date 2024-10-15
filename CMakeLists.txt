cmake_minimum_required(VERSION 3.10)
project(pbft_like_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include ZeroMQ
find_package(ZeroMQ REQUIRED)

# Include directories
include_directories(include)

# Create executables
add_executable(node_app src/main_node.cpp src/node.cpp)
add_executable(client_app src/main_client.cpp src/client.cpp)

# Link libraries
target_link_libraries(node_app zmq)
target_link_libraries(client_app zmq)
