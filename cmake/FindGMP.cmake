find_path(GMP_INCLUDE_DIRS
  NAMES gmp.h
)

find_library(GMP_LIBRARIES gmp)

add_library(GMP::gmp UNKNOWN IMPORTED)
set_target_properties(GMP::gmp PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${GMP_INCLUDE_DIRS}
  IMPORTED_LOCATION ${GMP_LIBRARIES}
)
target_link_libraries(GMP::gmp INTERFACE ZLIB::ZLIB)

find_package_handle_standard_args(GMP DEFAULT_MSG
  GMP_INCLUDE_DIRS GMP_LIBRARIES
)
