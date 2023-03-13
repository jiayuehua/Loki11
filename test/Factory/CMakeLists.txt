# Generic test that uses conan libs

add_executable(factory Factory.cpp)


target_link_libraries(
  factory PRIVATE loki 
                )
