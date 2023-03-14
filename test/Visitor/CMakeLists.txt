# Generic test that uses conan libs

add_executable(visitor main.cpp)


target_link_libraries(
  visitor PRIVATE loki 
                )
