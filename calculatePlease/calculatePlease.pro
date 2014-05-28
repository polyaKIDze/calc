QT += \
    network\
    webkitwidgets\

QMAKE_CXXFLAGS += -static

HEADERS += \
    calculate.h \
    config.h

FORMS += \
    calculate.ui \
    confDialog.ui

SOURCES += \
    calculate.cpp \
    main.cpp
