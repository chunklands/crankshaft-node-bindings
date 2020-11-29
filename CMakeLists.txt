cmake_minimum_required(VERSION 3.6)

set(CMAKE_BUILD_TYPE Debug)

project(crankshaft-node-binding)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "^(Apple)?Clang$")
  set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()

set(LIBCRANKSHAFT_NODE_BINDING_TARGET crankshaft-node-binding)

add_subdirectory(deps/crankshaft ${CMAKE_CURRENT_BINARY_DIR}/crankshaft)

file(GLOB_RECURSE LIBCRANKSHAFT_NODE_BINDING_TARGET_SRC src/*.cxx src/**/*.cxx)

add_library(
  ${LIBCRANKSHAFT_NODE_BINDING_TARGET}
  SHARED
    ${LIBCRANKSHAFT_NODE_BINDING_TARGET_SRC}
)

set_target_properties(
  ${LIBCRANKSHAFT_NODE_BINDING_TARGET}
  PROPERTIES
    SUFFIX .node
  )

# cpp version requirements
# - std::make_unique => cpp14
# - std::monostate => cpp17
set_property(TARGET ${LIBCRANKSHAFT_NODE_BINDING_TARGET} PROPERTY CXX_STANDARD 17)

target_include_directories(
  ${LIBCRANKSHAFT_NODE_BINDING_TARGET}
  PRIVATE
    node_modules/node-addon-api
    deps/node/src
    deps/crankshaft/include
)

target_link_libraries(
  ${LIBCRANKSHAFT_NODE_BINDING_TARGET}
  LINK_PUBLIC
    crankshaft
)
