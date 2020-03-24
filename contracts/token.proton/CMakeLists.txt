add_contract(token.proton token.proton ${CMAKE_CURRENT_SOURCE_DIR}/src/token.proton.cpp)

target_include_directories(token.proton
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(token.proton
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")


configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/token.proton.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/token.proton.contracts.md @ONLY )

target_compile_options( token.proton PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )

