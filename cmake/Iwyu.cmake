option(ENABLE_IWYU "Enable Include_What_You_Use" OFF)
if(ENABLE_IWYU)
  find_program(IWYU include-what-you-use)
  if(IWYU)
    message("use include-what-you-use")
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE
      "include-what-you-use;-Xiwyu;--transitive_includes_only")
  else()
    message(SEND_ERROR "iwyu requested but executable not found")
  endif()
endif()

