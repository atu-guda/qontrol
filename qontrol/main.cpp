/***************************************************************************
                          main.cpp  - main file for qontrol project
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2015 by atu
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

ProgOpts prog_opts;

void print_usage( const char *appname );
int  convert_model( const char *fn_old, const char *fn_new );
int batch_process( const char *model_file );

int main( int argc, char *argv[] )
{
  char *fn_new = nullptr;
  setenv( "LC_NUMERIC", "C", 1 );

  QApplication a( argc, argv );

  int op;
  while( ( op=getopt( argc, argv, "hvbc:o:s:x:X:d::" ) ) != -1 ) {
    switch( op ) {
      case 'h' : print_usage( argv[0] ); return 0;
      case 'v' : cout << PACKAGE << ' ' << VERSION << endl;  return 0;
      case 'b': prog_opts.batch = true; break;
      case 'c': fn_new = optarg; prog_opts.batch = true; break;
      case 'o': prog_opts.out_file = optarg; break;
      case 's': prog_opts.sim_name = optarg; break;
      case 'x': prog_opts.script = optarg; break;
      case 'X': prog_opts.file_script = optarg; break;
      case 'd': if( optarg ) { 
                  prog_opts.dbg = atoi( optarg ); 
                } else {
                  ++prog_opts.dbg;
                };
                break;
      default: cerr << "Error: unknown option '" << (char)(optopt) << "'" << endl;
    }
  }

  if( ! prog_opts.batch ) { // process with interface
    LaboWin *main_win = new LaboWin();

    main_win->show();

    for( int i=optind; i<argc ; ++i ) {
      main_win->doFileOpenXML( QString::L8B( argv[1] ) );
    }
    a.setApplicationDisplayName( PACKAGE );

    return a.exec();
  }


  // batch processing -- the only filename requires
  if( (argc-optind) != 1 ) { // only one positional allowed here
    cerr << "Single input filename required for batch procession" << endl;
    return 2;
  }

  if( fn_new ) { // conversion requested
    return convert_model( argv[optind], fn_new );
  }

  return batch_process( argv[optind] );

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

int batch_process( const char *model_file )
{
  LaboDoc doc;
  if( ! doc.openDocumentXML( L8B( model_file ) ) ) {
    cerr << "Fail to read file \"" << model_file << "\"" << endl;
    return 3;
  }
  cerr << "Batch process not implemented for now! ;-(" << endl;
  return 10;
}

// end of main.cpp
