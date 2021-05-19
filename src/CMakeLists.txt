# Generic test that uses conan libs
function(aoc arg)
add_executable(${arg} "${arg}.cpp")
target_include_directories(${arg} PRIVATE
          ${CMAKE_CURRENT_SOURCE_DIR} 
           )
target_link_libraries(
  ${arg} PRIVATE project_options project_warnings 
                 )
                   endfunction()
                   aoc(world)
