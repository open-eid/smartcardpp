cmake_minimum_required(VERSION 2.8)
project(smartcardpp)
find_package(OpenSSL REQUIRED)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
include_directories(${OPENSSL_INCLUDE_DIR})

add_library(smartcardpp STATIC
	CardBase.cpp
	locked_allocator.cpp
	PCSCManager.cpp
	SCError.cpp
	types.cpp
	md5.cpp
	SCardLog.cpp
	EstEIDManager.cpp
	Certificate.cpp
	SCardLog.cpp
	Certificate.cpp
)
if(WIN32)
	target_link_libraries(smartcardpp ws2_32 winscard ${LIBEAY_LIBRARY})
elseif(APPLE)
	target_link_libraries(smartcardpp "-framework PCSC" ${LIBEAY_LIBRARY})
else()
	find_package(PCSCLite REQUIRED)
	include_directories(${PCSCLITE_INCLUDE_DIR})
	target_link_libraries(smartcardpp ${PCSCLITE_LIBRARY} ${LIBEAY_LIBRARY})
endif()