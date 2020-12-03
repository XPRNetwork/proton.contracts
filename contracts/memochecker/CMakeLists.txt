add_contract(memochecker memochecker ${CMAKE_CURRENT_SOURCE_DIR}/src/memochecker.cpp)

target_include_directories(memochecker
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(memochecker
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")


configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/memochecker.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/memochecker.contracts.md @ONLY )

target_compile_options( memochecker PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )

