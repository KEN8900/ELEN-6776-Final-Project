# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/ibmscppbackend_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/ibmscppbackend_autogen.dir/ParseCache.txt"
  "ibmscppbackend_autogen"
  )
endif()
