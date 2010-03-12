set( PEPDBLIB_HDRS
     job.h
     pluginloader.h
     project.h
     proteindatabase.h
     featuretablemodel.h )

set( PEPDBLIB_SRCS
     job.cpp
     pluginloader.cpp
     project.cpp
     proteindatabase.cpp
     featuretablemodel.cpp
     ../common/exception.cpp )

qt4_wrap_cpp( PEPDBLIB_MOC_SRCS ${PEPDBLIB_HDRS} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_library( pepdb SHARED ${PEPDBLIB_SRCS} ${PEPDBLIB_MOC_SRCS} )

target_link_libraries( pepdb ${QT_LIBRARIES} )
