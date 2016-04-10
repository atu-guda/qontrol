#pragma once
/***************************************************************************
   longvalidator.h  -  LongValidator definition
                             -------------------
    begin                : 2016.04.10
    copyright            : GPLv2+ (C) 2016-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QValidator>
#include <QString>


class LongValidator : public QValidator
{
  Q_OBJECT;
  Q_PROPERTY( long bottom READ bottom WRITE setBottom NOTIFY bottomChanged );
  Q_PROPERTY( long top READ top WRITE setTop NOTIFY topChanged );

 public:
  explicit LongValidator( QObject * parent = Q_NULLPTR );
  LongValidator( long bottom, long top, QObject *parent = nullptr );
  // virtual ~LongValidator();

  QValidator::State validate( QString &s, int &pos ) const override;
  void fixup( QString &input ) const override;

  void setBottom( long _b );
  void setTop( long _t );
  virtual void setRange( long _b, long _t );

  long bottom() const { return b; }
  long top() const { return t; }
 Q_SIGNALS:
  void bottomChanged( long bottom );
  void topChanged( long top );

 private:
  Q_DISABLE_COPY( LongValidator );

  long b;
  long t;
};

