set( PEPDB_HDRS
     jobstatuswidget.h
     mainwindow.h
     newprojectdialog.h
     resulttable.h
     settingsdialog.h
     searchwidget.h )

set( PEPDB_SRCS
     jobstatuswidget.cpp
     main.cpp
     mainwindow.cpp
     newprojectdialog.cpp
     resulttable.cpp
     settingsdialog.cpp
     searchwidget.cpp )

set( PEPDB_UIS
     forms/jobstatuswidget.ui
     forms/mainwindow.ui
     forms/newprojectdialog.ui
     forms/resulttable.ui
     forms/settingsdialog.ui
     forms/searchwidget.ui )

qt4_wrap_cpp( PEPDB_MOC_SRCS ${PEPDB_HDRS} )

qt4_wrap_ui( PEPDB_UI_HDRS ${PEPDB_UIS} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_executable( pepdbapp ${PEPDB_SRCS} ${PEPDB_MOC_SRCS} ${PEPDB_UI_HDRS} )

target_link_libraries( pepdbapp ${QT_LIBRARIES} pepdb )
