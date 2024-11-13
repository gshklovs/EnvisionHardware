# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/grisha/esp/v5.3.1/esp-idf/components/bootloader/subproject"
  "/Users/grisha/Documents/EmbeddedProjects/camera_example/build/bootloader"
  "/Users/grisha/Documents/EmbeddedProjects/camera_example/build/bootloader-prefix"
  "/Users/grisha/Documents/EmbeddedProjects/camera_example/build/bootloader-prefix/tmp"
  "/Users/grisha/Documents/EmbeddedProjects/camera_example/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/grisha/Documents/EmbeddedProjects/camera_example/build/bootloader-prefix/src"
  "/Users/grisha/Documents/EmbeddedProjects/camera_example/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/grisha/Documents/EmbeddedProjects/camera_example/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/grisha/Documents/EmbeddedProjects/camera_example/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
