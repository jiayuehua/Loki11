# Generic test that uses conan libs

find_path(BOOST_MP11_INCLUDE_DIRS "boost/mp11.hpp")
find_package(Boost REQUIRED  )
add_library(loki  OrderedStatic.cpp  Singleton.cpp SmallObj.cpp SmartPtr.cpp StrongPtr.cpp  "loki/ThreadModel.h")
#add_library(loki )
target_compile_options(loki PRIVATE -fconcepts)

target_include_directories(loki PUBLIC
          ${CMAKE_CURRENT_SOURCE_DIR} 
${BOOST_MP11_INCLUDE_DIRS}
           )
target_link_libraries(
  loki PUBLIC project_options project_warnings Boost::boost
                )
