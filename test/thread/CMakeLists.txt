# Generic test that uses conan libs

add_executable(thread Thread.cpp)
#find_package(folly CONFIG REQUIRED)
target_link_libraries(
  thread PRIVATE loki Thread::Threads
                )
