#include "ChoiceWin.h"
#include "Shrink.h"

#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   ShrinkChoiceWin win("E:\\Temp\\AudiS5Cabrio.png");
   win.show();
   return app.exec();
}
