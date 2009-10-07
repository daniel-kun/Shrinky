#ifndef CHOICE_H_04102009
#define CHOICE_H_04102009

#include <QLabel>
#include <QFrame>
#include <QWidget>
#include <QThread>
#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QBoxLayout>
#include <QTranslator>
#include <QGridLayout>
#include <QToolButton>
#include <QRadioButton>
#include <QIntValidator>
#include <QCommandLinkButton>

enum ShrinkMode
{
   smFileSize,
   smPixels
};

struct ShrinkParams
{
   // source and destination file-names.
   QString source, dest;
   ShrinkMode mode;
   // When mode is smFileSize, size holds the new file-size in bytes and width and height are not used.
   // When mode is smPixels, width and height are the width and height in pixels and size is not used.
   int width, height;
   quint64 size;
};

class ShrinkWorker: public QThread
{
   Q_OBJECT

public:
   void shrink(const ShrinkParams &aParams);
   QString lastError() const;

protected:
   virtual void run();

private:
   ShrinkParams m_params;
   QString m_lastError;
};

class ClickLabel: public QLabel
{
   Q_OBJECT

signals:
   void clicked();

public:
   ClickLabel(QWidget *parent = 0);

protected:
   virtual void mousePressEvent(QMouseEvent *event);
};

class ShrinkChoiceWin: public QWidget
{
   Q_OBJECT

public: 
   ShrinkChoiceWin(const QString &aImage, QWidget *aParent = 0);
   virtual ~ShrinkChoiceWin();

protected:
   virtual void showEvent(QShowEvent *event);
   virtual void closeEvent(QCloseEvent *event);

private:
   QString m_strSourceFileName;
   QImage m_imgSource;
   QPixmap m_pxBackground;
   ShrinkWorker m_worker;

   QVBoxLayout m_vlMain;

   QHBoxLayout m_hlPreviewFrame;
   QFrame m_frPreview;
   QVBoxLayout m_vlPreview;
   ClickLabel m_lPreview;
   QLabel m_lSourceFileName;
   QCommandLinkButton m_btnJustShrink, m_btnCustomShrink;

   QGroupBox m_gbSettings;
   QVBoxLayout m_vlSettings;

   QRadioButton m_rbShrinkFileSize;
   QWidget m_wShrinkFileSize;
   QGridLayout m_glShrinkFileSize;
   QLabel m_lFileSize;
   QLineEdit m_edFileSize;
   QLabel m_lFileSizeExamples;

   QRadioButton m_rbShrinkPixels;
   QWidget m_wShrinkPixels;
   QGridLayout m_glShrinkPixels;
   QCheckBox m_cbPixelsAspectRatio;
   QLabel m_lPixelsWidth, m_lPixelsHeight, m_lPixels;
   QLineEdit m_edPixelsWidth, m_edPixelsHeight;

   QRadioButton m_rbShrinkPercent;
   QWidget m_wShrinkPercent;
   QGridLayout m_glShrinkPercent;
   QCheckBox m_cbPercentAspectRatio;
   QLabel m_lPercentWidth, m_lPercentHeight, m_lPercent;
   QLineEdit m_edPercentWidth, m_edPercentHeight;

   QHBoxLayout m_hlDestFile;
   QLabel m_lDestFile;
   QLineEdit m_edDestFile;
   QToolButton m_btnDestFile;

   QHBoxLayout m_hlBottom;
   QCheckBox m_cbOpenAfterShrinking;
   QToolButton m_btnLangEnglish, m_btnLangGerman;

   QIntValidator m_valPixels, m_valPercent;
   QTranslator m_translatorGer;

   bool m_askedForOverwrite;

   void retranslateUi();
   bool setSourceFile(const QString &aFileName);
   QString fileFilter() const;

private slots:
   void updateSettingsVisible();
   void updateShrinkSettingWidgets();

   void keepAspectPixelWidth();
   void keepAspectPixelHeight();

   void keepAspectPercentWidth();
   void keepAspectPercentHeight();

   void syncPercentAspect();
   void syncPixelsAspect();

   void shrink();
   void finish();

   void selectDestFile();
   void selectSourceFile();

   void setLangEnglish();
   void setLangGerman();
};

#endif
