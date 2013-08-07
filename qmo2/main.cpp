/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2013 by atu
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

#include <gsl/gsl_rng.h>

#include <QApplication>
#include <QFont>

#include "qmo2win.h"

int main( int argc, char *argv[] )
{
  QApplication a(argc, argv);
  //a.setFont( QFont( "helvetica", 12, QFont::Normal, false ) );
  gsl_rng_env_setup();

  QMo2Win *qmo2 = new QMo2Win();

  qmo2->show();

  if( argc>1 )
    qmo2->doFileOpenXML( QString::fromLocal8Bit( argv[1] ) );
	
  return a.exec();
}

// end of main.cpp

