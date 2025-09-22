cmake_minimum_required(VERSION 3.29.2)

set(PROGRAM_VERSION "Elypso Hub 2026.1.0")
set(PROGRAM_VERSION_NUMBER 2026.1.0.0)

project("Elypso_Hub" VERSION ${PROGRAM_VERSION_NUMBER} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform Detection
if (WIN32)
    message(STATUS "[ELYPSO_HUB] Platform = Windows")
else()
    message(FATAL_ERROR "[ELYPSO_HUB] Unsupported platform. Only Windows is supported.")
endif()

# Build Type Detection
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IS_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(IS_RELEASE TRUE)
else()
    message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'! Must be Debug, Release, or RelWithDebInfo.")
endif()

# Runtime lib handling for MSVC
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Optional Resource File
set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/files/icon.rc")

# Directory Setup
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

set(EXT_SHARED_DIR "${CMAKE_SOURCE_DIR}/_external_shared")
set(EXT_WINDOW_DIR "${EXT_SHARED_DIR}/Kalawindow")
set(EXT_CRASH_DIR "${EXT_SHARED_DIR}/KalaCrashHandler")

# Library Paths
if(IS_RELEASE)
    set(WINDOW_LIBRARY_PATH "${EXT_WINDOW_DIR}/release/_KWnd03.lib")
    set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/release/KalaCrashHandler.lib")
else()
    set(WINDOW_LIBRARY_PATH "${EXT_WINDOW_DIR}/debug/_KWnd03d.lib")
    set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/debug/KalaCrashHandlerD.lib")
endif()

# Source Files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*/*.cpp"
)

# Executable
add_executable(Elypso_Hub ${SOURCE_FILES})

# Enable exception handling for MSVC
if (MSVC)
    target_compile_options(Elypso_Hub PRIVATE /EHsc)
endif()

set_target_properties(Elypso_Hub PROPERTIES OUTPUT_NAME "Elypso Hub")
target_compile_features(Elypso_Hub PRIVATE cxx_std_20)

# Includes
file(GLOB_RECURSE HEADERS
	configure_depends
	"${CMAKE_SOURCE_DIR}/include/*.hpp"
)
target_sources(Elypso_Hub PRIVATE ${HEADERS})
target_include_directories(Elypso_Hub PRIVATE
	"${INCLUDE_DIR}"
	"${EXT_SHARED_DIR}"
	"${EXT_SHARED_DIR}/KalaWindow/include"
	"${EXT_SHARED_DIR}/KalaCrashHandler/include"
)

# Preprocessor Defines
target_compile_definitions(Elypso_Hub PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)

# Link libraries
target_link_libraries(Elypso_Hub PRIVATE
	opengl32
	${WINDOW_LIBRARY_PATH}
	${CRASH_LIBRARY_PATH})

# Hide console in release mode
if(IS_RELEASE)
    set_target_properties(Elypso_Hub PROPERTIES WIN32_EXECUTABLE TRUE)
    if (MSVC)
        set_target_properties(Elypso_Hub PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif()
endif()

# Set Windows Details tab data
set(ICON_DIR "${CMAKE_SOURCE_DIR}/files/icon.ico")

if (IS_RELEASE)
    set(FILEFLAGS "0x00")
else()
	set(FILEFLAGS "0x01")
endif()
	
set(FILETYPE 0x1)    # 0x1 - executable, 0x2 - dynamic-link library
set(FILESUBTYPE 0x0) # 0x0 = normal DLL, only change if you're making drivers/fonts

# Public fields displayed in Windows Explorer
set(FILE_DESCRIPTION  "Elypso Hub")
set(FILE_VERSION      "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")
set(PRODUCT_NAME      "Elypso Hub")
set(PRODUCT_VERSION   "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")
set(COMPANY_NAME      "Lost Empire Entertainment")
set(LEGAL_COPYRIGHT   "(C) 2025 Lost Empire Entertainment")
set(LEGAL_TRADEMARKS  "Trademark of Lost Empire Entertainment")
set(ORIGINAL_FILENAME "${PRODUCT_NAME}")

# Private/optional fields
set(INTERNAL_NAME "${PRODUCT_NAME}")
set(COMMENTS      "${PRODUCT_NAME}")
set(PRIVATE_BUILD "${PRODUCT_NAME}")
set(SPECIAL_BUILD "${PRODUCT_NAME}")

# Generate version.rc
configure_file(
	${CMAKE_SOURCE_DIR}/version.rc.in
	${CMAKE_BINARY_DIR}/version.rc
	@ONLY
)

target_sources(Elypso_Hub PRIVATE "${CMAKE_BINARY_DIR}/version.rc")

# Expose version string
add_compile_definitions(
	PROGRAM_VERSION="${PROGRAM_VERSION}"
)

# Installation
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Elypso_Hub DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy files directory
add_custom_command(TARGET Elypso_Hub POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_FILE_DIR:Elypso_Hub>/files"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/files" "$<TARGET_FILE_DIR:Elypso_Hub>/files"
)

# Copy external binaries
if(IS_RELEASE)
	set(BIN_KALAWINDOW "${EXT_WINDOW_DIR}/release")
	set(BIN_KALACRASHHANDLER "${EXT_CRASH_DIR}/release")
else()
	set(BIN_KALAWINDOW "${EXT_WINDOW_DIR}/debug")
	set(BIN_KALACRASHHANDLER "${EXT_CRASH_DIR}/debug")
endif()

if (IS_RELEASE)
	file(GLOB BIN_FILES
		"${BIN_KALAWINDOW}/*.dll"
		"${BIN_KALACRASHHANDLER}/*.dll"
	)
else()
	file(GLOB BIN_FILES
		"${BIN_KALAWINDOW}/*.dll"
		"${BIN_KALAWINDOW}/*.pdb"
		"${BIN_KALACRASHHANDLER}/*.dll"
	)
endif()

set(BIN_TARGET_DIR "$<TARGET_FILE_DIR:Elypso_Hub>")

foreach(BIN_FILE ${BIN_FILES})
    get_filename_component(BIN_NAME ${BIN_FILE} NAME)
    add_custom_command(TARGET Elypso_Hub POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
			"${BIN_FILE}"
			"${BIN_TARGET_DIR}/${BIN_NAME}"
    )
endforeach()

# Copy important files
set(COPY_TARGETS README.md LICENSE.md)
foreach(FILE ${COPY_TARGETS})
add_custom_command(TARGET Elypso_Hub POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove 
		"$<TARGET_FILE_DIR:Elypso_Hub>/${FILE}"
    COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_SOURCE_DIR}/${FILE}"
		"$<TARGET_FILE_DIR:Elypso_Hub>/${FILE}"
)
endforeach()

# Copy entire docs folder
add_custom_command(TARGET Elypso_Hub POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
		"$<TARGET_FILE_DIR:Elypso_Hub>/docs"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/docs"
		"$<TARGET_FILE_DIR:Elypso_Hub>/docs"
)

# Package
include(CPack)
