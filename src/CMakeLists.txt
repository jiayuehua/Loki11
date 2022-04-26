# Generic test that uses conan libs
function(aoc arg)

  find_path(BOOST_MP11_INCLUDE_DIRS "boost/mp11.hpp")
add_executable(${arg} "${arg}.cpp")

target_include_directories(${arg} PRIVATE
          ${CMAKE_CURRENT_SOURCE_DIR} 
${BOOST_MP11_INCLUDE_DIRS}
           )
target_link_libraries(
  ${arg} PRIVATE project_options project_warnings loki
                 )
                   endfunction()
                   #aoc(world)
                   #aoc(assert)
#                   aoc(enforce)
#                   aoc(ad_hoc)
#                   aoc(minmax)
#                   aoc(typelist)
                   #aoc(abstractfactory)
                   #aoc(smartptr)
                 #  aoc(hop)

              #     aoc(Visitor)
            #       aoc(multimethod)
              #     aoc(orderedstatic)
#                   aoc(variant)
#                   aoc(arrow_proxy )
