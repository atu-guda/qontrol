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
#include "labodoc.h"

using namespace std;

void print_usage( const char *appname );
int  convert_model( const char *fn_old, const char *fn_new );

int main( int argc, char *argv[] )
{
  char *fn_new = nullptr;
  setenv( "LC_NUMERIC", "C", 1 );

  QApplication a( argc, argv );

  int op;
  while( ( op=getopt( argc, argv, "hvc:" ) ) != -1 ) {
    switch( op ) {
      case 'h' : print_usage( argv[0] ); return 0;
      case 'v' : cout << PACKAGE << ' ' << VERSION << endl;  return 0;
      case 'c': fn_new = optarg; break;
      default: cerr << "Error: unknown option '" << (char)(optopt) << "'" << endl;
    }
  }

  if( fn_new ) { // conversion requested
    if( (argc-optind) != 1 ) { // only one positional allowed here
      cerr << "Single input filename required for conversion" << endl;
      return 2;
    }
    return convert_model( argv[optind], fn_new );
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

int convert_model( const char *fn_old, const char *fn_new )
{
  LaboDoc doc;
  if( ! doc.openDocumentXML( L8B( fn_old ) ) ) {
    cerr << "Fail to read file \"" << fn_old << "\"" << endl;
    return 3;
  }
  if( ! doc.saveDocumentXML( L8B( fn_new ) ) ) {
    cerr << "Fail to save file \"" << fn_new << "\"" << endl;
    return 3;
  }
  return 0;
}

// end of main.cpp

