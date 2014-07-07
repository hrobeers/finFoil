TEMPLATE = subdirs

CONFIG( no_app ) {
    message( "App disabled" )
} else{
    message( "App enabled" )
    SUBDIRS += src/app
}

CONFIG( deps ) {
    message( "Dependency build enabled" )
    SUBDIRS += jenson \
               qtunits

    jenson.file = submodules/jenson/jenson.pro
    qtunits.file = submodules/qtunits/qtunits.pro
} else{
    message( "Dependency build disabled" )
}

CONFIG( unittests ) {
    message( "Tests enabled" )
    SUBDIRS += src/unittest
} else{
    message( "Tests disabled" )
}
