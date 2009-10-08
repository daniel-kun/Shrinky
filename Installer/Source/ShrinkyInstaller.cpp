#include "ShrinkyInstaller.h"

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QBitmap>

ShrinkyInstaller::ShrinkyInstaller():
   m_lLogo(this),
   m_lSubText(this),
   m_lSettings(this),
   m_btnInstall(this),
   m_settings(this)
{
   setWindowIcon(QIcon(":/ShrinkyLogo-256.png"));
   setObjectName("ShrinkyInstaller");
   setStyleSheet("QWidget#ShrinkyInstaller{ background-image: url(:/Background.png); }");
   resize(600, 400);
   QPixmap pix(":/Shrinky.png");
   m_lLogo.move((width() - pix.width()) / 2, 70);
   m_lLogo.setPixmap(pix);

   QFont font = m_btnInstall.font();
   font.setPointSizeF(font.pointSizeF() * 1.5);
   font.setStretch(130);
   m_btnInstall.setFont(font);

   m_lSettings.move(7, height() - 25);
   m_lSettings.setTextInteractionFlags(Qt::TextBrowserInteraction);
   m_settings.setVisible(false);

   connect(
      &m_lSettings,
      SIGNAL(linkActivated(QString)),
      this,
      SLOT(showSettings()));

   retranslateUi();
}

void ShrinkyInstaller::retranslateUi()
{
   setWindowTitle(tr("Install Shrinky"));
   m_btnInstall.setText(tr("Install now!"));
   m_btnInstall.adjustSize();
   m_btnInstall.resize(
      static_cast<int>(m_btnInstall.width() * 1.2),
      static_cast<int>(m_btnInstall.height() * 1.2));
   m_btnInstall.move(
      (width() - m_btnInstall.width()) / 2,
      300);
   QString templ = "<a href=\"settings\">%1</a>";
   if (m_settings.isVisible())
      m_lSettings.setText(templ.arg(tr("Hide settings")));
   else
      m_lSettings.setText(templ.arg(tr("Show settings")));
   m_settings.retranslateUi();
}

void ShrinkyInstaller::showSettings()
{
   if (!m_settings.isVisible())
   {
      int w = m_lSettings.x() + m_lSettings.width() + 7;
      int t = m_btnInstall.y() + m_btnInstall.height() + 2;
      int h = (height() - t - 2);
      m_settings.setGeometry(
         w,
         t,
         width() - w * 2,
         h);
   }
   m_settings.setVisible(!m_settings.isVisible());
   retranslateUi();
}

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   ShrinkyInstaller installer;
   installer.show();
   return app.exec();
}
