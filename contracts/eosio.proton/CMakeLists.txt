add_contract(eosio.proton eosio.proton ${CMAKE_CURRENT_SOURCE_DIR}/src/eosio.proton.cpp)

target_include_directories(eosio.proton
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(eosio.proton
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
