# - Find MS Visual C++
#
#  VC_INCLUDE_DIR  - where to find headers
#  VC_INCLUDE_DIRS - where to find headers
#  VC_LIBRARY_DIR  - where to find libraries
#  VC_FOUND        - True if MSVC found.

MACRO(DETECT_VC_VERSION_HELPER _ROOT _VERSION)
  # Software/Wow6432Node/...
  GET_FILENAME_COMPONENT(VC${_VERSION}_DIR "[${_ROOT}\\SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7;${_VERSION}]" ABSOLUTE)

  IF(VC${_VERSION}_DIR AND VC${_VERSION}_DIR STREQUAL "/registry")
    SET(VC${_VERSION}_DIR)
    GET_FILENAME_COMPONENT(VC${_VERSION}_DIR "[${_ROOT}\\SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7;${_VERSION}]" ABSOLUTE)
    IF(VC${_VERSION}_DIR AND NOT VC${_VERSION}_DIR STREQUAL "/registry")
      SET(VC${_VERSION}_DIR "${VC${_VERSION}_DIR}VC/")
    ENDIF(VC${_VERSION}_DIR AND NOT VC${_VERSION}_DIR STREQUAL "/registry")
  ENDIF(VC${_VERSION}_DIR AND VC${_VERSION}_DIR STREQUAL "/registry")

  IF(VC${_VERSION}_DIR AND NOT VC${_VERSION}_DIR STREQUAL "/registry")
    SET(VC${_VERSION}_FOUND ON)
    IF(NOT MSVC_FIND_QUIETLY)
      MESSAGE(STATUS "Found Visual C++ ${_VERSION} in ${VC${_VERSION}_DIR}")
    ENDIF(NOT MSVC_FIND_QUIETLY)
  ELSEIF(VC${_VERSION}_DIR AND NOT VC${_VERSION}_DIR STREQUAL "/registry")
    SET(VC${_VERSION}_FOUND OFF)
    SET(VC${_VERSION}_DIR "")
  ENDIF(VC${_VERSION}_DIR AND NOT VC${_VERSION}_DIR STREQUAL "/registry")
ENDMACRO(DETECT_VC_VERSION_HELPER)

MACRO(DETECT_VC_VERSION _VERSION)
  SET(VC${_VERSION}_FOUND OFF)
  DETECT_VC_VERSION_HELPER("HKEY_CURRENT_USER" ${_VERSION})

  IF(NOT VC${_VERSION}_FOUND)
    DETECT_VC_VERSION_HELPER("HKEY_LOCAL_MACHINE" ${_VERSION})
  ENDIF(NOT VC${_VERSION}_FOUND)

  IF(VC${_VERSION}_FOUND)
    SET(VC_FOUND ON)
    SET(VC_DIR "${VC${_VERSION}_DIR}")
  ENDIF(VC${_VERSION}_FOUND)
ENDMACRO(DETECT_VC_VERSION)

IF(MSVC11)
  DETECT_VC_VERSION("11.0")

  IF(NOT MSVC11_REDIST_DIR)
    # If you have VC++ 2012 Express, put x64/Microsoft.VC110.CRT/*.dll in ${EXTERNAL_PATH}/redist
    SET(MSVC11_REDIST_DIR "${EXTERNAL_PATH}/redist")
  ENDIF(NOT MSVC11_REDIST_DIR)
ELSEIF(MSVC10)
  DETECT_VC_VERSION("10.0")

  IF(NOT MSVC10_REDIST_DIR)
    # If you have VC++ 2010 Express, put x64/Microsoft.VC100.CRT/*.dll in ${EXTERNAL_PATH}/redist
    SET(MSVC10_REDIST_DIR "${EXTERNAL_PATH}/redist")
  ENDIF(NOT MSVC10_REDIST_DIR)
ELSEIF(MSVC90)
  DETECT_VC_VERSION("9.0")
ELSEIF(MSVC80)
  DETECT_VC_VERSION("8.0")
ENDIF(MSVC11)

# If you plan to use VC++ compilers with WINE, set VC_DIR environment variable
IF(NOT VC_DIR)
  SET(VC_DIR $ENV{VC_DIR})
ENDIF(NOT VC_DIR)

IF(NOT VC_DIR)
  STRING(REGEX REPLACE "/bin/.+" "" VC_DIR ${CMAKE_CXX_COMPILER})
ENDIF(NOT VC_DIR)

SET(VC_INCLUDE_DIR "${VC_DIR}/include")
SET(VC_INCLUDE_DIRS ${VC_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VC_INCLUDE_DIR})
