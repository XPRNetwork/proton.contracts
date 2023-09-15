cmake_minimum_required(VERSION 3.5)
set(CMAKE_DEPENDS_USE_COMPILER FALSE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-command-line-argument")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument -DEOSIO_NO_RICARDIAN")

project(proton)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(blanc)
include(EosioWasmToolchain)

add_contract(token.proton token.proton ${CMAKE_CURRENT_SOURCE_DIR}/src/token.proton.cpp)

target_include_directories(token.proton
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(token.proton
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")


configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/token.proton.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/token.proton.contracts.md @ONLY )

target_compile_options( token.proton PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
