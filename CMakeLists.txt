cmake_minimum_required(VERSION 3.12)

project(cocot VERSION 1.2 LANGUAGES C)
set(COCOT_VERSION "1.2-20171118")

include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(GNUInstallDirs)

set(DEFAULT_TERM_CODE "UTF-8" CACHE STRING "default value for term code")
set(DEFAULT_PROC_CODE "CP932" CACHE STRING "default value for proc code")

# iconv
find_package(Iconv REQUIRED)
set(HAVE_ICONV_H 1)
set(CMAKE_REQUIRED_LIBRARIES Iconv::Iconv)

# openpty / login_tty (libc or libutil)
set(COCOT_UTIL_LIBRARY "")
check_function_exists(openpty HAVE_OPENPTY_LIBC)
if(HAVE_OPENPTY_LIBC)
  set(HAVE_LIBUTIL 1)
else()
  check_library_exists(util openpty "" HAVE_OPENPTY_LIBUTIL)
  if(HAVE_OPENPTY_LIBUTIL)
    set(HAVE_LIBUTIL 1)
    set(COCOT_UTIL_LIBRARY util)
  elseif(EXISTS /dev/ptmx)
    check_include_file(stropts.h HAVE_STROPTS_H)
  endif()
endif()
if(COCOT_UTIL_LIBRARY)
  list(APPEND CMAKE_REQUIRED_LIBRARIES ${COCOT_UTIL_LIBRARY})
endif()

# headers
foreach(header
    stdlib.h string.h sys/ioctl.h sys/time.h sys/types.h sys/wait.h
    termios.h unistd.h utmp.h libutil.h util.h pty.h signal.h)
  string(TOUPPER "HAVE_${header}" var)
  string(REGEX REPLACE "[/.]" "_" var "${var}")
  check_include_file(${header} ${var})
endforeach()

# functions
foreach(func memcpy memmove memset select strerror cfmakeraw login_tty)
  string(TOUPPER "HAVE_${func}" var)
  check_function_exists(${func} ${var})
endforeach()
check_symbol_exists(max "stdlib.h" HAVE_MAX)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_executable(cocot
  ccio.c
  cocot.c
  init.c
  l10n_ja.c
  l10n_cjk_uni.c
  l10n_cjk_uni_table.c
  loop.c
  sigwinch.c
  suspend.c)
target_compile_definitions(cocot PRIVATE HAVE_CONFIG_H=1)
target_include_directories(cocot PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cocot PRIVATE Iconv::Iconv ${COCOT_UTIL_LIBRARY})

install(TARGETS cocot RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
