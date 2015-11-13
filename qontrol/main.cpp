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

#include "labowin.h"
#include "labodoc.h"

using namespace std;

ProgOpts prog_opts;
QString log_app;
QTextStream *os = nullptr;

bool parse_cmdline( QApplication &app );
int  convert_model( const QString &fn_old, const QString &fn_new );
int batch_process( const QString &model_file );
void appMsgOut( QtMsgType type, const QMessageLogContext &ctx, const QString &msg );
void initAppDirs();

int main( int argc, char *argv[] )
{
  setenv( "LC_NUMERIC", "C", 1 );

  QApplication a( argc, argv );
  a.setApplicationName( PACKAGE );
  a.setApplicationDisplayName( PACKAGE );
  a.setApplicationVersion( VERSION );

  qInstallMessageHandler( appMsgOut );

  initAppDirs();

  parse_cmdline( a );

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

    for( auto fn : prog_opts.models ) {
      if( prog_opts.dbg>0 ) {
        qDebug() << "Try to open file" << fn << WHE;
      }
      main_win->doFileOpen( fn );
    }

    return a.exec();
  }


  // batch processing -- the only filename requires
  if( prog_opts.models.size() != 1 ) { // only one positional allowed here
    cerr << "Single input filename required for batch procession, "
         << prog_opts.models.size() << " is given" << endl;
    for( auto fn : prog_opts.models ) {
      cerr << qP( fn ) << endl;
    }
    return 2;
  }
  QString in_file = prog_opts.models[0];

  if( ! prog_opts.fn_new.isEmpty() ) { // conversion requested
    return convert_model( in_file, prog_opts.fn_new );
  }

  return batch_process( in_file );

}


int  convert_model( const QString &fn_old, const QString &fn_new )
{
  LaboDoc doc;
  if( ! doc.openDocument( fn_old ) ) {
    cerr << "Fail to read file \"" << qP(fn_old) << "\"" << endl;
    return 3;
  }
  doc.setPathName( fn_new );
  if( ! doc.saveDocument( false ) ) {
    cerr << "Fail to save file \"" << qP(fn_new) << "\"" << endl;
    return 3;
  }
  return 0;
}

int batch_process( const QString &model_file )
{
  int rc = 0;
  LaboDoc doc;
  if( ! doc.openDocument( model_file ) ) {
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

bool parse_cmdline( QApplication &app )
{
  QCommandLineParser prs;
  prs.setApplicationDescription( QSL( PACKAGE " - dynamic systems simulation application" ) );
  prs.addOptions( {
      { QSL("b"), QSL("Batch file procession") },
      { QSL("c"), QSL("converts file to current format (adds -b)"), QSL("out_file") },
      { QSL("d"), QSL("debug level"), QSL("N") },
      { QSL("e"), QSL("convert script Exit status to program exit status") },
      { QSL("g"), QSL("output graph file after run..."), QSL("graph[:file_name]")  },
      { QSL("I"), QSL("add scripts include dir..."), QSL("directory") },
      { QSL("L"), QSL("add model libs dir..."), QSL("directory") },
      { QSL("M"), QSL("Run model script (for batch run)") },
      { QSL("N"), QSL("No-Run - only load (for batch run)") },
      { QSL("o"), QSL("messages output file"), QSL("file") },
      { QSL("P"), QSL("dump graph to file... (TODO)"), QSL("graph[:file_name]") },
      { QSL("p"), QSL("dump output array to file... (TODO)"), QSL("out[:file_name]")  },
      { QSL("s"), QSL("simulation name (for batch run)"), QSL("simulation"), QSL("sim0")  },
      { QSL("u"), QSL("output after run (for batch run)..."), QSL("var") },
      { QSL("X"), QSL("execute files (for batch run)..."), QSL("file") },
      { QSL("x"), QSL("JS code to execute (for batch run)"), QSL("script")  },
  } );
  prs.addHelpOption();
  prs.addVersionOption();

  prs.process( app );

  if( prs.isSet( QSL("b") ) ) {  prog_opts.batch = true;  }

  prog_opts.fn_new = prs.value( QSL("c") );
  if( !prog_opts.fn_new.isEmpty() ) {
    prog_opts.batch = true;
  }

  prog_opts.dbg = prs.value( QSL("d") ).toInt();

  if( prs.isSet( QSL("e") ) ) { prog_opts.exit_st = true;  }

  prog_opts.out_plots = prs.values( QSL("g") );
  prog_opts.inc_dirs  += prs.values( QSL("I") );
  prog_opts.lib_dirs  += prs.values( QSL("L") );

  if( prs.isSet( QSL("M") ) ) {  prog_opts.mod_scr= true;  }

  if( prs.isSet( QSL("N") ) ) {  prog_opts.norun = true;   }

  prog_opts.out_file = prs.value( QSL("o") );

  prog_opts.grdata_outs  = prs.values( QSL("P") );
  prog_opts.out_outs  = prs.values( QSL("p") );

  prog_opts.sim_name = prs.value( QSL("s") );

  prog_opts.out_vars  = prs.values( QSL("u") );
  prog_opts.s_files   = prs.values( QSL("X") );

  prog_opts.script  = prs.value( QSL("x") );

  prog_opts.models = prs.positionalArguments();

  if( prog_opts.dbg > 0 ) {
    cerr << "dbg = " << prog_opts.dbg << endl;
    cerr << "Found options:" << endl;
    for( auto nm : prs.optionNames() ) {
      cerr << qP(nm) << ' ';
    }
    cerr << endl;
  }

  return true;
}

// end of main.cpp

