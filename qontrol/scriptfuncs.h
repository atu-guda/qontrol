#pragma once
/***************************************************************************
                          scriptfuncs.h - JS funcs description
                             -------------------
    begin                : Tue Aug 1 2000
    copyright            : (C) 2000-2022 by atu
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


#include <QString>
#include <QScriptEngine>
#include "defs.h"

// ----------------  misc types --------------------------------------------

// struct to return 3-values from C++ to js
// helper functions: in tmodel.cpp

struct TripleF {
  double a, b, c;
  QString toString() const;
};
Q_DECLARE_METATYPE(TripleF);
QScriptValue TripleFtoScriptValue( QScriptEngine *eng, const TripleF &s );
void fromScriptValuetoTripleF( const QScriptValue &obj, TripleF &s );

class XsScriptExtension : public QObject {
  Q_OBJECT
  public:
    XsScriptExtension( QScriptEngine *par = nullptr ) : QObject( par ), eng( par ) {};
    virtual ~XsScriptExtension() = default;
    Q_INVOKABLE QString int2str( long v, int fw, QChar fc = ' ', int base = 10 ) const;
    Q_INVOKABLE bool isNear( double a, double b, double eps = D_EPS ) const;
    Q_INVOKABLE bool include( const QString &file );
  protected:
    QScriptEngine *eng;
};


QScriptValue script_int2str( QScriptContext *ctx, QScriptEngine *eng );
QScriptValue script_print( QScriptContext *ctx, QScriptEngine *eng );
QScriptValue script_isNear( QScriptContext *ctx, QScriptEngine *eng );
QScriptValue script_include( QScriptContext *ctx, QScriptEngine *eng );




