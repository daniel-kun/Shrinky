#ifndef SHRINKYINSTALLER_H
#define SHRINKYINSTALLER_H

#include "InstallSettings.h"

#include <QFrame>
#include <QImage>
#include <QLabel>
#include <QPushButton>

class ShrinkyInstaller: public QWidget
{
   Q_OBJECT

public:
    ShrinkyInstaller();

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
   QImage m_imgBackground;

    QLabel m_lLogo, m_lSubText, m_lSettings;
    QPushButton m_btnInstall;

    InstallSettings m_settings;

    virtual void retranslateUi();

private slots:
    void showSettings();
};

#endif
