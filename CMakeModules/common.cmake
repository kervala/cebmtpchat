###
# Helper macro that generates .pc and installs it.
# Argument: name - the name of the .pc package, e.g. "mylib.pc"
###
MACRO(GEN_PKGCONFIG name)
  IF(NOT WIN32)
    CONFIGURE_FILE(${name}.in "${CMAKE_CURRENT_BINARY_DIR}/${name}")
    INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/${name}" DESTINATION lib/pkgconfig)
  ENDIF(NOT WIN32)
ENDMACRO(GEN_PKGCONFIG)

###
# Helper macro that generates config.h from config.h.in
###
MACRO(GEN_CONFIG_H)
  IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in)
    # convert relative to absolute paths
    IF(WIN32)
      SET(OLD_ETC_PREFIX ${ETC_PREFIX})
      SET(OLD_SHARE_PREFIX ${SHARE_PREFIX})
      SET(OLD_SBIN_PREFIX ${SBIN_PREFIX})
      SET(OLD_BIN_PREFIX ${BIN_PREFIX})
      SET(OLD_INCLUDE_PREFIX ${INCLUDE_PREFIX})
      SET(OLD_LIB_PREFIX ${LIB_PREFIX})
      SET(OLD_PLUGIN_PREFIX ${PLUGIN_PREFIX})

      SET(ETC_PREFIX "${PREFIX}/${ETC_PREFIX}")
      SET(SHARE_PREFIX "${PREFIX}/${SHARE_PREFIX}")
      SET(SBIN_PREFIX "${PREFIX}/${SBIN_PREFIX}")
      SET(BIN_PREFIX "${PREFIX}/${BIN_PREFIX}")
      SET(INCLUDE_PREFIX "${PREFIX}/${INCLUDE_PREFIX}")
      SET(LIB_PREFIX "${PREFIX}/${LIB_PREFIX}")
      SET(PLUGIN_PREFIX "${PREFIX}/${PLUGIN_PREFIX}")

      IF(NOT TARGET_ICON)
        SET(TARGET_ICON "${TARGET}.ico")
      ENDIF(NOT TARGET_ICON)
    ENDIF(WIN32)

    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
    ADD_DEFINITIONS(-DHAVE_CONFIG_H)

    IF(WIN32)
      SET(ETC_PREFIX ${OLD_ETC_PREFIX})
      SET(SHARE_PREFIX ${OLD_SHARE_PREFIX})
      SET(SBIN_PREFIX ${OLD_SBIN_PREFIX})
      SET(BIN_PREFIX ${OLD_BIN_PREFIX})
      SET(INCLUDE_PREFIX ${OLD_INCLUDE_PREFIX})
      SET(LIB_PREFIX ${OLD_LIB_PREFIX})
      SET(PLUGIN_PREFIX ${OLD_PLUGIN_PREFIX})
    ENDIF(WIN32)
  ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in)
ENDMACRO(GEN_CONFIG_H)

###
# Helper macro that generates revision.h from revision.h.in
###
MACRO(GEN_REVISION_H)
  IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.svn/")
    FIND_PACKAGE(Subversion)

    IF(Subversion_FOUND)
      Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} ER)
      SET(REVISION ${ER_WC_REVISION})
    ENDIF(Subversion_FOUND)
  ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.svn/")

  IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.hg/")
    FIND_PACKAGE(Mercurial)

    IF(Mercurial_FOUND)
      Mercurial_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} ER)
      SET(REVISION ${ER_WC_REVISION})
    ENDIF(Mercurial_FOUND)
  ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.hg/")
  
  IF(REVISION AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/revision.h.in)
#    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/revision.h.in ${CMAKE_CURRENT_BINARY_DIR}/revision.h.txt)
#    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different revision.h.txt revision.h)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
    ADD_DEFINITIONS(-DHAVE_REVISION_H)
	
    # a custom target that is always built
    ADD_CUSTOM_TARGET(revision ALL
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/revision.h)

    # creates svnheader.h using cmake script
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/revision.h
      COMMAND ${CMAKE_COMMAND}
      -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
      -DSubversion_SVN_EXECUTABLE=${Subversion_SVN_EXECUTABLE}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/GetRevision.cmake)

    # revision.h is a generated file
    SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/revision.h
      PROPERTIES GENERATED TRUE
      HEADER_FILE_ONLY TRUE)
  ENDIF(REVISION AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/revision.h.in)
ENDMACRO(GEN_REVISION_H)

MACRO(SIGN_FILE target)
  IF(WITH_SIGN_FILE AND WIN32 AND WINSDK_SIGNTOOL AND ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    GET_TARGET_PROPERTY(filename ${target} LOCATION)
#    ADD_CUSTOM_COMMAND(
#      TARGET ${target}
#      POST_BUILD
#      COMMAND ${WINSDK_SIGNTOOL} sign ${filename}
#      VERBATIM)
  ENDIF(WITH_SIGN_FILE AND WIN32 AND WINSDK_SIGNTOOL AND ${CMAKE_BUILD_TYPE} STREQUAL "Release")
ENDMACRO(SIGN_FILE)

###
#
###
MACRO(SET_TARGET_EXECUTABLE name)
  ADD_EXECUTABLE(${name} ${ARGN})

  IF(REVISION)
    # explicitly say that the executable depends on the svnheader
    ADD_DEPENDENCIES(${name} revision)
  ENDIF(REVISION)
  
  IF(WIN32)
    SET_TARGET_PROPERTIES(${name} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS") #  /ENTRY:mainCRTStartup
  ENDIF(WIN32)

  INSTALL(TARGETS ${name} RUNTIME DESTINATION ${BIN_PREFIX})
  SIGN_FILE(${name})
ENDMACRO(SET_TARGET_EXECUTABLE)

###
#
###
MACRO(SET_TARGET_SERVICE name)
  ADD_EXECUTABLE(${name} ${ARGN})

  IF(REVISION)
    # explicitly say that the executable depends on the svnheader
    ADD_DEPENDENCIES(${name} revision)
  ENDIF(REVISION)

  INSTALL(TARGETS ${name} RUNTIME DESTINATION ${SBIN_PREFIX})
  SIGN_FILE(${name})
ENDMACRO(SET_TARGET_SERVICE)

###
#
###
MACRO(SET_TARGET_CONSOLE_EXECUTABLE name)
  ADD_EXECUTABLE(${name} ${ARGN})

  IF(REVISION)
    # explicitly say that the executable depends on the svnheader
    ADD_DEPENDENCIES(${name} revision)
  ENDIF(REVISION)

  INSTALL(TARGETS ${name} RUNTIME DESTINATION ${BIN_PREFIX})
  SIGN_FILE(${name})
ENDMACRO(SET_TARGET_CONSOLE_EXECUTABLE)

###
#
###
MACRO(SET_TARGET_GUI_EXECUTABLE name)
  ADD_EXECUTABLE(${name} WIN32 ${ARGN})

  IF(REVISION)
    # explicitly say that the executable depends on the svnheader
    ADD_DEPENDENCIES(${name} revision)
  ENDIF(REVISION)

  INSTALL(TARGETS ${name} RUNTIME DESTINATION ${BIN_PREFIX})
  SIGN_FILE(${name})
ENDMACRO(SET_TARGET_GUI_EXECUTABLE)

###
#
###
MACRO(SET_TARGET_LIB name)
  IF(WITH_STATIC)
    ADD_LIBRARY(${name} STATIC ${ARGN})
  ELSE(WITH_STATIC)
    ADD_LIBRARY(${name} SHARED ${ARGN})
    SIGN_FILE(${name})
  ENDIF(WITH_STATIC)
  # To prevent other libraries to be linked to the same libraries
  SET_TARGET_PROPERTIES(${name} PROPERTIES LINK_INTERFACE_LIBRARIES "")

  IF(WITH_PREFIX_LIB)
    SET_TARGET_PROPERTIES(${name} PROPERTIES PREFIX "lib")
  ENDIF(WITH_PREFIX_LIB)

  IF(WIN32)
    # DLL is in bin directory under Windows
    SET(LIBRARY_DEST ${BIN_PREFIX})
  ELSE(WIN32)
    SET(LIBRARY_DEST ${LIB_PREFIX})
  ENDIF(WIN32)

  IF((WITH_INSTALL_LIBRARIES AND WITH_STATIC) OR NOT WITH_STATIC)
    INSTALL(TARGETS ${name} RUNTIME DESTINATION ${BIN_PREFIX} LIBRARY DESTINATION ${LIBRARY_DEST} ARCHIVE DESTINATION ${LIB_PREFIX})
  ENDIF((WITH_INSTALL_LIBRARIES AND WITH_STATIC) OR NOT WITH_STATIC)
ENDMACRO(SET_TARGET_LIB)

###
#
###
MACRO(SET_TARGET_PLUGIN name)
  IF(WITH_STATIC_PLUGINS)
    ADD_LIBRARY(${name} STATIC ${ARGN})
  ELSE(WITH_STATIC_PLUGINS)
    ADD_LIBRARY(${name} MODULE ${ARGN})
    SIGN_FILE(${name})
  ENDIF(WITH_STATIC_PLUGINS)

  IF(WIN32)
    # DLL is in bin directory under Windows
    SET(PLUGIN_DEST ${PLUGIN_PREFIX})
  ELSE(WIN32)
    SET(PLUGIN_DEST ${PLUGIN_PREFIX})
  ENDIF(WIN32)

  IF((WITH_INSTALL_LIBRARIES AND WITH_STATIC_PLUGINS) OR NOT WITH_STATIC_PLUGINS)
    INSTALL(TARGETS ${name} LIBRARY DESTINATION ${PLUGIN_DEST} ARCHIVE DESTINATION ${LIB_PREFIX})
  ENDIF((WITH_INSTALL_LIBRARIES AND WITH_STATIC_PLUGINS) OR NOT WITH_STATIC_PLUGINS)
ENDMACRO(SET_TARGET_PLUGIN)

###
# Helper macro that sets the default library properties.
# Argument: name - the target name whose properties are being set
# Argument:
###
MACRO(SET_DEFAULT_PROPS name label)
  IF(${ARGC} EQUAL 3)
    SET_TARGET_PROPERTIES(${name} PROPERTIES SUFFIX .${ARGN})
  ENDIF(${ARGC} EQUAL 3)

  IF(NAMESPACE)
    SET(filename "${NAMESPACE}_${name}")
  ENDIF(NAMESPACE)

  SET_TARGET_PROPERTIES(${name} PROPERTIES PROJECT_LABEL ${label})
  GET_TARGET_PROPERTY(type ${name} TYPE)

  IF(filename)
    SET_TARGET_PROPERTIES(${name} PROPERTIES OUTPUT_NAME ${filename})
  ENDIF(filename)

  IF(${type} STREQUAL SHARED_LIBRARY)
    # Set versions only if target is a shared library
    SET_TARGET_PROPERTIES(${name} PROPERTIES
      VERSION ${VERSION}
      SOVERSION ${VERSION_MAJOR})
    IF(LIB_PREFIX)
      SET_TARGET_PROPERTIES(${name} PROPERTIES INSTALL_NAME_DIR ${LIB_PREFIX})
    ENDIF(LIB_PREFIX)
  ENDIF(${type} STREQUAL SHARED_LIBRARY)

  IF(${type} STREQUAL EXECUTABLE AND WIN32)
    SET_TARGET_PROPERTIES(${name} PROPERTIES
      VERSION ${VERSION}
      SOVERSION ${VERSION_MAJOR}
      COMPILE_FLAGS "/GA"
      LINK_FLAGS "/VERSION:${VERSION}")
  ENDIF(${type} STREQUAL EXECUTABLE AND WIN32)

  IF(WITH_STLPORT)
    TARGET_LINK_LIBRARIES(${name} ${STLPORT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    IF(WIN32)
      SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS "/X")
    ENDIF(WIN32)
  ENDIF(WITH_STLPORT)

  IF(WIN32)
    SET_TARGET_PROPERTIES(${name} PROPERTIES DEBUG_POSTFIX "d" RELEASE_POSTFIX "")
  ENDIF(WIN32)
ENDMACRO(SET_DEFAULT_PROPS)

###
# Checks build vs. source location. Prevents In-Source builds.
###
MACRO(CHECK_OUT_OF_SOURCE)
  IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "

CMake generation for this project is not allowed within the source directory!
Remove the CMakeCache.txt file and try again from another folder, e.g.:

   rm CMakeCache.txt
   mkdir cmake
   cd cmake
   cmake -G \"Unix Makefiles\" ..
    ")
  ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

ENDMACRO(CHECK_OUT_OF_SOURCE)

MACRO(SETUP_DEFAULT_OPTIONS)
  ###
  # Features
  ###
  OPTION(WITH_WARNINGS            "Show all warnings"                             OFF)
  OPTION(WITH_LOGGING             "With Logging"                                  ON )
  OPTION(WITH_COVERAGE            "With Code Coverage Support"                    OFF)
  OPTION(WITH_PCH                 "With Precompiled Headers"                      ON )
  IF(WIN32)
    OPTION(WITH_STATIC            "With static libraries."                        ON )
  ELSE(WIN32)
    OPTION(WITH_STATIC            "With static libraries."                        OFF)
  ENDIF(WIN32)
  OPTION(WITH_STATIC_PLUGINS      "With static plugins."                          OFF)
  OPTION(WITH_STATIC_EXTERNAL     "With static external libraries"                OFF)
  OPTION(WITH_SIGN_FILE           "Sign files."                                   OFF)
  IF(WIN32)
    OPTION(WITH_UNIX_STRUCTURE    "With UNIX structure (bin, include, lib)."      OFF)
  ELSE(WIN32)
    OPTION(WITH_UNIX_STRUCTURE    "With UNIX structure (bin, include, lib)."      ON )
  ENDIF(WIN32)
  OPTION(WITH_PREFIX_LIB          "With lib prefix for libraries."                OFF)
  OPTION(WITH_INSTALL_LIBRARIES   "With libraries installation."                  ON )
  OPTION(WITH_UPDATE_TRANSLATIONS "Update translations from sources."             OFF)

  ###
  # Optional support
  ###
  OPTION(WITH_STLPORT             "With STLport support."                         OFF)
  OPTION(WITH_RTTI                "With RTTI support."                            ON )
  OPTION(WITH_EXCEPTIONS          "With exceptions support."                      ON )
ENDMACRO(SETUP_DEFAULT_OPTIONS)

MACRO(SETUP_BUILD)
  # Force out of source builds.
#  CHECK_OUT_OF_SOURCE()

  #-----------------------------------------------------------------------------
  # Redirect output files
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

  # DLL should be in the same directory as EXE under Windows
  IF(WIN32)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  ELSE(WIN32)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  ENDIF(WIN32)

  SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

  IF(NOT CMAKE_BUILD_TYPE MATCHES "Debug" AND NOT CMAKE_BUILD_TYPE MATCHES "Release")
    # enforce release mode if it's neither Debug nor Release
    SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  ENDIF(NOT CMAKE_BUILD_TYPE MATCHES "Debug" AND NOT CMAKE_BUILD_TYPE MATCHES "Release")

  IF(WIN32)
    IF(MSVC10)
      # /Ox is working with VC++ 2010, but custom optimizations don't exist
      SET(SPEED_OPTIMIZATIONS "/Ox /GF /GS-")
      # without inlining it's unusable, use custom optimizations again
      SET(MIN_OPTIMIZATIONS "/Od /Ob1")
	ELSEIF(MSVC90)
      # don't use a /O[012x] flag if you want custom optimizations
      SET(SPEED_OPTIMIZATIONS "/Ob2 /Oi /Ot /Oy /GT /GF /GS-")
      # without inlining it's unusable, use custom optimizations again
      SET(MIN_OPTIMIZATIONS "/Ob1")
	ELSEIF(MSVC80)
      # don't use a /O[012x] flag if you want custom optimizations
      SET(SPEED_OPTIMIZATIONS "/Ox /GF /GS-")
      # without inlining it's unusable, use custom optimizations again
      SET(MIN_OPTIMIZATIONS "/Od /Ob1")
	ELSE(MSVC10)
      MESSAGE(FATAL_ERROR "Can't determine compiler version ${MSVC_VERSION}")	
    ENDIF(MSVC10)

    SET(PLATFORM_CFLAGS "${PLATFORM_CFLAGS} /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_WARNINGS /DWIN32 /D_WINDOWS /W3 /Zi /Zm1000 /MP /wd4250")
    SET(PLATFORM_CXXFLAGS "${PLATFORM_CFLAGS}")

    # Exceptions are only set for C++
    IF(WITH_EXCEPTIONS)
      SET(PLATFORM_CXXFLAGS "${PLATFORM_CXXFLAGS} /EHsc")
    ENDIF(WITH_EXCEPTIONS)

    # RTTI is only set for C++
    IF(WITH_RTTI)
      SET(PLATFORM_CXXFLAGS "${PLATFORM_CXXFLAGS} /GR")
    ELSE(WITH_RTTI)
      SET(PLATFORM_CXXFLAGS "${PLATFORM_CXXFLAGS} /GR-")
    ENDIF(WITH_RTTI)

    # Common link flags
    SET(PLATFORM_LINKFLAGS "-DEBUG")

    IF(TARGET_X64)
      # Fix a bug with Intellisense
      SET(PLATFORM_CFLAGS "${PLATFORM_CFLAGS} /D_WIN64")
      # Fix a compilation error for some big C++ files
      SET(MIN_OPTIMIZATIONS "${MIN_OPTIMIZATIONS} /bigobj")
    ELSE(TARGET_X64)
      # Allows 32 bits applications to use 3 GB of RAM
      SET(PLATFORM_LINKFLAGS "${PLATFORM_LINKFLAGS} /LARGEADDRESSAWARE")
    ENDIF(TARGET_X64)

    SET(DEBUG_CFLAGS "/MDd /RTC1 /D_DEBUG /DDEBUG ${MIN_OPTIMIZATIONS}")
    SET(RELEASE_CFLAGS "/MD /DNDEBUG ${SPEED_OPTIMIZATIONS}")
    SET(DEBUG_LINKFLAGS "/NODEFAULTLIB:msvcrt /INCREMENTAL:YES")
    SET(RELEASE_LINKFLAGS "/OPT:REF /OPT:ICF /INCREMENTAL:NO")
  ELSE(WIN32)
    SET(PLATFORM_CFLAGS "-g -D_REENTRANT")

    IF(WITH_COVERAGE)
      SET(PLATFORM_CFLAGS "-fprofile-arcs -ftest-coverage ${PLATFORM_CFLAGS}")
    ENDIF(WITH_COVERAGE)

    IF(WITH_WARNINGS)
      SET(PLATFORM_CFLAGS "-Wall -ansi ${PLATFORM_CFLAGS}")
    ENDIF(WITH_WARNINGS)

    IF(APPLE)
      SET(PLATFORM_CFLAGS "-gdwarf-2 ${PLATFORM_CFLAGS}")
    ENDIF(APPLE)

    SET(PLATFORM_CXXFLAGS ${PLATFORM_CFLAGS})

    # Exceptions are only set for C++
    IF(NOT WITH_EXCEPTIONS)
      SET(PLATFORM_CXXFLAGS "${PLATFORM_CXXFLAGS} -fno-exceptions")
    ENDIF(NOT WITH_EXCEPTIONS)

    # RTTI is only set for C++
    IF(NOT WITH_RTTI)
      SET(PLATFORM_CXXFLAGS "${PLATFORM_CXXFLAGS} -fno-rtti")
    ENDIF(NOT WITH_RTTI)

    IF(NOT APPLE)
      SET(PLATFORM_LINKFLAGS "-Wl,--no-undefined -Wl,--as-needed")
    ENDIF(NOT APPLE)

    SET(DEBUG_CFLAGS "-D_DEBUG -DDEBUG")
    SET(RELEASE_CFLAGS "-DNDEBUG -O3")
  ENDIF(WIN32)

  # Determine target CPU
  IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")
    IF(NOT CMAKE_SIZEOF_VOID_P)
      INCLUDE (CheckTypeSize)
      CHECK_TYPE_SIZE("void*"  CMAKE_SIZEOF_VOID_P)
    ENDIF(NOT CMAKE_SIZEOF_VOID_P)

    # Using 32 or 64 bits libraries
    SET(TARGET_X86 1)
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
      SET(ARCH "x86_64")
      SET(TARGET_X64 1)
      ADD_DEFINITIONS(-DHAVE_X86_64)
    ELSE(CMAKE_SIZEOF_VOID_P EQUAL 8)
      SET(ARCH "x86")
      ADD_DEFINITIONS(-DHAVE_X86)
    ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)
#     ADD_DEFINITIONS(-DHAVE_IA64)
  ENDIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")

  INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/PCHSupport.cmake)
  
ENDMACRO(SETUP_BUILD)

MACRO(SETUP_BUILD_FLAGS)
  SET(CMAKE_C_FLAGS ${PLATFORM_CFLAGS} CACHE STRING "" FORCE)
  SET(CMAKE_CXX_FLAGS ${PLATFORM_CXXFLAGS} CACHE STRING "" FORCE)

  ## Debug
  SET(CMAKE_C_FLAGS_DEBUG ${DEBUG_CFLAGS} CACHE STRING "" FORCE)
  SET(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_CFLAGS} CACHE STRING "" FORCE)
  SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${PLATFORM_LINKFLAGS} ${DEBUG_LINKFLAGS}" CACHE STRING "" FORCE)
  SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${PLATFORM_LINKFLAGS} ${DEBUG_LINKFLAGS}" CACHE STRING "" FORCE)
  SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${PLATFORM_LINKFLAGS} ${DEBUG_LINKFLAGS}" CACHE STRING "" FORCE)

  ## Release
  SET(CMAKE_C_FLAGS_RELEASE ${RELEASE_CFLAGS} CACHE STRING "" FORCE)
  SET(CMAKE_CXX_FLAGS_RELEASE ${RELEASE_CFLAGS} CACHE STRING "" FORCE)
  SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${PLATFORM_LINKFLAGS} ${RELEASE_LINKFLAGS}" CACHE STRING "" FORCE)
  SET(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${PLATFORM_LINKFLAGS} ${RELEASE_LINKFLAGS}" CACHE STRING "" FORCE)
  SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${PLATFORM_LINKFLAGS} ${RELEASE_LINKFLAGS}" CACHE STRING "" FORCE)
ENDMACRO(SETUP_BUILD_FLAGS)

MACRO(SETUP_PREFIX_PATHS name)
  ## Allow override of install_prefix path.
  SET(PREFIX "${CMAKE_INSTALL_PREFIX}")

  IF(UNIX)
    ## Allow override of install_prefix/etc path.
    IF(NOT ETC_PREFIX)
      SET(ETC_PREFIX "${PREFIX}/etc/${name}" CACHE PATH "Installation path for configurations")
    ENDIF(NOT ETC_PREFIX)

    ## Allow override of install_prefix/share path.
    IF(NOT SHARE_PREFIX)
      SET(SHARE_PREFIX "${PREFIX}/share/${name}" CACHE PATH "Installation path for data.")
    ENDIF(NOT SHARE_PREFIX)

    ## Allow override of install_prefix/sbin path.
    IF(NOT SBIN_PREFIX)
      SET(SBIN_PREFIX "${PREFIX}/sbin" CACHE PATH "Installation path for admin tools and services.")
    ENDIF(NOT SBIN_PREFIX)

    ## Allow override of install_prefix/bin path.
    IF(NOT BIN_PREFIX)
      SET(BIN_PREFIX "${PREFIX}/bin" CACHE PATH "Installation path for tools and applications.")
    ENDIF(NOT BIN_PREFIX)

    ## Allow override of install_prefix/include path.
    IF(NOT INCLUDE_PREFIX)
      SET(INCLUDE_PREFIX "${PREFIX}/include" CACHE PATH "Installation path for headers.")
    ENDIF(NOT INCLUDE_PREFIX)

    ## Allow override of install_prefix/lib path.
    IF(NOT LIB_PREFIX)
      SET(LIB_PREFIX "${PREFIX}/lib" CACHE PATH "Installation path for libraries.")
    ENDIF(NOT LIB_PREFIX)

    ## Allow override of install_prefix/lib path.
    IF(NOT PLUGIN_PREFIX)
      SET(PLUGIN_PREFIX "${PREFIX}/lib/${name}" CACHE PATH "Installation path for plugins.")
    ENDIF(NOT PLUGIN_PREFIX)

    # Aliases for automake compatibility
    SET(prefix ${PREFIX})
    SET(exec_prefix ${BIN_PREFIX})
    SET(libdir ${LIB_PREFIX})
    SET(includedir ${PREFIX}/include)
  ENDIF(UNIX)
  IF(WIN32)
    IF(TARGET_X64)
      SET(SUFFIX "64")
    ENDIF(TARGET_X64)

    IF(WITH_UNIX_STRUCTURE)
      SET(ETC_PREFIX "etc/${name}")
      SET(SHARE_PREFIX "share/${name}")
      SET(SBIN_PREFIX "bin${SUFFIX}")
      SET(BIN_PREFIX "bin${SUFFIX}")
      SET(INCLUDE_PREFIX "include")
      SET(LIB_PREFIX "lib${SUFFIX}") # static libs
      SET(PLUGIN_PREFIX "bin${SUFFIX}")
    ELSE(WITH_UNIX_STRUCTURE)
      SET(ETC_PREFIX ".")
      SET(SHARE_PREFIX ".")
      SET(SBIN_PREFIX ".")
      SET(BIN_PREFIX ".")
      SET(INCLUDE_PREFIX "include")
      SET(LIB_PREFIX "lib${SUFFIX}")
      SET(PLUGIN_PREFIX ".")
    ENDIF(WITH_UNIX_STRUCTURE)
    SET(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${BIN_PREFIX})
  ENDIF(WIN32)
ENDMACRO(SETUP_PREFIX_PATHS)

MACRO(SETUP_EXTERNAL)
  IF(WIN32)
    FIND_PACKAGE(External REQUIRED)

    INCLUDE(${CMAKE_ROOT}/Modules/Platform/Windows-cl.cmake)
    IF(MSVC10)
      IF(NOT MSVC10_REDIST_DIR)
        # If you have VC++ 2010 Express, put x64/Microsoft.VC100.CRT/*.dll in ${EXTERNAL_PATH}/redist
        SET(MSVC10_REDIST_DIR "${EXTERNAL_PATH}/redist")
      ENDIF(NOT MSVC10_REDIST_DIR)

      GET_FILENAME_COMPONENT(VC_ROOT_DIR "[HKEY_CURRENT_USER\\Software\\Microsoft\\VisualStudio\\10.0_Config;InstallDir]" ABSOLUTE)
      # VC_ROOT_DIR is set to "registry" when a key is not found
      IF(VC_ROOT_DIR MATCHES "registry")
        GET_FILENAME_COMPONENT(VC_ROOT_DIR "[HKEY_CURRENT_USER\\Software\\Microsoft\\VCExpress\\10.0_Config;InstallDir]" ABSOLUTE)
        IF(VC_ROOT_DIR MATCHES "registry")
          MESSAGE(FATAL_ERROR "Unable to find VC++ 2010 directory!")
        ENDIF(VC_ROOT_DIR MATCHES "registry")
      ENDIF(VC_ROOT_DIR MATCHES "registry")
      # convert IDE fullpath to VC++ path
      STRING(REGEX REPLACE "Common7/.*" "VC" VC_DIR ${VC_ROOT_DIR})
    ELSE(MSVC10)
      IF(${CMAKE_MAKE_PROGRAM} MATCHES "Common7")
        # convert IDE fullpath to VC++ path
        STRING(REGEX REPLACE "Common7/.*" "VC" VC_DIR ${CMAKE_MAKE_PROGRAM})
      ELSE(${CMAKE_MAKE_PROGRAM} MATCHES "Common7")
        # convert compiler fullpath to VC++ path
        STRING(REGEX REPLACE "VC/bin/.+" "VC" VC_DIR ${CMAKE_CXX_COMPILER})
      ENDIF(${CMAKE_MAKE_PROGRAM} MATCHES "Common7")
    ENDIF(MSVC10)
  ELSE(WIN32)
    IF(CMAKE_FIND_LIBRARY_SUFFIXES AND NOT APPLE)
      IF(WITH_STATIC_EXTERNAL)
        SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
      ELSE(WITH_STATIC_EXTERNAL)
        SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
      ENDIF(WITH_STATIC_EXTERNAL)
    ENDIF(CMAKE_FIND_LIBRARY_SUFFIXES AND NOT APPLE)
  ENDIF(WIN32)

  IF(WITH_STLPORT)
    FIND_PACKAGE(STLport REQUIRED)
    INCLUDE_DIRECTORIES(${STLPORT_INCLUDE_DIR})
    IF(WIN32)
      SET(VC_INCLUDE_DIR "${VC_DIR}/include")

      FIND_PACKAGE(WindowsSDK REQUIRED)
      # use VC++ and Windows SDK include paths
      INCLUDE_DIRECTORIES(${VC_INCLUDE_DIR} ${WINSDK_INCLUDE_DIR})
    ENDIF(WIN32)
  ENDIF(WITH_STLPORT)
 
ENDMACRO(SETUP_EXTERNAL)
