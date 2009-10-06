#ifndef SHRINK_H_02102009
#define SHRINK_H_02102009

#include <QString>
#include <QSize>

enum ShrinkErrorCode
{
   secNoError,
   secSourceDoesNotExist,
   secFailedToConvert,
   secSourceFileIsEmpty,
   secFailedToLoadSource,
   secFailedToSaveDest
};

struct ShrinkError
{
   ShrinkErrorCode code;
   QString message;

   ShrinkError(): code(secNoError) { }
   ShrinkError(ShrinkErrorCode code, const QString &message):
         code(code),
         message(message)
   {
   }
   operator bool() const
   {
      return code == secNoError;
   }
};

ShrinkError shrinkImage(const QString &aSource, const QString &aDest, QSize aSize);
ShrinkError shrinkImageToFileSize(
   const QString &aSource,
   const QString &aDest,
   quint64 aSize);
#endif
