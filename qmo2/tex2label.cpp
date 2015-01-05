/***************************************************************************
                          tex2label.cpp  - function and data to convert
                          some TeX commands to HTML subset for QLabel
                             -------------------
    begin                : 2015.01.15
    copyright            : GPL (C) 2015-2015 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QString>

QString tex2label( const QString &t )
{
  QString r;
  for( QChar c : t ) {
    r += c;
  }


  return r;
}

