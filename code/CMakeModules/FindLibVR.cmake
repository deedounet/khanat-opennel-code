# - Locate LibVR library
# This module defines
#  LIBVR_LIBRARIES, the libraries to link against
#  LIBVR_FOUND, if false, do not try to link to LIBVR
#  LIBVR_INCLUDE_DIR, where to find headers.

IF(LIBVR_LIBRARIES AND LIBVR_INCLUDE_DIR)
  # in cache already
  SET(LIBVR_FIND_QUIETLY TRUE)
ENDIF()

FIND_PATH(LIBVR_INCLUDE_DIR hmd.h
  PATH_SUFFIXES include/LibVR
)

FIND_LIBRARY(LIBVR_LIBRARY
  NAMES vr
  PATH_SUFFIXES lib
  PATHS
)

IF(LIBVR_LIBRARY AND LIBVR_INCLUDE_DIR)
  IF(NOT LIBVR_FIND_QUIETLY)
    MESSAGE(STATUS "Found LibVR: ${LIBVR_LIBRARY}")
  ENDIF()
  SET(LIBVR_FOUND "YES")
  SET(LIBVR_DEFINITIONS "-DHAVE_LIBVR")
  SET(NL_STEREO_AVAILABLE ON)
ELSE()
  IF(NOT LIBVR_FIND_QUIETLY)
    MESSAGE(STATUS "Warning: Unable to find LibVR!")
  ENDIF()
ENDIF()
