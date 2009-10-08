#include "InstallSettings.h"
#include "InstallUtils.h"

#include <QFileDialog>

InstallSettings::InstallSettings(QWidget *parent):
   QWidget(parent),
   m_glSettings(this)
{
   m_glSettings.addWidget(&m_lDestDir, 0, 0);
   m_glSettings.addWidget(&m_edDestDir, 0, 1);
   m_glSettings.addWidget(&m_btnDestDir, 0, 2);
   m_glSettings.addWidget(&m_cbShellExtension, 1, 1);
   m_lDestDir.setBuddy(&m_edDestDir);

   m_edDestDir.setText(defaultInstallPath());
   m_cbShellExtension.setChecked(true);

   retranslateUi();

   connect(
      &m_btnDestDir,
      SIGNAL(clicked()),
      this,
      SLOT(browseForDestDir()));
}

void InstallSettings::retranslateUi()
{
   m_lDestDir.setText(tr("&Install to:"));
   m_btnDestDir.setText("...");
   m_cbShellExtension.setText(tr("Install extension to the Windows Explorer"));
}

void InstallSettings::browseForDestDir()
{
   QString dest = QFileDialog::getExistingDirectory(
      this,
      tr("Pick a directory to install to"),
      m_edDestDir.text());
   if (!dest.isEmpty())
      m_edDestDir.setText(QDir::toNativeSeparators(dest));
}
