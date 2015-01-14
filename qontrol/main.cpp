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

  prog_opts.inc_dirs << ".";
  prog_opts.inc_dirs << "./scripts";

  int op;
  while( ( op=getopt( argc, argv, "hvbeNMc:g:o:s:u:x:I:X:d::" ) ) != -1 ) {
    switch( op ) {
      case 'h' : print_usage( argv[0] ); return 0;
      case 'v' : cout << PACKAGE << ' ' << VERSION << endl;  return 0;
      case 'b': prog_opts.batch = true; break;
      case 'e': prog_opts.exit_st = true; break;
      case 'N': prog_opts.norun = true; break;
      case 'M': prog_opts.mod_scr = true; break;
      case 'c': fn_new = optarg; prog_opts.batch = true; break;
      case 'g': prog_opts.out_plots << L8B( optarg ); break;
      case 'o': prog_opts.out_file = L8B( optarg ); break;
      case 's': prog_opts.sim_name = L8B( optarg ); break;
      case 'u': prog_opts.out_vars << L8B( optarg ); break;
      case 'x': prog_opts.script = L8B( optarg ); break;
      case 'X': prog_opts.s_files << L8B( optarg ); break;
      case 'I': prog_opts.inc_dirs << L8B( optarg ); break;
      case 'd': if( optarg ) {
                  prog_opts.dbg = atoi( optarg );
                } else {
                  ++prog_opts.dbg;
                };
                break;
      default: cerr << "Error: unknown option '" << (char)(optopt) << "'" << endl;
    }
  }

  QDir::setSearchPaths( "scripts", prog_opts.inc_dirs );

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
  // TODO: after options relaxation, fill here
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
  int rc = 0;
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

  if( ! prog_opts.sim_name.isEmpty() ) {
    if( ! model->setActiveSimul( prog_opts.sim_name ) ) {
      cerr << "Not found simulation \"" << qP(prog_opts.sim_name) << "\" in model" << endl;
      return 4;
    }
  }

  if( ! prog_opts.norun ) {
    cout << "Starintg run: " << endl;
    int rc = model->run_bg();
    cout << "End run, rc= " << rc << endl;

    if( rc ) {
      for( auto nm : prog_opts.out_vars ) {
        QString v = model->getOutValue( nm );
        cout << qP(nm) << " = " << qP( v ) << endl;
      };

      for( auto nm : prog_opts.out_plots ) {
        QStringList pc = nm.split( ":" );
        if( pc.size() < 1 ) { continue; }
        QString gname = pc[0];
        QString gfile = gname + "_out.png";
        if( pc.size() > 1 ) {
          gfile = pc[1];
        }
        model->plotToPng( gname, gfile );
      };
    }
  }

  if( prog_opts.mod_scr ) {
    if( prog_opts.dbg > 0 ) {
      cerr << "Running model script" << endl;
    }
    QString o = model->runModelScript();
    cerr << "Model script result: \"" << qP(o) << "\"" << endl;
    if( prog_opts.exit_st ) {
      rc = (int)( o.toDouble() ); // to allow narrowing
    }
  }

  for( auto f: prog_opts.s_files ) {
    if( prog_opts.dbg > 0 ) {
      cerr << "Executing script file " << qP(f) << endl;
    }
    f.prepend( "scripts:" );
    if( QFile::exists( f ) ) {
      QFile sf( f );
      if( ! sf.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        cerr << "Fail to open file " << qP(f) << endl;
        continue; // TODO: may be exit?
      }
      QByteArray scr = sf.readAll();
      if( ! scr.isEmpty() ) {
        QString o = model->runScript( scr );
        cerr << "Script result: \"" << qP(o) << "\"" << endl;
        if( prog_opts.exit_st ) {
          rc = (int)( o.toDouble() ); // to allow narrowing
        }
      }
    }
  }

  if( ! prog_opts.script.isEmpty() ) {
    QString o = model->runScript( prog_opts.script );
    cerr << "Script result: \"" << qP(o) << "\"" << endl;
    if( prog_opts.exit_st ) {
      rc = (int)( o.toDouble() ); // to allow narrowing
      if( prog_opts.dbg > 0 ) {
        cerr << "Script return value: " << rc <<  endl;
      }
    }
  }

  if( prog_opts.dbg > 0 ) {
    cerr << "Batch return value: " << rc << " exit_st: " <<  prog_opts.exit_st << endl;
  }

  return rc;
}

// end of main.cpp

