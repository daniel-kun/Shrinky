#include "ChoiceWin.h"
#include "Shrink.h"

#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   QString fileName;
   if (argc == 2)
      fileName = argv[1];
   ShrinkChoiceWin win(fileName);
   win.show();
   return app.exec();
}
