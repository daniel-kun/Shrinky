TEMPLATE=app

CONFIG += debug_and_release

TRANSLATIONS += ../Resources/Shrinky_de.ts

RESOURCES += ../Resources/Shrinky.qrc

win32:RC_FILE += ../Resources/Shrinky.rc

HEADERS += \
   ../Source/Shrink.h \
   ../Source/ChoiceWin.h \

SOURCES += \
   ../Source/Shrink.cpp \
   ../Source/ChoiceWin.cpp \
   ../Source/Main.cpp
