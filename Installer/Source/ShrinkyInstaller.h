#ifndef SHRINKYINSTALLER_H
#define SHRINKYINSTALLER_H

#include "InstallSettings.h"

#include <QFrame>
#include <QPushButton>
#include <QLabel>

class ShrinkyInstaller: public QFrame
{
   Q_OBJECT

public:
    ShrinkyInstaller();

private:
    QLabel m_lLogo, m_lSubText, m_lSettings;
    QPushButton m_btnInstall;

    InstallSettings m_settings;

    virtual void retranslateUi();

private slots:
    void showSettings();
};

#endif
