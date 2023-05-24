# Generic test that uses conan libs

add_executable(smartptr SmartPtr.cpp)
#find_package(folly CONFIG REQUIRED)
target_link_libraries(
  smartptr PRIVATE loki Threads::Threads
                )
