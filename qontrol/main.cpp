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
QString log_app;
QTextStream *os = nullptr;

void print_usage( const char *appname );
int  convert_model( const char *fn_old, const char *fn_new );
int batch_process( const char *model_file );
void appMsgOut( QtMsgType type, const QMessageLogContext &ctx, const QString &msg );
void initAppDirs();

int main( int argc, char *argv[] )
{
  char *fn_new = nullptr;
  setenv( "LC_NUMERIC", "C", 1 );

  QApplication a( argc, argv );
  a.setApplicationName( PACKAGE );
  a.setApplicationDisplayName( PACKAGE );
  a.setApplicationVersion( VERSION );

  qInstallMessageHandler( appMsgOut );

  initAppDirs();

  int op;
  while( ( op=getopt( argc, argv, "hvbeNMc:g:o:s:u:x:I:L:X:d::" ) ) != -1 ) {
    switch( op ) {
      case 'h' : print_usage( argv[0] ); return 0;
      case 'v' : cout << PACKAGE << ' ' << VERSION << endl;  return 0;
      case 'b': prog_opts.batch = true; break;
      case 'e': prog_opts.exit_st = true; break; // convert scripts result to int -> status
      case 'N': prog_opts.norun = true; break;   // no run, only load (for batch)
      case 'M': prog_opts.mod_scr = true; break; // run model script (for batch)
      case 'c': fn_new = optarg; prog_opts.batch = true; break; // convert to this
      case 'g': prog_opts.out_plots << L8B( optarg ); break; // see defs.h
      case 'o': prog_opts.out_file = L8B( optarg ); break;
      case 's': prog_opts.sim_name = L8B( optarg ); break;
      case 'u': prog_opts.out_vars << L8B( optarg ); break;
      case 'x': prog_opts.script = L8B( optarg ); break;
      case 'X': prog_opts.s_files << L8B( optarg ); break;
      case 'I': prog_opts.inc_dirs << L8B( optarg ); break;
      case 'L': prog_opts.lib_dirs << L8B( optarg ); break;
      case 'd': if( optarg ) {
                  prog_opts.dbg = atoi( optarg );
                } else {
                  ++prog_opts.dbg;
                };
                break;
      default: cerr << "Error: unknown option '" << (char)(optopt) << "'" << endl;
               return 1;
    }
  }

  QFile out_file; QTextStream out_stream;
  if( ! prog_opts.out_file.isEmpty() ) {
    out_file.setFileName( prog_opts.out_file );
    if( out_file.open( QIODevice::WriteOnly | QIODevice::Text ) ){
      out_stream.setDevice( &out_file );
      os = &out_stream;
    }

  }

  QDir::setSearchPaths( LIB_DIR,    prog_opts.lib_dirs );
  QDir::setSearchPaths( SCRIPT_DIR, prog_opts.inc_dirs );

  if( ! prog_opts.batch ) { // process with interface
    auto main_win = new LaboWin();

    main_win->show();

    for( int i=optind; i<argc ; ++i ) {
      QString fn = QString::L8B( argv[i] );
      if( prog_opts.dbg>0 ) {
        qDebug() << "Try to open file" << fn << WHE;
      }
      main_win->doFileOpen( fn );
    }

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
  if( ! doc.openDocument( L8B( fn_old ) ) ) {
    cerr << "Fail to read file \"" << fn_old << "\"" << endl;
    return 3;
  }
  doc.setPathName( L8B( fn_new ) );
  if( ! doc.saveDocument( false ) ) {
    cerr << "Fail to save file \"" << fn_new << "\"" << endl;
    return 3;
  }
  return 0;
}

int batch_process( const char *model_file )
{
  int rc = 0;
  LaboDoc doc;
  if( ! doc.openDocument( L8B( model_file ) ) ) {
    qCritical() << "Fail to read file " << model_file << WHE;
    return 3;
  }
  TModel *model = doc.getRoot()->getObjT<TModel*>( "model" );
  if( !model ) {
    qCritical() << "Not found model in file" << model_file << WHE;
    return 5;
  }

  if( ! prog_opts.sim_name.isEmpty() ) {
    if( ! model->setActiveSimul( prog_opts.sim_name ) ) {
      qCritical() << "Not found simulation " << prog_opts.sim_name << " in model" << WHE;
      return 4;
    }
  }

  if( ! prog_opts.norun ) {
    if( prog_opts.dbg > 0 ) {
      qDebug() << "Starintg run: ";
    }
    int rc = model->run_bg();
    cerr << "End run, rc= " << rc << endl;

    if( rc ) {
      for( auto nm : prog_opts.out_vars ) {
        QString v = model->getOutValue( nm );
        cout << qP(nm) << " = " << qP( v ) << endl;
        if( os ) {
          *os << nm << " = " << v << endl;
        }
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
    QString o = model->runFileScript( f );
    cerr << "Script result: \"" << qP(o) << "\"" << endl;
    if( prog_opts.exit_st ) {
      rc = (int)( o.toDouble() ); // to allow narrowing
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

void appMsgOut( QtMsgType type, const QMessageLogContext & /*ctx*/, const QString &msg )
{
  static const char *msgStr[] = { "dbg: ", "warn:", "err: ", "Fatal!", "??5", "??6" };
  if( type > QtFatalMsg ) {
    type = QtCriticalMsg;
  }
  QByteArray lm = QByteArray(msgStr[type]) + ' ' + msg.toLocal8Bit() + '\n';
  //      + "(" + ctx.file + ":" + QByteArray::number(ctx.line) + ", " + ctx.function + '\n';
  cerr << lm.constData();
  log_app.append( lm );
  if( os ) {
    *os << lm.constData();
  }
  if( type == QtFatalMsg ) {
    abort();
  }
}

void initAppDirs()
{

  prog_opts.lib_dirs << QSL( "." );
  prog_opts.lib_dirs << QSL( "./" LIB_DIR );

  prog_opts.inc_dirs << QSL( "." );
  prog_opts.inc_dirs << QSL( "./" SCRIPT_DIR );

  QDir tst_dir;

  // "~/.local/share/qontrol/scripts"
  // "~/.local/share/xdg-app/exports/share/qontrol/scripts"
  // "/var/xdg-app/exports/share/qontrol/scripts"
  // "/usr/local/share/qontrol/scripts"
  // "/usr/share/qontrol/scripts"
  auto dataDirs = QStandardPaths::standardLocations( QStandardPaths::AppLocalDataLocation );
  for( auto d: dataDirs ) {
    QString s = d % QSL( "/" LIB_DIR );
    if( tst_dir.exists( s ) ) {
       prog_opts.lib_dirs << s;
    }
    s = d % QSL( "/" SCRIPT_DIR );
    if( tst_dir.exists( s ) ) {
      prog_opts.inc_dirs << s;
    }
  }
}

// end of main.cpp

