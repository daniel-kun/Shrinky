#include "ShrinkyInstaller.h"

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QBitmap>

ShrinkyInstaller::ShrinkyInstaller():
   m_lLogo(this),
   m_lSettings(this),
   m_btnInstall(this)
{
   setWindowTitle(tr("Install Shrinky"));
   setWindowIcon(QIcon(":/ShrinkyLogo-256.png"));
   setObjectName("ShrinkyInstaller");
   setStyleSheet("QWidget#ShrinkyInstaller{ background-image: url(:/Background.png); }");
   resize(600, 400);
   QPixmap pix(":/Shrinky.png");
   m_lLogo.move((width() - pix.width()) / 2, 70);
   m_lLogo.setPixmap(pix);

   QFont font = m_btnInstall.font();
//   font.setBold(true);
   font.setPointSizeF(font.pointSizeF() * 1.5);
   font.setStretch(130);
   m_btnInstall.setFont(font);
   m_btnInstall.setText(tr("Install now!"));
   m_btnInstall.adjustSize();
   m_btnInstall.resize(
      static_cast<int>(m_btnInstall.width() * 1.2),
      static_cast<int>(m_btnInstall.height() * 1.2));
   m_btnInstall.move(
      (width() - m_btnInstall.width()) / 2,
      300);

   m_lSettings.setText("<a href=\"settings\">" + tr("Change settings") + "</a>");
   m_lSettings.move(7, height() - 25);
}

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   ShrinkyInstaller installer;
   installer.show();
   return app.exec();
}
