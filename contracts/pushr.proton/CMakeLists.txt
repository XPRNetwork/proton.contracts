add_contract(pushr.proton pushr.proton ${CMAKE_CURRENT_SOURCE_DIR}/src/pushr.proton.cpp)

target_include_directories(pushr.proton
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(pushr.proton
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/pushr.proton.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/pushr.proton.contracts.md @ONLY )

target_compile_options( pushr.proton PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
