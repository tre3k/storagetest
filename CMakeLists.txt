#
# Copyright (c) 2025 Kirill Pshenichnyi <pshcyrill@mail.ru>
#
# This file is part of storagetest.
#
# storagetest is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# storagetest is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with storagetest.  If not, see <https://www.gnu.org/licenses/>.
#
#    Author: Kirill Pshenichnyi <pshcyrill@mail.ru>
#

cmake_minimum_required(VERSION 3.16)

set(PROGRAM_NAME storagetest)
project(${PROGRAM_NAME})
set(SOURCES_SUBDIR src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(INCLUDES
  ${PROJECT_SOURCE_DIR}/${SOURCES_SUBDIR}
  )
set(LIBS_PATH
  ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
  )

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_library(SSL_LIB ssl)
find_library(CRYPTO_LIB crypto)
find_library(PTHREAD_LIB pthread)

include_directories(${INCLUDES})
link_directories(${LIBS_PATH})

execute_process(
  COMMAND "git" "describe" "--always" "--tags"
  OUTPUT_VARIABLE VERSION_GIT_OUTPUT
  )

string(TIMESTAMP TODAY "%d.%m.%Y")
string(TIMESTAMP COPY_YEAR "%Y")

string(REGEX REPLACE "\n$" "" VERSION_GIT_OUTPUT "${VERSION_GIT_OUTPUT}")
set(APPLICATION_VERSION \"${VERSION_GIT_OUTPUT}\")
set(APPLICATION_NAME "\"storagetest\"")

set(TODAY_TEXT \"${TODAY}\")
set(COPY_YEAR_TEXT \"${COPY_YEAR}\")
set(CMAKE_C_COMPILER_VERSION_TEXT \"${CMAKE_C_COMPILER_VERSION}\")
set(CMAKE_C_COMPILER_ID_TEXT \"${CMAKE_C_COMPILER_ID}\")

if(NOT DEFINED USE_SHA512)
  set(USE_SHA512 0)
endif(NOT DEFINED USE_SHA512)

configure_file(config.h.in ${PROJECT_SOURCE_DIR}/${SOURCES_SUBDIR}/config.h)

add_library(${PROGRAM_NAME} STATIC
  ${SOURCES_SUBDIR}/storagetest.c
  ${SOURCES_SUBDIR}/fileinformation.c
  ${SOURCES_SUBDIR}/status.c
  )

target_link_libraries(${PROGRAM_NAME}
  ${SSL_LIB}
  ${CRYPTO_LIB}
  ${PTHREAD_LIB}
  )

add_executable(${PROGRAM_NAME}cli
  ${SOURCES_SUBDIR}/main.c
  )

target_link_libraries(${PROGRAM_NAME}cli
  ${SSL_LIB}
  ${CRYPTO_LIB}
  ${PTHREAD_LIB}
  storagetest
  )

add_subdirectory(${PROJECT_SOURCE_DIR}/${SOURCES_SUBDIR}/tests)
