FILE(GLOB SRC main.cpp
	${CMAKE_SOURCE_DIR}/ryzom/client/src/app_bundle_utils.cpp
	${CMAKE_SOURCE_DIR}/ryzom/client/src/user_agent.cpp
	${CMAKE_SOURCE_DIR}/ryzom/client/src/client_cfg.cpp
	${CMAKE_SOURCE_DIR}/ryzom/client/src/login_patch.cpp
	${CMAKE_SOURCE_DIR}/ryzom/client/src/login_xdelta.cpp
	${CMAKE_SOURCE_DIR}/ryzom/client/src/stdpch.cpp
	${CMAKE_SOURCE_DIR}/ryzom/client/src/stdpch.h
)

# always enable custom patch server
ADD_DEFINITIONS(${LIBXML2_DEFINITIONS} -DRZ_USE_CUSTOM_PATCH_SERVER)

ADD_EXECUTABLE(khanat_client_patcher ${SRC})

INCLUDE_DIRECTORIES(
			${LIBXML2_INCLUDE_DIR}
			${CURL_INCLUDE_DIRS}
			${ZLIB_INCLUDE_DIR}
			${CMAKE_SOURCE_DIR}/ryzom/client/src
)

TARGET_LINK_LIBRARIES(khanat_client_patcher
				nelmisc
				nelnet
				ryzom_gameshare
				ryzom_sevenzip
				${CURL_LIBRARIES})

IF(APPLE)
  FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)
  TARGET_LINK_LIBRARIES(khanat_client_patcher ${FOUNDATION_LIBRARY})
ENDIF(APPLE)

ADD_DEFINITIONS(${CURL_DEFINITIONS} -DRZ_NO_CLIENT -DNL_USE_SEVENZIP)

NL_DEFAULT_PROPS(khanat_client_patcher "Khanat, Tools: Khanat Client Patcher")
NL_ADD_RUNTIME_FLAGS(khanat_client_patcher)

IF(WITH_PCH)
  ADD_NATIVE_PRECOMPILED_HEADER(khanat_client_patcher ${CMAKE_SOURCE_DIR}/ryzom/client/src/stdpch.h ${CMAKE_SOURCE_DIR}/ryzom/client/src/stdpch.cpp)
ENDIF(WITH_PCH)

INSTALL(TARGETS khanat_client_patcher RUNTIME DESTINATION ${RYZOM_GAMES_PREFIX} COMPONENT client)
