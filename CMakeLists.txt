cmake_minimum_required(VERSION 3.6)

set(CMAKE_BUILD_TYPE Debug)

project(crankshaft-node-binding)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "^(Apple)?Clang$")
  set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()

set(LIBCRANKSHAFT_NODE_BINDING_TARGET crankshaft-node-binding)
# set(CRANKSHAFT_EXAMPLE) ...

add_subdirectory(deps/crankshaft)

add_library(
  ${LIBCRANKSHAFT_NODE_BINDING_TARGET}
  SHARED
    src/module.cxx
    src/module_engine.cxx
    src/module_window.cxx
)

set_target_properties(
  ${LIBCRANKSHAFT_NODE_BINDING_TARGET}
  PROPERTIES
    OUTPUT_NAME crankshaft
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

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcrankshaft.d.ts
  ${CMAKE_CURRENT_BINARY_DIR}
  COPYONLY
)