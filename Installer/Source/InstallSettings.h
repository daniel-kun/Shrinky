#ifndef INSTALLSETTINGS_H
#define INSTALLSETTINGS_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QBoxLayout>

class InstallSettings : public QWidget
{
   Q_OBJECT

public:
    InstallSettings(QWidget *parent = 0);

    virtual void retranslateUi();

private:
    QGridLayout m_glSettings;
    QLabel m_lDestDir;
    QLineEdit m_edDestDir;
    QToolButton m_btnDestDir;

    QCheckBox m_cbShellExtension;

private slots:
    void browseForDestDir();
};

#endif // INSTALLSETTINGS_H
