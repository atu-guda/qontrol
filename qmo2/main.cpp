/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2014 by atu
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
#include <iostream>

#include <QApplication>
#include <QFont>

#include "getopt.h"

#include "labowin.h"

using namespace std;

void print_usage( const char *appname );

int main( int argc, char *argv[] )
{
  setenv( "LC_NUMERIC", "C", 1 );

  QApplication a( argc, argv );

  int op;
  while( ( op=getopt( argc, argv, "hv" ) ) != -1 ) {
    switch( op ) {
      case 'h' : print_usage( argv[0] ); return 0;
      case 'v' : cout << PACKAGE << ' ' << VERSION << endl;  return 0;
      default: cerr << "Error: unknown option '" << (char)(optopt) << "'" << endl;
    }
  }

  LaboWin *main_win = new LaboWin();

  main_win->show();

  for( int i=optind; i<argc ; ++i ) {
    main_win->doFileOpenXML( QString::L8B( argv[1] ) );
  }

  a.setApplicationDisplayName( PACKAGE );

  return a.exec();
}

void print_usage( const char *appname )
{
  cout << "Usage: \n" << appname << " [-h] [-v] [files...]" << endl;
}

// end of main.cpp

