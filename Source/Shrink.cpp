#include "Shrink.h"

#include <QImage>
#include <QFile>
#include <QBuffer>

#include <cmath>

ShrinkError shrinkImage(const QString &aSource, const QString &aDest, QSize aSize)
{
   if (!QFile::exists(aSource))
      return ShrinkError(
         secSourceDoesNotExist, 
         QObject::tr("The source-file %1 does not exist.").arg(aSource));

   QImage source;
   if (!source.load(aSource))
   {
      return ShrinkError(
            secFailedToLoadSource,
            QObject::tr("The source-file %1 could not be loaded.").arg(aSource));
   }
   QImage dest(
      source.scaled(
         aSize,
         Qt::IgnoreAspectRatio,
         Qt::SmoothTransformation));
   if (!dest.save(aDest, "PNG"))
   {
      return ShrinkError(
         secFailedToSaveDest,
         QObject::tr("Could not save destination-file %1.").arg(aDest));
   }
   return ShrinkError();
}

namespace
{
   quint64 shrinkTry(
      QImage &aSource,
      double factor,
      QBuffer &result)
   {
      result.buffer().clear();
      result.reset();
      aSource.scaled(
         static_cast<int>(aSource.width() * factor),
         static_cast<int>(aSource.height() * factor))
            .save(&result, "PNG");
      return result.size();
   }
}

ShrinkError shrinkImageToFileSize(
   const QString &aSource,
   const QString &aDest,
   quint64 aSize)
{
   if (!QFile::exists(aSource))
      return ShrinkError(
         secSourceDoesNotExist,
         QObject::tr("The source-file %1 does not exist.").arg(aSource));

   QImage source;
   if (!source.load(aSource))
   {
      return ShrinkError(
            secFailedToLoadSource,
            QObject::tr("The source-file %1 could not be loaded.").arg(aSource));
   }

   double sourceSize = 0.0;
   {
      QBuffer tempBuffer;
      tempBuffer.open(QBuffer::WriteOnly);
      // First, save as PNG (we don't know the source file-format, so we always
      // save is at PNG, even if it originally was one) to find out the unscaled
      // file-size.
      if (!source.save(&tempBuffer, "PNG"))
         return ShrinkError(
            secFailedToConvert,
            QObject::tr("The source-file could not be saved in PNG format."));
      sourceSize = tempBuffer.buffer().size();
   }
   double factor = std::sqrt(aSize / sourceSize);
   QSize newSize(
      static_cast<int>(source.width() * factor),
      static_cast<int>(source.height() * factor));
   if (!source.scaled(
      newSize,
      Qt::IgnoreAspectRatio,
      Qt::SmoothTransformation).save(
         aDest, "PNG"))
   {
      return ShrinkError(
         secFailedToSaveDest,
         QObject::tr("Could not save destination-file %1.").arg(aDest));
   }
   return ShrinkError();
}
