# For Emscripten, invoke something like:
# source ~/emsdk_portable/emsdk_env.sh;
# mkdir BUILD; cd BUILD;
# cmake -DCMAKE_TOOLCHAIN_FILE=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release ..

# For Mac, you must use the Xcode generator. Makefile is currently broken.
# mkdir BUILD; cd BUILD;
# cmake -GXcode ..

# For iOS, you must use the Xcode generator and the bundled toolchain
# mkdir BUILD; cd BUILD;
# cmake -DCMAKE_TOOLCHAIN_FILE=../CMakeModules/iOS.toolchain.cmake -GXcode ..


if(APPLE)
	# We're using the new Mac RPATH policy.
	CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
elseif(ANDROID)
	cmake_minimum_required(VERSION 3.6)
else()
	# Ubuntu 12.04 LTS ships 2.8.7
	CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
endif()

if(POLICY CMP0054)
	cmake_policy(SET CMP0054 NEW)
endif()

PROJECT(mpg123)

#enable_language(ASM_NASM)
enable_language(ASM)

INCLUDE(CMakeDependentOption)
INCLUDE(CheckFunctionExists)
#SET(CPACK_SET_DESTDIR TRUE)
#INCLUDE(CPack)

# By default, we'll exclude the tests from the ALL target. But for convenience, we also will provide a testsiup aggregate target.
#OPTION(MPG123LIB_BUILD_TESTS "Build tests with ALL target" OFF)
#SET(BIAP_TESTS_CUSTOM_TARGET_NAME "testsiup")
#ADD_CUSTOM_TARGET(${BIAP_TESTS_CUSTOM_TARGET_NAME})

macro(USE_C99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
    endif ()
  else ()
    set (CMAKE_C_STANDARD 99)
  endif ()
endmacro(USE_C99)

USE_C99()

IF(EMSCRIPTEN)
	# Emscripten documentation says .so is better than .a, but also implies .bc may be best.
	# TODO: Figure out how .bc fits into all of this.
	OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" ON)
ELSEIF(APPLE)
	IF(IOS)
		OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" OFF)
	ELSE()
		OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" ON)
		CMAKE_DEPENDENT_OPTION(WANTS_BUILD_FRAMEWORK "Set to ON to build framework instead of dylib. Only valid if BUILD_SHARED_LIBRARY is ON an is OS X." ON "WANTS_BUILD_SHARED_LIBRARY" ON)
	ENDIF()
ELSEIF(ANDROID)
	OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" ON)
ELSEIF(UNIX)
	OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" ON)
ELSE()
	OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" ON)
ENDIF()

IF(MSVC)
	ADD_DEFINITIONS(/D _CRT_SECURE_NO_WARNINGS)
ENDIF()


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules/" ${CMAKE_MODULE_PATH})
INCLUDE(HelperFunctions)

IF(EMSCRIPTEN)

ELSEIF(APPLE)
	IF(IOS)

	ELSE()

	ENDIF()


ELSEIF(ANDROID)

ELSEIF(WIN32)

ELSEIF(UNIX)

ENDIF()



# RPATH stuff. This MUST be set before any targets that use it are declared.
# Only affects shared libraries.
if(WANTS_BUILD_SHARED_LIBRARY)
	HELPER_SETUP_RPATH()
endif()


SET(MPG123LIB_C_FLAGS "")
SET(MPG123LIB_LINK_FLAGS "")
#SET(MPG123EXE_C_FLAGS "")
#SET(MPG123EXE_LINK_FLAGS "")

IF(EMSCRIPTEN)
	# we always need to set this when building a libray with Emscripten
	ADD_DEFINITIONS(-DMPG123_EXPORT=EMSCRIPTEN_KEEPALIVE)
	
	# we always need to set this when using an Emscripten built library, otherwise symbols get stripped.
	#	SET(MPG123EXE_C_FLAGS "${MPG123EXE_C_FLAGS} -DMPG123_EXPORT")

	# Without this, all executables only generate .js files without the html wrapper.
	SET(CMAKE_EXECUTABLE_SUFFIX ".html")

	# IUP executables will always need this switch because IUP is event driven and doesn't quit when Emscripten tries to run/quit main().
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s NO_EXIT_RUNTIME=1")

ENDIF()

IF(WANTS_BUILD_SHARED_LIBRARY)
	IF("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
#		SET(MPG123LIB_C_FLAGS "${MPG123LIB_C_FLAGS} -fvisibility=hidden")
#		ADD_DEFINITIONS("-DMPG123_EXPORT=__attribute__ ((visibility(\"default\")))" )
	ELSEIF("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
#		SET(MPG123LIB_C_FLAGS "${MPG123LIB_C_FLAGS} -fvisibility=hidden")
#		ADD_DEFINITIONS("-DMPG123_EXPORT=__attribute__ ((visibility(\"default\")))" )
	ELSEIF(MSVC)
		SET(MPG123LIB_C_FLAGS "${MPG123LIB_C_FLAGS} -DBUILD_MPG123_DLL")

	ENDIF()


ENDIF()

IF(MSVC)
	# Kind of a hack. Need to get Windows exe's to link and not show a console window. Not sure why it is defaulting to /SUBSYSTEM:CONSOLE even though WIN32 is specified in the ADD_EXECUTABLE
	# SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
ENDIF()




SET(MPG123LIB_PUBLIC_HEADERS
	${CMAKE_SOURCE_DIR}/ports/Android/mpg123.h
	${CMAKE_SOURCE_DIR}/src/libmpg123/mpg123.h.in
	${CMAKE_SOURCE_DIR}/src/libmpg123/fmt123.h
)

SET(MPG123LIB_COMMON_SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/src/libmpg123/libmpg123.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/frame.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/id3.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/format.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/stringbuf.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/icy.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/icy2utf8.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/index.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/layer1.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/layer2.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/layer3.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/parse.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/optimize.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_8bit.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/ntom.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/dct64.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/equalizer.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/tabinit.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/feature.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/readers.c

	${CMAKE_SOURCE_DIR}/src/compat/compat.c
	${CMAKE_SOURCE_DIR}/src/compat/compat_str.c


)

SET(MPG123LIB_LINK_LIBRARIES "")
SET(MPG123LIB_SOURCE_FILES ${MPG123LIB_COMMON_SOURCE_FILES})
SET(MPG123LIB_FOREIGN_SOURCE_FILES "")

IF(APPLE)
ELSEIF(ANDROID)
ELSE()
ENDIF()

# GOTCHA: CFLAGS don't get passed to the ASM files. We need to use DEFINTIONS instead.


SET(MPGLIB123_CFLAGS_FOR_PURE_C_FALLBACK "-Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -ffast-math -O3")
SET(MPGLIB123_DEFINITIONS_FOR_PURE_C_FALLBACK "-DACCURATE_ROUNDING -DOPT_GENERIC")
SET(MPG123_PURE_C_FALLBACK_SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_real.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_s32.c
#	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_8bit.c
)

SET(MPGLIB123_CFLAGS_FOR_ARMV5 "-mcpu=cortex-m3 -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -ffast-math -O3")
SET(MPGLIB123_DEFINITIONS_FOR_ARMV5 "-DACCURATE_ROUNDING -DOPT_ARM -DREAL_IS_FIXED -DNO_REAL -DNO_32BIT -DHAVE_STRERROR -DASMALIGN_BYTE")
SET(MPG123_ARMV5_SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_arm.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_arm_accurate.S
)

SET(MPGLIB123_CFLAGS_FOR_ARMV7 "-Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -ffast-math -O3")
SET(MPGLIB123_DEFINITIONS_FOR_ARMV7 "-DACCURATE_ROUNDING -DOPT_ARM -DREAL_IS_FIXED -DNO_REAL -DNO_32BIT -DHAVE_STRERROR -DASMALIGN_BYTE")
#SET(MPGLIB123_DEFINITIONS_FOR_ARMV7 "-DOPT_ARM")
SET(MPG123_ARMV7_SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_arm.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_arm_accurate.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_arm_accurate.S
)
# TODO: NEON
#LOCAL_CFLAGS     := -DACCURATE_ROUNDING \
#					-DOPT_NEON \
#					-DHAVE_STRERROR \
#					-Wno-int-to-pointer-cast \
#					-Wno-pointer-to-int-cast \
#					-ffast-math -O3
#LOCAL_SRC_FILES +=  synth_real.c synth_s32.c
#LOCAL_SRC_FILES +=  synth_neon.S synth_neon_accurate.S synth_neon_float.S synth_neon_s32.S
#LOCAL_SRC_FILES +=  dct36_neon.S dct64_neon_float.S synth_stereo_neon_accurate.S synth_stereo_neon_float.S synth_stereo_neon_s32.S

#   /Library/Frameworks/Android/android-sdk/ndk-bundle/toolchains/x86-4.9/prebuilt/darwin-x86_64/lib/gcc/i686-linux-android/4.9.x/../../../../i686-linux-android/bin/ld: warning: shared library text segment is not shareable
#    /Library/Frameworks/Android/android-sdk/ndk-bundle/toolchains/x86-4.9/prebuilt/darwin-x86_64/lib/gcc/i686-linux-android/4.9.x/../../../../i686-linux-android/bin/ld: error: treating warnings as errors
#    clang: error: linker command failed with exit code 1 (use -v to see invocation)
# Update: Pushing on with this error results in a Error: dlopen failed [because] libmpg123.so has text relocations
# It appears Android 6.0/Marshmellow/API 23 rejects text relocations so we can't use this.
SET(MPGLIB123_CFLAGS_FOR_X86 "-Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -ffast-math -O3 -fPIC")
SET(MPGLIB123_DEFINITIONS_FOR_X86 "-DACCURATE_ROUNDING -DHAVE_STRERROR -DOPT_SSE=1 -DPIC -D__ELF__")
SET(MPG123_X86_SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_real.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_s32.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_sse.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_sse_accurate.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_sse_float.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_sse_s32.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_stereo_sse_accurate.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_stereo_sse_float.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_stereo_sse_s32.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/dct64_i386.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/dct36_sse.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/dct64_sse.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/dct64_sse_float.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/tabinit_mmx.S
)

SET(MPGLIB123_CFLAGS_FOR_AMD64 "-Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -ffast-math -O3")
SET(MPGLIB123_DEFINITIONS_FOR_AMD64 "-DACCURATE_ROUNDING -DHAVE_STRERROR -DOPT_X86_64")
SET(MPG123_AMD64_SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_real.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_s32.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/getcpuflags_x86_64.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_x86_64.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_x86_64_s32.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_x86_64_accurate.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_x86_64_float.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_stereo_x86_64_float.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_stereo_x86_64.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_stereo_x86_64_s32.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_stereo_x86_64_accurate.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/dct36_x86_64.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/dct64_x86_64.S
	${CMAKE_SOURCE_DIR}/src/libmpg123/dct64_x86_64_float.S
)

SET(MPGLIB123_CFLAGS_FOR_ARM64 "-Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -ffast-math -O3")
SET(MPGLIB123_DEFINITIONS_FOR_ARM64 "-DACCURATE_ROUNDING -DHAVE_STRERROR -DOPT_NEON64")
SET(MPG123_ARM64_SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_real.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/synth_s32.c
	${CMAKE_SOURCE_DIR}/src/libmpg123/dct36_neon64.S
    ${CMAKE_SOURCE_DIR}/src/libmpg123/dct64_neon64.S
    ${CMAKE_SOURCE_DIR}/src/libmpg123/dct64_neon64_float.S
    ${CMAKE_SOURCE_DIR}/src/libmpg123/synth_neon64.S
    ${CMAKE_SOURCE_DIR}/src/libmpg123/synth_neon64_accurate.S
    ${CMAKE_SOURCE_DIR}/src/libmpg123/synth_neon64_float.S
    ${CMAKE_SOURCE_DIR}/src/libmpg123/synth_neon64_s32.S
    ${CMAKE_SOURCE_DIR}/src/libmpg123/synth_stereo_neon64.S
    ${CMAKE_SOURCE_DIR}/src/libmpg123/synth_stereo_neon64_accurate.S
    ${CMAKE_SOURCE_DIR}/src/libmpg123/synth_stereo_neon64_float.S
    ${CMAKE_SOURCE_DIR}/src/libmpg123/synth_stereo_neon64_s32.S
)

# TODO: Figure out how to set the architecture for other platforms
SET(MPGLIB123_USE_ARMV5 0)
SET(MPGLIB123_USE_ARMV7 0)
SET(MPGLIB123_USE_X86 0)
SET(MPGLIB123_USE_AMD64 0)
SET(MPGLIB123_USE_ARM64 0)
SET(MPGLIB123_USE_PURE_C_FALLBACK 0)

MESSAGE("ANDROID ${ANDROID}")
MESSAGE("ANDROID_ABI ${ANDROID_ABI}")
IF(ANDROID)
	IF("${ANDROID_ABI}" STREQUAL "armeabi")
		SET(MPGLIB123_USE_ARMV5 1)
	ELSEIF("${ANDROID_ABI}" STREQUAL "armeabi-v7a")
		SET(MPGLIB123_USE_ARMV7 1)
	ELSEIF("${ANDROID_ABI}" STREQUAL "armeabi-v7a with NEON")
		# TODO: have dedicated Neon option
		SET(MPGLIB123_USE_ARMV7 1)
	ELSEIF("${ANDROID_ABI}" STREQUAL "x86")
		# Unfortunately, can't get x86 asm to work on Android.
#		SET(MPGLIB123_USE_X86 1)
		SET(MPGLIB123_USE_PURE_C_FALLBACK 1)

	ELSEIF("${ANDROID_ABI}" STREQUAL "x86_64")
		SET(MPGLIB123_USE_AMD64 1)
	ELSEIF("${ANDROID_ABI}" STREQUAL "arm64-v8a")
		SET(MPGLIB123_USE_ARM64 1)
	ELSE()
		MESSAGE(FATAL_ERROR "Unknown architecture")
	ENDIF()
ELSE()

	MESSAGE("Warning: Always picking amd64 architecure")
	SET(MPGLIB123_USE_AMD64 1)

ENDIF()



IF(MPGLIB123_USE_ARMV5)
	LIST(APPEND MPG123LIB_SOURCE_FILES ${MPG123_ARMV5_SOURCE_FILES})
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MPGLIB123_CFLAGS_FOR_ARMV5}")
#	ADD_DEFINITIONS(${MPGLIB123_DEFINITIONS_FOR_ARMV5})


ELSEIF(MPGLIB123_USE_ARMV7)
	LIST(APPEND MPG123LIB_SOURCE_FILES ${MPG123_ARMV7_SOURCE_FILES})
#	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MPGLIB123_CFLAGS_FOR_ARMV7}")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MPGLIB123_CFLAGS_FOR_ARMV7} ${MPGLIB123_DEFINITIONS_FOR_ARMV7}")
#	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MPGLIB123_DEFINITIONS_FOR_ARMV7}")
#	ADD_DEFINITIONS(${MPGLIB123_DEFINITIONS_FOR_ARMV7})

ELSEIF(MPGLIB123_USE_X86)
	LIST(APPEND MPG123LIB_SOURCE_FILES ${MPG123_X86_SOURCE_FILES})
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MPGLIB123_CFLAGS_FOR_X86}")
	ADD_DEFINITIONS(${MPGLIB123_DEFINITIONS_FOR_X86})

	# This doesn't seem to work.
	# link_directories(/Library/Frameworks/Android/android-sdk/ndk-bundle/platforms/android-16/arch-x86/usr/lib)

	# This works around 2 bugs:
	# Bug 1: We are getting a linker warning. Android treats it as fatal, but we want to proceed.
	# Bug 2: For x86, the NDK is failing to find crtbegin_so.o crtend_so.o libm.so libdl.so libc.so
	# According to the net copying the files (or symlinking) into the source directory is the workaround.
	# But I found that I need to put crtbegin_so.o crtend_so.o in the source directory,
	# but the .so files didn't work. So putting an explicit path to the link directory seemed to work.
	# Using link_directories() didn't work.
	# The "source" directory is tricky with Android/CMake's out-of-source-builds.
	# For debug, I was in mpg123/Android/mpg123player/mpg123/.externalNativeBuild/cmake/debug/x86
	# (which is above CMakeFiles/mpg123.dir/src/libmpg123 where the .o files are)
	# So in mpg123/Android/mpg123player/mpg123/.externalNativeBuild/cmake/debug/x86,
	# ln -s /Library/Frameworks/Android/android-sdk/ndk-bundle/platforms/android-16/arch-x86/usr/lib/crtbegin_so.o crtbegin_so.o
	# ln -s /Library/Frameworks/Android/android-sdk/ndk-bundle/platforms/android-16/arch-x86/usr/lib/crtend_so.o crtend_so.o
	# For Release:
	# cd to mpg123/Android/mpg123player/mpg123/.externalNativeBuild/cmake/release/x86,
	# ln -s /Library/Frameworks/Android/android-sdk/ndk-bundle/platforms/android-16/arch-x86/usr/lib/crtbegin_so.o crtbegin_so.o
	# ln -s /Library/Frameworks/Android/android-sdk/ndk-bundle/platforms/android-16/arch-x86/usr/lib/crtend_so.o crtend_so.o
	#
	set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-fatal-warnings -L/Library/Frameworks/Android/android-sdk/ndk-bundle/platforms/android-16/arch-x86/usr/lib")




MESSAGE("CMAKE_C_FLAGS are ${CMAKE_C_FLAGS}")

ELSEIF(MPGLIB123_USE_AMD64)
	LIST(APPEND MPG123LIB_SOURCE_FILES ${MPG123_AMD64_SOURCE_FILES})
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MPGLIB123_CFLAGS_FOR_AMD64}")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPGLIB123_CFLAGS_FOR_AMD64}")
	ADD_DEFINITIONS(${MPGLIB123_DEFINITIONS_FOR_AMD64})

ELSEIF(MPGLIB123_USE_ARM64)
	LIST(APPEND MPG123LIB_SOURCE_FILES ${MPG123_ARM64_SOURCE_FILES})
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MPGLIB123_CFLAGS_FOR_ARM64}")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPGLIB123_CFLAGS_FOR_ARM64}")
	ADD_DEFINITIONS(${MPGLIB123_DEFINITIONS_FOR_ARM64})

ELSEIF(MPGLIB123_USE_PURE_C_FALLBACK)
	LIST(APPEND MPG123LIB_SOURCE_FILES ${MPG123_PURE_C_FALLBACK_SOURCE_FILES})
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MPGLIB123_CFLAGS_FOR_PURE_C_FALLBACK}")
	ADD_DEFINITIONS(${MPGLIB123_DEFINITIONS_FOR_PURE_C_FALLBACK})


ELSE()

	# TODO: I think there are pure C files, but I don't feel like looking them up right now.
	MESSAGE(FATAL_ERROR "Unknown architecture")


ENDIF()

# Would prefer TARGET_INCLUDE_DIRECTORIES or something, but supporting CMake 2.8.7
INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/ports/Android
	${CMAKE_SOURCE_DIR}/src/libmpg123
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/compat
)


HELPER_CREATE_LIBRARY(mpg123 "${WANTS_BUILD_SHARED_LIBRARY}" "${WANTS_BUILD_FRAMEWORK}" "${MPG123LIB_SOURCE_FILES}" "${MPG123LIB_PUBLIC_HEADERS}" "" "" "${MPG123LIB_FOREIGN_SOURCE_FILES}" "${MPG123LIB_LINK_LIBRARIES}" "${MPG123LIB_C_FLAGS}" "de.mpg123.mpg123")



# For uninstall
HELPER_SETUP_UNINSTALL_TARGET()





