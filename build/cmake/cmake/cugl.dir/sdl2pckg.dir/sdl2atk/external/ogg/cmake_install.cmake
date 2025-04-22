# Install script for directory: /users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/sdl2atk/external/ogg

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/users/patrickchoo/cs4152/mahsjong/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/libogg.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libogg.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libogg.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libogg.a")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ogg" TYPE FILE FILES
    "/users/patrickchoo/cs4152/mahsjong/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/include/ogg/config_types.h"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/include/ogg/ogg.h"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/include/ogg/os_types.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg/OggTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg/OggTargets.cmake"
         "/users/patrickchoo/cs4152/mahsjong/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/CMakeFiles/Export/dee6fd410a50d06b294b496f57355584/OggTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg/OggTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg/OggTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg" TYPE FILE FILES "/users/patrickchoo/cs4152/mahsjong/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/CMakeFiles/Export/dee6fd410a50d06b294b496f57355584/OggTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg" TYPE FILE FILES "/users/patrickchoo/cs4152/mahsjong/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/CMakeFiles/Export/dee6fd410a50d06b294b496f57355584/OggTargets-noconfig.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg" TYPE FILE FILES
    "/users/patrickchoo/cs4152/mahsjong/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/OggConfig.cmake"
    "/users/patrickchoo/cs4152/mahsjong/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/OggConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/users/patrickchoo/cs4152/mahsjong/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/ogg.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/SDL2_atk/html" TYPE FILE FILES
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/framing.html"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/index.html"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/oggstream.html"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/ogg-multiplex.html"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/fish_xiph_org.png"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/multiplex1.png"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/packets.png"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/pages.png"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/stream.png"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/vorbisword2.png"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/white-ogg.png"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/white-xifish.png"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/rfc3533.txt"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/rfc5334.txt"
    "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/skeleton.html"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/SDL2_atk/html" TYPE DIRECTORY FILES "/users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/libogg")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/users/patrickchoo/cs4152/mahsjong/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
