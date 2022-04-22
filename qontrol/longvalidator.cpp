/***************************************************************************
   longvalidator.cpp  -  LongValidator definition
                             -------------------
    begin                : 2016.04.10
    copyright            : GPLv2+ (C) 2016-2022 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/
// simplefied from QIntVlaidator (+allow hex, oct)

#include <defs.h>
#include <longvalidator.h>


LongValidator::LongValidator( QObject *parent )
  : QValidator( parent )
{
  b = LONG_MIN;
  t = LONG_MAX;
}


LongValidator::LongValidator( long _b, long _t, QObject *parent )
  : QValidator( parent ), b( _b ), t( _t )
{
}


QValidator::State LongValidator::validate( QString &input, int& /*pos*/ ) const
{
  if( input.isEmpty() ) {
    return Intermediate;
  }

  if( b >= 0  &&  input.startsWith('-') ) {
    return Invalid;
  }

  if( t < 0 && input.startsWith('+') ) {
    return Invalid;
  }

  if( input.size() == 1 && (input.at(0) == '+' || input.at(0) == '-') ) {
    return Intermediate;
  }

  bool ok;
  long entered = input.toLong( &ok, 0 );
  if( !ok ) {
    return Invalid;
  }

  if( entered >= b && entered <= t ) {
    return Acceptable;
  }

  return Intermediate;
}

void LongValidator::fixup( QString &input ) const
{
  QString res;
  static const QString good_chars { QSL("-+0123456789abcdefABCDEFx") };
  for( auto c : input ) {
    if( good_chars.contains( c ) ) {
      res += c;
    }
  }
  input = res;
}

void LongValidator::setRange( long _b, long _t )
{
  bool rangeChanged = false;
  if( b != _b ) {
    b = _b;
    rangeChanged = true;
    emit bottomChanged( b );
  }

  if( t != _t ) {
    t = _t;
    rangeChanged = true;
    emit topChanged( t );
  }

  if (rangeChanged) {
    emit changed();
  }
}


void LongValidator::setBottom( long _b )
{
  setRange( _b, t );
}

void LongValidator::setTop( long _t )
{
  setRange( b, _t );
}


