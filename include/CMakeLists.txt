# Generic test that uses conan libs

find_path(BOOST_MP11_INCLUDE_DIRS "boost/mp11.hpp")
add_library(loki  OrderedStatic.cpp SafeFormat.cpp Singleton.cpp SmallObj.cpp SmartPtr.cpp StrongPtr.cpp)


target_include_directories(loki PUBLIC
          ${CMAKE_CURRENT_SOURCE_DIR} 
${BOOST_MP11_INCLUDE_DIRS}
           )
target_link_libraries(
  loki PRIVATE project_options project_warnings 
                )
