add_contract(cfund.proton cfund.proton ${CMAKE_CURRENT_SOURCE_DIR}/src/cfund.proton.cpp)

target_include_directories(cfund.proton
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(cfund.proton
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")


configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/cfund.proton.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/cfund.proton.contracts.md @ONLY )

target_compile_options( cfund.proton PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )

