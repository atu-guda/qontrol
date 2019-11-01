/***************************************************************************
     scriptdialog.cpp - dialog to edit and run scripts
                             -------------------
    begin                : 2016.10.25
    copyright            : GPL (C) 2016-2019 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "defs.h"
#include "scriptdialog.h"

#include <Qsci/qsciapis.h>

using namespace std;

ScriptDialog::ScriptDialog( QString &_scr, TModel *_model, QWidget *_parent )
     : QDialog( _parent ), scr0( _scr ),scr( _scr ), scr00( _scr ), model( _model )
{
  setupUi();
}



void ScriptDialog::setupUi()
{
  setWindowTitle( QSL("Script:") );
  auto em = QFontMetrics( font() ).width( 'W' );

  resize( 80*em, 60*em );
  auto lay_v = new QHBoxLayout( this );

  if( !model ) {
    auto lbl = new QLabel( QSL("No model detected"), this );
    lay_v->addWidget( lbl );
    return;
  }


  auto lay_h = new QVBoxLayout;
  lay_v->addLayout( lay_h );

  sced = new QsciScintilla( this );
  // TODO: common
  sced->setUtf8( true );
  sced->setCaretLineVisible( true );
  sced->setCaretLineBackgroundColor( QColor(QSL("gainsboro")) );
  sced->setAutoIndent( true );
  sced->setIndentationGuides( false );
  sced->setIndentationsUseTabs(false);
  sced->setIndentationWidth( 2 );
  // sced->setMarginsBackgroundColor( QColor("gainsboro") );
  // sced->setMarginLineNumbers( 1, true );
  // sced->setMarginWidth( 1, 50 );
  sced->setAutoCompletionSource( QsciScintilla::AcsAll );
  // sced->setAutoCompletionCaseSensitivity( true );
  // sced->setAutoCompletionReplaceWord( true );
  // sced->setAutoCompletionUseSingle( QsciScintilla::AcusAlways );
  sced->setAutoCompletionThreshold( 2 );
  sced->setBraceMatching( QsciScintilla::SloppyBraceMatch );
  sced->setMatchedBraceBackgroundColor( Qt::yellow );
  sced->setUnmatchedBraceForegroundColor( Qt::blue );
  sced->markerDefine( QsciScintilla::MarkerSymbol::Circle, 1 );
  sced->setMarkerBackgroundColor( Qt::red, 1 );

  auto lexer =  new QsciLexerJavaScript( sced );
  QsciAPIs *apis = new QsciAPIs( lexer );

  auto words = model->getScriptNames();
  for( const auto &w : words ) {
    apis->add( w );
  }

  apis->prepare();
  sced->setLexer( lexer );

  sced->setText( scr );

  lay_h->addWidget( sced, 4 );

  outed = new QTextEdit( this );
  outed->setReadOnly( true );
  lay_h->addWidget( outed, 2 );

  erred = new QTextEdit( this );
  erred->setReadOnly( true );
  lay_h->addWidget( erred, 1 );

  auto bbox = new QDialogButtonBox( Qt::Vertical, this );
  lay_v->addWidget( bbox );

  auto btRun = bbox->addButton( QSL("&Run"), QDialogButtonBox::ActionRole );
  connect( btRun, &QPushButton::pressed, this, &ScriptDialog::run );

  auto btClose = bbox->addButton( QSL("&Close"), QDialogButtonBox::ActionRole );
  connect( btClose, &QPushButton::pressed, this, &ScriptDialog::reject );

  auto btRunMdl = bbox->addButton( QSL("Run model scr"), QDialogButtonBox::ActionRole );
  connect( btRunMdl, &QPushButton::pressed, this, &ScriptDialog::runModelScript );

  auto btInitEng = bbox->addButton( QSL("Init engine"), QDialogButtonBox::ActionRole );
  connect( btInitEng, &QPushButton::pressed, this, &ScriptDialog::initEngine );

  auto btClearScript = bbox->addButton( QSL("clear Script"), QDialogButtonBox::ActionRole );
  connect( btClearScript, &QPushButton::pressed, this, &ScriptDialog::clearScript );

  auto btClearOutput = bbox->addButton( QSL("clear Output"), QDialogButtonBox::ActionRole );
  connect( btClearOutput, &QPushButton::pressed, this, &ScriptDialog::clearOutput );

  auto btSave = bbox->addButton( QSL("Save"), QDialogButtonBox::ActionRole );
  connect( btSave, &QPushButton::pressed, this, &ScriptDialog::save );

  auto btReload = bbox->addButton( QSL("Reload"), QDialogButtonBox::ActionRole );
  connect( btReload, &QPushButton::pressed, this, &ScriptDialog::reload );

  auto btRevert = bbox->addButton( QSL("Revert"), QDialogButtonBox::ActionRole );
  connect( btRevert, &QPushButton::pressed, this, &ScriptDialog::revert );
}

void ScriptDialog::accept()
{
  QDialog::accept();
}

void ScriptDialog::runScr( const QString &s )
{
  ScriptResult sres;
  int rc = model->runScript( s, &sres );
  errstr = QSL("== rc= " ) % QSN( rc ) % QSL("\n");
  if( sres.err_line != 0 ) {
    errstr += sres.err % QSL(" Line: ") % QSN( sres.err_line ) % QSL("\n" );
    errstr += sres.bt.join( '\n' );
    sced->markerAdd( sres.err_line-1, 1 );
  } else {
    out += sres.str % QSL("\n");
    sced->markerDeleteAll(-1 );
  }

  // sced->annotate( 1, QSL("Annontation"), 0 );
  outed->setText( out );
  erred->setText( errstr );
}


void ScriptDialog::run()
{
  scr = sced->text();
  runScr( scr );
}

void ScriptDialog::runModelScript()
{
  QString s = model->getDataD( QSL("script"), QSL("") );
  runScr( s );
}

void ScriptDialog::initEngine()
{
  model->initEngine();
}


void ScriptDialog::clearScript()
{
  scr = QString();
  sced->setText( scr );
}

void ScriptDialog::clearOutput()
{
  out = QString();
  outed->setText( out );
}

void ScriptDialog::save()
{
  scr = sced->text();
  scr0 = scr;
}

void ScriptDialog::reload()
{
  scr = scr0;
  sced->setText( scr );
}

void ScriptDialog::revert()
{
  scr = scr00;
  sced->setText( scr );
}


