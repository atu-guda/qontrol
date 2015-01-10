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
int batch_process( const char *model_file, const QStringList &ovars );

int main( int argc, char *argv[] )
{
  char *fn_new = nullptr;
  setenv( "LC_NUMERIC", "C", 1 );

  QApplication a( argc, argv );

  QStringList out_vars; // output variables in batch mode, relative to scheme

  int op;
  while( ( op=getopt( argc, argv, "hvbc:o:s:u:x:X:d::" ) ) != -1 ) {
    switch( op ) {
      case 'h' : print_usage( argv[0] ); return 0;
      case 'v' : cout << PACKAGE << ' ' << VERSION << endl;  return 0;
      case 'b': prog_opts.batch = true; break;
      case 'c': fn_new = optarg; prog_opts.batch = true; break;
      case 'o': prog_opts.out_file = optarg; break;
      case 's': prog_opts.sim_name = optarg; break;
      case 'u': out_vars << L8B( optarg ); break;
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

  return batch_process( argv[optind], out_vars );

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

int batch_process( const char *model_file, const QStringList &ovars )
{
  LaboDoc doc;
  if( ! doc.openDocumentXML( L8B( model_file ) ) ) {
    cerr << "Fail to read file \"" << model_file << "\"" << endl;
    return 3;
  }
  TModel *model = doc.getModel();
  if( !model ) {
    cerr << "Not found model in file" << endl;
    return 5;
  }

  // TODO: sim!
  if( prog_opts.sim_name ) {
    QString sim_name = L8B( prog_opts.sim_name );
    if( ! model->setActiveSimul( sim_name ) ) {
      cerr << "Not found simulation \"" << prog_opts.sim_name << "\" in model" << endl;
      return 4;
    }
  }
  cout << "Starintg run: " << endl;
  int rc = model->run_bg();
  cout << "End run, rc= " << rc << endl;

  if( rc ) {
    for( auto nm : ovars ) {
      QString v = model->getOutValue( nm );
      cout << qP(nm) << " = " << qP( v ) << endl;
    };
  }

  return 10;
}

// end of main.cpp

