# Generic test that uses conan libs

add_executable(abstractfactory Factory.cpp)
target_link_libraries( abstractfactory PRIVATE loki)
