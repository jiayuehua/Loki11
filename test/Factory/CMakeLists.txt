# Generic test that uses conan libs

add_executable(factory Factory.cpp)
#find_package(folly CONFIG REQUIRED)

target_link_libraries(
  factory PRIVATE loki 
                )
add_executable(clonefactory CloneFactory.cpp)
target_link_libraries(
  clonefactory PRIVATE loki 
                )
