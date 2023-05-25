# Generic test that uses conan libs

add_executable(multimethod Multimethod.cpp)
#find_package(folly CONFIG REQUIRED)
target_link_libraries(
  multimethod PRIVATE loki 
                )
