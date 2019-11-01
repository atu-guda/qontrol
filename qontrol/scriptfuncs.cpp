/***************************************************************************
                     scriptfuncs.cpp - functions and types to use from JS
                             -------------------
    begin                : 2015.01.20 - partial copy from model.cpp
    copyright            : (C) 2015-2019 by atu
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

#include <cmath>
#include <iostream>

#include <QFile>
#include <QDebug>

#include "scriptfuncs.h"

using namespace std;

QString XsScriptExtension::int2str( long v, int fw, QChar fc, int base ) const
{
  return QSL("%1").arg( v, fw, base, fc );
}

bool XsScriptExtension::isNear( double a, double b, double eps ) const
{
  return fabs( a-b ) <= eps;
}


bool XsScriptExtension::include( const QString &file )
{
  QString sfile = QSL("scripts:") + file;
  if( ! QFile::exists( sfile ) ) {
    return false;
  }

  QFile sf( sfile );
  if( ! sf.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    qWarning() << "Fail to open script file " << sfile << WHE;
    return false;
  }

  QByteArray scr = sf.readAll();
  if( scr.isEmpty() ) {
    return false;
  }

  QScriptValue res = eng->evaluate( scr );
  if( res.isError() ) {
    int line = res.property( QSL("lineNumber") ).toInt32();
    QString msg = res.property( QSL("mesage") ).toString();
    qWarning() << QSL("Exception at line ") << line <<  QSL(" file: ") << sfile
               << QSL(" error: ") << msg;
    return false;
  }

  return true;

}


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
  QString s = QSL( "%1" ).arg( n, fw, base, fc );
  return QScriptValue( s );
}

QScriptValue script_print( QScriptContext *ctx, QScriptEngine *eng )
{
  QString r;
  for( int i=0; i < ctx->argumentCount(); ++i ) {
    if (i > 0) {
      r.append(" ");
    }
    r.append( ctx->argument(i).toString() );
  }

  log_app.append( r );
  cout << qP( r );

  return eng->undefinedValue();
}


QScriptValue script_isNear( QScriptContext *ctx, QScriptEngine * /*eng*/ )
{
  double a = 0, b = 0, eps = D_EPS;
  int n = ctx->argumentCount();
  if( n > 0 ) {
    a = ctx->argument(0).toNumber();
  }
  if( n > 1 ) {
    b = ctx->argument(1).toNumber();
  }
  if( n > 2 ) {
    eps = ctx->argument(2).toNumber();
  }
  bool r = fabs( a-b ) < eps;

  return QScriptValue( r );
}


QScriptValue script_include( QScriptContext *ctx, QScriptEngine *eng )
{
  if( ctx->argumentCount() < 1 ) {
    return QScriptValue( false );
  }
  QString sfile = ctx->argument(0).toString();

  sfile.prepend( QSL("scripts:") );
  if( ! QFile::exists( sfile ) ) {
    qWarning() << QSL("file to include ") << sfile << QSL(" not exists") << WHE;
    return QScriptValue( false );
  }

  QFile sf( sfile );
  if( ! sf.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    qWarning() << "Fail to open script file " << sfile << WHE;
    return QScriptValue( false );
  }

  QByteArray scr = sf.readAll();
  if( scr.isEmpty() ) {
    return QScriptValue( false );
  }

  QScriptValue res = eng->evaluate( scr );
  QString r = res.toString();
  if( eng->hasUncaughtException() ) {
    int line = eng->uncaughtExceptionLineNumber();
    qWarning() << "uncaught exception at line " << line <<  " file: " << sfile
               << " error: " << r;
    return QScriptValue( false );
  } else {
    qWarning() << "included file " << sf.fileName() << WHE;
  }

  return QScriptValue( true );
}





QString TripleF::toString() const
{
  return QSL( "[ %1; %2; $3 ]").arg(a).arg(b).arg(c);
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


