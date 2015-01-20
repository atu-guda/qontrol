/***************************************************************************
                     scriptfuncs.cpp - functions and types to use from JS
                             -------------------
    begin                : 2015.01.20 - partial copy from model.cpp
    copyright            : (C) 2015-2015 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "scriptfuncs.h"

using namespace std;

// -------------------- helper JS functions ---------------------------------------
QScriptValue script_int2str( QScriptContext *ctx, QScriptEngine * /*eng*/ )
{
  int narg = ctx->argumentCount();
  int n = 0;
  if( narg > 0 ) {
    n = ctx->argument(0).toInt32();
  }
  int fw = 1;
  if( narg > 1 ) {
    fw = ctx->argument(1).toInt32();
  }
  QChar fc = ' ';
  if( narg > 2 ) {
    fc = ctx->argument(2).toString()[0];
  }
  int base = 10;
  if( narg > 3 ) {
    base = ctx->argument(3).toInt32();
  }
  QString s = QString( "%1" ).arg( n, fw, base, fc );
  return QScriptValue( s );
}

QString TripleF::toString() const
{
  return QString( "[ %1; %2; $3 ]").arg(a).arg(b).arg(c);
}

QScriptValue TripleFtoScriptValue( QScriptEngine *eng, const TripleF &s )
{
  QScriptValue obj = eng->newObject();
  obj.setProperty( "a", s.a );
  obj.setProperty( "b", s.b );
  obj.setProperty( "c", s.c );
  return obj;
}

void fromScriptValuetoTripleF( const QScriptValue &obj, TripleF &s )
{
  s.a = obj.property("a").toNumber();
  s.b = obj.property("b").toNumber();
  s.c = obj.property("c").toNumber();
}

