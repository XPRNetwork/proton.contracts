add_contract(eosio.proton eosio.proton ${CMAKE_CURRENT_SOURCE_DIR}/src/eosio.proton.cpp)

target_include_directories(eosio.proton
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../eosio.system/include)

set_target_properties(eosio.proton
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/eosio.proton.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/eosio.proton.contracts.md @ONLY )

target_compile_options( eosio.proton PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
