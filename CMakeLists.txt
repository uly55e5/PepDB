project( PepDB CXX )

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")

cmake_minimum_required(VERSION 2.6.0)

find_package(Qt4 4.6.0  COMPONENTS  QtCore QtGui QtWebkit QtSql QtXml QtXmlPatterns REQUIRED)

include(${QT_USE_FILE})

add_subdirectory( src )