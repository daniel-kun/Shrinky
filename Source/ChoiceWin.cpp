#include "ChoiceWin.h"

#include "Shrink.h"

#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QCloseEvent>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QDesktopServices>

void ShrinkWorker::shrink(const ShrinkParams &aParams)
{
   m_params = aParams;
   start();
}

QString ShrinkWorker::lastError() const
{
   return m_lastError;
}

void ShrinkWorker::run()
{
   if (m_params.mode == smPixels)
   {
      ShrinkError error = shrinkImage(
            m_params.source,
            m_params.dest,
            QSize(m_params.width, m_params.height));
      if (!error)
      {
         m_lastError = error.message;
      }
   }
   else
   {
      shrinkImageToFileSize(
         m_params.source,
         m_params.dest,
         m_params.size);
   }
}

ClickLabel::ClickLabel(QWidget *parent):
   QLabel(parent)
{
}

void ClickLabel::mousePressEvent(QMouseEvent *event)
{
   QLabel::mousePressEvent(event);
   if (event->buttons() & Qt::LeftButton)
   {
      emit clicked();
   }
}

namespace
{
   bool parseFileSize(const QString &aText, quint64 &aSize)
   {
      QString text(aText.toUpper());
      QMap<QString, int> units;
      units["KB"] = 1024;
      units["MB"] = 1024 * 1024;
      units["GB"] = 1024 * 1024 * 1024;
      quint64 factor = 1;
      for (QMap<QString, int>::const_iterator i = units.begin();
         i != units.end();
         ++i)
      {
         int idx = text.indexOf(i.key());
         if (idx >= 0)
         {
            // Found unit. Remove it from text
            // and set the correct factor.
            text.remove(idx, i.key().size());
            factor = i.value();
            break;
         }
      }
      if (factor == 1 )
      {
         for (QMap<QString, int>::const_iterator i = units.begin();
            i != units.end();
            ++i)
         {
            int idx = text.indexOf(i.key()[0]);
            if (idx >= 0)
            {
               // Found unit. Remove it from text
               // and set the correct factor.
               text = text.remove(idx, 1);
               factor = i.value();
               break;
            }
         }
      }
      if (factor == 1)
         factor = 1024;
      // Remove every whitespace
      text.replace(QRegExp("\\s"), "");
      if (text.indexOf(QRegExp("[^0123456789]")) >= 0)
      {
         return false;
      }
      aSize = static_cast<quint64>(text.toDouble() * factor);
      return true;
   }
}

ShrinkChoiceWin::ShrinkChoiceWin(const QString &aImage, QWidget *aParent):
   QWidget(aParent),
   m_pxBackground(":/BackgroundGradient.png"),
   m_vlMain(this),
   m_vlPreview(&m_frPreview),
   m_vlSettings(&m_gbSettings),
   m_glShrinkFileSize(&m_wShrinkFileSize),
   m_glShrinkPixels(&m_wShrinkPixels),
   m_glShrinkPercent(&m_wShrinkPercent),
   m_valPixels(0),
   m_valPercent(0),
   m_askedForOverwrite(false)
{
   setWindowIcon(QPixmap(":/ShrinkyLogo-256.png"));
   QPalette pal(palette());
   QLinearGradient gradient(
         QPointF(width() / 2, height()),
         QPointF(width() / 2, height() - 350));
   gradient.setColorAt(0, QColor(145, 210, 255, 255));
   gradient.setColorAt(1, Qt::white);
   pal.setBrush(QPalette::Window, QBrush(gradient));
   setPalette(pal);
   setMinimumWidth(550);
   m_vlMain.addLayout(&m_hlPreviewFrame);
   m_vlMain.addWidget(&m_btnJustShrink);
   m_vlMain.addWidget(&m_btnCustomShrink);
   m_vlMain.addWidget(&m_gbSettings);
   m_vlMain.addLayout(&m_hlBottom);
   m_vlMain.addStretch(1);

   m_hlPreviewFrame.addStretch(1);
   m_hlPreviewFrame.addWidget(&m_frPreview);
   m_hlPreviewFrame.addStretch(1);

   m_frPreview.setFrameStyle(QFrame::StyledPanel);
   m_vlPreview.addWidget(&m_lPreview);
   m_vlPreview.addWidget(&m_lSourceFileName);
   m_lPreview.setCursor(Qt::PointingHandCursor);

   m_btnJustShrink.setDefault(true);

   m_btnCustomShrink.setCheckable(true);
   m_btnCustomShrink.setChecked(false);

   m_vlSettings.addWidget(&m_rbShrinkFileSize);
   m_vlSettings.addWidget(&m_wShrinkFileSize);
   m_vlSettings.addWidget(&m_rbShrinkPixels);
   m_vlSettings.addWidget(&m_wShrinkPixels);
   m_vlSettings.addWidget(&m_rbShrinkPercent);
   m_vlSettings.addWidget(&m_wShrinkPercent);
   m_vlSettings.addLayout(&m_hlDestFile);

   m_glShrinkFileSize.addWidget(&m_lFileSize, 0, 0);
   m_glShrinkFileSize.addWidget(&m_edFileSize, 0, 1);
   m_glShrinkFileSize.addWidget(&m_lFileSizeExamples, 1, 0, 1, 2);
   m_glShrinkFileSize.setColumnStretch(3, 1);

   m_glShrinkPixels.addWidget(&m_cbPixelsAspectRatio, 0, 0, 1, 6);
   m_glShrinkPixels.addWidget(&m_lPixelsWidth, 1, 0);
   m_glShrinkPixels.addWidget(&m_edPixelsWidth, 1, 1);
   m_glShrinkPixels.addWidget(&m_lPixelsHeight, 1, 2);
   m_glShrinkPixels.addWidget(&m_edPixelsHeight, 1, 3);
   m_glShrinkPixels.addWidget(&m_lPixels, 1, 4);
   m_glShrinkPixels.setColumnStretch(6, 1);

   m_edPixelsWidth.setMaximumWidth(40);
   m_edPixelsHeight.setMaximumWidth(40);

   m_glShrinkPercent.addWidget(&m_cbPercentAspectRatio, 0, 0, 1, 6);
   m_glShrinkPercent.addWidget(&m_lPercentWidth, 1, 0);
   m_glShrinkPercent.addWidget(&m_edPercentWidth, 1, 1);
   m_glShrinkPercent.addWidget(&m_lPercentHeight, 1, 2);
   m_glShrinkPercent.addWidget(&m_edPercentHeight, 1, 3);
   m_glShrinkPercent.addWidget(&m_lPercent, 1, 4);
   m_glShrinkPercent.setColumnStretch(6, 1);

   m_edPercentWidth.setMaximumWidth(40);
   m_edPercentHeight.setMaximumWidth(40);

   m_lFileSize.setBuddy(&m_edFileSize);
   m_lPixelsWidth.setBuddy(&m_edPixelsWidth);
   m_lPixelsHeight.setBuddy(&m_edPixelsHeight);
   m_lPercentWidth.setBuddy(&m_edPercentWidth);
   m_lPercentHeight.setBuddy(&m_edPercentHeight);

   m_hlDestFile.addWidget(&m_lDestFile);
   m_hlDestFile.addWidget(&m_edDestFile);
   m_hlDestFile.addWidget(&m_btnDestFile);

   m_lDestFile.setBuddy(&m_edDestFile);

   m_hlBottom.addWidget(&m_cbOpenAfterShrinking);
   m_hlBottom.addWidget(&m_btnLangEnglish);
   m_hlBottom.addWidget(&m_btnLangGerman);

   m_btnLangEnglish.setIcon(QPixmap(":/English.png"));
   m_btnLangGerman.setIcon(QPixmap(":/German.png"));

   m_btnLangEnglish.setAutoRaise(true);
   m_btnLangGerman.setAutoRaise(true);

   m_cbPercentAspectRatio.setChecked(true);
   m_cbPixelsAspectRatio.setChecked(true);

   /* default suggestions */
   m_edFileSize.setText("150 kb");

   m_edPercentWidth.setValidator(&m_valPercent);
   m_edPercentHeight.setValidator(&m_valPercent);
   m_edPixelsWidth.setValidator(&m_valPixels);
   m_edPixelsHeight.setValidator(&m_valPixels);
   m_valPixels.setBottom(1);
   m_valPercent.setRange(1, 1000000);

   connect(
      &m_btnCustomShrink,
      SIGNAL(toggled(bool)),
      this,
      SLOT(updateSettingsVisible()));

   connect(
      &m_rbShrinkFileSize,
      SIGNAL(toggled(bool)),
      this,
      SLOT(updateShrinkSettingWidgets()));
   connect(
      &m_rbShrinkPixels,
      SIGNAL(toggled(bool)),
      this,
      SLOT(updateShrinkSettingWidgets()));
   connect(
      &m_rbShrinkPercent,
      SIGNAL(toggled(bool)),
      this,
      SLOT(updateShrinkSettingWidgets()));

   connect(
      &m_edPixelsWidth,
      SIGNAL(textEdited(QString)),
      this,
      SLOT(keepAspectPixelWidth()));
   connect(
      &m_edPixelsHeight,
      SIGNAL(textEdited(QString)),
      this,
      SLOT(keepAspectPixelHeight()));
   connect(
      &m_edPercentHeight,
      SIGNAL(textEdited(QString)),
      this,
      SLOT(keepAspectPercentHeight()));
   connect(
      &m_edPercentWidth,
      SIGNAL(textEdited(QString)),
      this,
      SLOT(keepAspectPercentWidth()));

   connect(
      &m_btnJustShrink,
      SIGNAL(clicked()),
      this,
      SLOT(shrink()));

   connect(
      &m_worker,
      SIGNAL(finished()),
      this,
      SLOT(finish()));

   connect(
      &m_cbPercentAspectRatio,
      SIGNAL(stateChanged(int)),
      this,
      SLOT(syncPercentAspect()));
   connect(
      &m_cbPixelsAspectRatio,
      SIGNAL(stateChanged(int)),
      this,
      SLOT(syncPixelsAspect()));

   connect(
      &m_lPreview,
      SIGNAL(clicked()),
      this,
      SLOT(selectSourceFile()));
   connect(
      &m_lPreview,
      SIGNAL(linkActivated(QString)),
      this,
      SLOT(selectSourceFile()));

   connect(
      &m_btnDestFile,
      SIGNAL(clicked()),
      this,
      SLOT(selectDestFile()));

   connect(
      &m_btnLangEnglish,
      SIGNAL(clicked()),
      this,
      SLOT(setLangEnglish()));
   connect(
      &m_btnLangGerman,
      SIGNAL(clicked()),
      this,
      SLOT(setLangGerman()));

   m_translatorGer.load(":/Shrinky_de.qm");
   QLocale defaultLocale;
   if (defaultLocale.language() == QLocale::German)
      setLangGerman();
   retranslateUi();
   setSourceFile(aImage);
   m_rbShrinkFileSize.setChecked(true);
   updateSettingsVisible();
}

ShrinkChoiceWin::~ShrinkChoiceWin()
{
   m_worker.wait(0);
}

void ShrinkChoiceWin::retranslateUi()
{
   m_btnJustShrink.setText(tr("&Shrink it!"));
   m_btnCustomShrink.setText(tr("I want to &decide myself"));
   m_btnCustomShrink.setDescription(tr("Decide yourself which settings should be used to shrink this image."));
   m_gbSettings.setTitle(tr("Settings"));
   m_rbShrinkFileSize.setText(tr("I want to shrink to this filesize"));
   m_lFileSize.setText(tr("&Filesize:"));
   m_lFileSizeExamples.setText(tr("Examples: 1,5mb, 120 kb, 1.000"));
   m_rbShrinkPixels.setText(tr("&I want to shrink to this width and height"));
   m_cbPixelsAspectRatio.setText(tr("&Keep aspect ratio"));
   m_lPixelsWidth.setText(tr("&Width:"));
   m_lPixelsHeight.setText(tr("&Height:"));
   m_lPixels.setText(tr("Pixels"));
   m_rbShrinkPercent.setText(tr("I want to shrink by this &percentage"));
   m_cbPercentAspectRatio.setText(tr("&Keep aspect ratio"));
   m_lPercentWidth.setText(tr("&Width:"));
   m_lPercentHeight.setText(tr("&Height:"));
   m_lPercent.setText(tr("%"));
   m_lDestFile.setText(tr("Save &result to:"));
   m_btnDestFile.setText(tr("..."));
   m_cbOpenAfterShrinking.setText(tr("&Open the shrinked file when finished."));
   if (m_btnCustomShrink.isChecked())
   {
      m_btnJustShrink.setDescription(tr("Shrink the image using the settings you made below."));
   }
   else
   {
      m_btnJustShrink.setDescription(tr("Let Shrinky determine the best settings to shrink this image."));
   }
   if (m_strSourceFileName.isEmpty())
   {
      m_lPreview.setText(
         "<p><img src=\":/Shrinky.png\" alt=\"Shrinky\"/></p><p><center><a href=\"load\">" +
         tr("Click here to load an image to shrink.") + "</a></center></p>");
   }
}

void ShrinkChoiceWin::showEvent(QShowEvent *event)
{
   QWidget::showEvent(event);
   resize(sizeHint());
   resize(width(), 1);
   QDesktopWidget desktop;
   QRect r(frameGeometry());
   r.moveCenter(desktop.availableGeometry(this).center());
   move(r.left(), r.top());
}

void ShrinkChoiceWin::closeEvent(QCloseEvent *event)
{
   if (m_worker.isRunning())
      event->ignore();
}

void ShrinkChoiceWin::updateSettingsVisible()
{
   m_gbSettings.setVisible(m_btnCustomShrink.isChecked());
   retranslateUi();
   resize(width(), 1);
}

void ShrinkChoiceWin::updateShrinkSettingWidgets()
{
   m_wShrinkFileSize.setVisible(m_rbShrinkFileSize.isChecked());
   m_wShrinkPixels.setVisible(m_rbShrinkPixels.isChecked());
   m_wShrinkPercent.setVisible(m_rbShrinkPercent.isChecked());
   resize(width(), 1);
}

void ShrinkChoiceWin::keepAspectPixelWidth()
{
   if (m_cbPixelsAspectRatio.isChecked())
   {
      int w = m_edPixelsWidth.text().toInt();
      if (w > 0)
      {
         QSize sourceSize = m_imgSource.size();
         double sw = sourceSize.width(),
            sh = sourceSize.height();
         m_edPixelsHeight.setText(QString::number(static_cast<int>(sh / sw * w)));
      }
   }
}

void ShrinkChoiceWin::keepAspectPixelHeight()
{
   if (m_cbPixelsAspectRatio.isChecked())
   {
      int h = m_edPixelsHeight.text().toInt();
      if (h > 0)
      {
         QSize sourceSize = m_imgSource.size();
         double sw = sourceSize.width(),
            sh = sourceSize.height();
         m_edPixelsWidth.setText(QString::number(static_cast<int>(sw / sh * h)));
      }
   }
}

void ShrinkChoiceWin::keepAspectPercentWidth()
{
   if (m_cbPercentAspectRatio.isChecked())
   {
      int w = m_edPercentWidth.text().toInt();
      if (w > 0)
      {
         m_edPercentHeight.setText(QString::number(w));
      }
   }
}

void ShrinkChoiceWin::keepAspectPercentHeight()
{
   if (m_cbPercentAspectRatio.isChecked())
   {
      int h = m_edPercentHeight.text().toInt();
      if (h > 0)
      {
         m_edPercentWidth.setText(QString::number(h));
      }
   }
}

void ShrinkChoiceWin::syncPercentAspect()
{
   if (m_cbPercentAspectRatio.isChecked())
   {
      keepAspectPercentWidth();
   }
}

void ShrinkChoiceWin::syncPixelsAspect()
{
   if (m_cbPixelsAspectRatio.isChecked())
   {
      keepAspectPixelWidth();
   }
}

void ShrinkChoiceWin::shrink()
{
   if (m_worker.isRunning())
   {
      QMessageBox::warning(
         this,
         qApp->applicationName(),
         tr("Shrinking is already in progress."));
      return;
   }

   ShrinkParams params;
   params.source = m_strSourceFileName;
   params.dest = m_edDestFile.text();

   if (QFile::exists(params.dest))
   {
      if (!m_askedForOverwrite)
      {
         if (QMessageBox::question(
            this,
            qApp->applicationName(),
            tr("The file %1 already exists. Do you want to overwrite it?")
               .arg(params.dest),
            QMessageBox::Yes,
            QMessageBox::No) != QMessageBox::Yes)
         {
            return;
         }
      }
   }

   if (m_rbShrinkFileSize.isChecked())
   {
      params.mode = smFileSize;
      if (!parseFileSize(m_edFileSize.text(), params.size))
      {
         QMessageBox::warning(
            this,
            qApp->applicationName(),
            tr("The entered file-size of %1 is not valid.").arg(m_edFileSize.text()));
         return;
      }
   }
   else
   {
      if (m_rbShrinkPixels.isChecked())
      {
         params.width = m_edPixelsWidth.text().toInt();
         params.height = m_edPixelsHeight.text().toInt();
      }
      else
      {
         params.width =
            static_cast<int>(
               m_imgSource.size().width() *
               (m_edPercentWidth.text().toInt() / 100.0));
         params.height =
            static_cast<int>(
               m_imgSource.size().height() *
               (m_edPercentHeight.text().toInt() / 100.0));
      }
      params.mode = smPixels;

      if (params.width == 0)
      {
         QMessageBox::warning(
            this,
            qApp->applicationName(),
            tr("Invalid width, please enter a valid new width that's greater than zero."));
         return;
      }
      if (params.height == 0)
      {
         QMessageBox::warning(
            this,
            qApp->applicationName(),
            tr("Invalid height, please enter a valid new height that's greater than zero."));
         return;
      }
   }
   setEnabled(false);

   m_worker.shrink(params);
}

void ShrinkChoiceWin::finish()
{
   setEnabled(true);
   if (!m_worker.lastError().isEmpty())
   {
      QMessageBox::critical(
         this,
         qApp->applicationName(),
         m_worker.lastError());
      return;
   }
   if (m_cbOpenAfterShrinking.isChecked())
   {
      QString url = "file:///" + m_edDestFile.text();
      QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
   }
   close();
}

void ShrinkChoiceWin::selectSourceFile()
{
   QString newFile = QFileDialog::getOpenFileName(
      this,
      tr("Select a file to shrink"),
      m_strSourceFileName,
      fileFilter());
   if (!newFile.isEmpty())
   {
      newFile = QDir::toNativeSeparators(newFile);
      setSourceFile(newFile);
   }
}

bool ShrinkChoiceWin::setSourceFile(const QString &aFileName)
{
   if (aFileName.isEmpty())
   {
      m_imgSource = QImage();
      m_strSourceFileName.clear();
      m_edPercentWidth.setText(QString());
      m_edPercentHeight.setText(QString());
      m_edPixelsWidth.setText(QString());
      m_edPixelsHeight.setText(QString());
      m_edDestFile.setText(QString());
      return false;
   }
   if (!m_imgSource.load(aFileName))
   {
      QMessageBox::warning(
         this,
         qApp->applicationName(),
         tr("The file seems to be in an unsupported format."));
      return false;
   }
   m_strSourceFileName = aFileName;
   m_lPreview.setPixmap(
      QPixmap::fromImage(
         QImage(aFileName).scaledToHeight(
            250,
            Qt::SmoothTransformation)));
   m_lSourceFileName.setText(QDir::toNativeSeparators(aFileName));
   QFileInfo info(aFileName);
   m_edDestFile.setText(
      QDir::toNativeSeparators(
         info.absoluteDir().absolutePath() +
         "/" + info.baseName() +
         tr("-small.%1").arg("png")));

   QSize sourceSize(m_imgSource.size());
   sourceSize.scale(QSize(640, 480), Qt::KeepAspectRatio);
   m_edPixelsWidth.setText(QString::number(sourceSize.width()));
   m_edPixelsHeight.setText(QString::number(sourceSize.height()));

   QString percentage(
      QString::number(
            ((sourceSize.width() * 10) /
             (m_imgSource.size().width())) * 10));
   m_edPercentWidth.setText(percentage);
   m_edPercentHeight.setText(percentage);
   return true;
}

QString ShrinkChoiceWin::fileFilter() const
{
   return tr("Images (*.png *.jpeg *jpg *gif *bmp *tiff);;All files (*.*)");
}

void ShrinkChoiceWin::selectDestFile()
{
   QString newFile = QFileDialog::getSaveFileName(
      this,
      tr("Select a destination file-name"),
      m_edDestFile.text(),
      fileFilter());
   if (!newFile.isEmpty())
   {
      newFile = QDir::toNativeSeparators(newFile);
      m_edDestFile.setText(newFile);
      m_askedForOverwrite = true;
   }
}

void ShrinkChoiceWin::setLangEnglish()
{
   qApp->removeTranslator(&m_translatorGer);
   retranslateUi();
   resize(width(), 1);
}

void ShrinkChoiceWin::setLangGerman()
{
   qApp->installTranslator(&m_translatorGer);
   retranslateUi();
   resize(width(), 1);
}
