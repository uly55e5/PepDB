set( UNIPROTINPUT_HDRS
     uniprotinputplugin.h
     uniprotxmlreader.h )

set( UNIPROTINPUT_SRCS
     uniprotinputplugin.cpp
     uniprotxmlreader.cpp )

qt4_wrap_cpp( UNIPROTINPUT_MOC_SRCS ${UNIPROTINPUT_HDRS} )

add_library( uniprotinput SHARED ${UNIPROTINPUT_SRCS} ${UNIPROTINPUT_MOC_SRCS} )

target_link_libraries( uniprotinput ${QT_LIBRARIES} pepdb)
