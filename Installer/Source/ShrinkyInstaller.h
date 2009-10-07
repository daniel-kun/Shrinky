#ifndef SHRINKYINSTALLER_H
#define SHRINKYINSTALLER_H

#include <QFrame>
#include <QPushButton>
#include <QLabel>

class ShrinkyInstaller: public QFrame
{
public:
    ShrinkyInstaller();

private:
    QLabel m_lLogo, m_lSettings;
    QPushButton m_btnInstall;
};

#endif
