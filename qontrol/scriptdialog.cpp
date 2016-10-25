/***************************************************************************
     scriptdialog.cpp - dialog to edit and run scripts
                             -------------------
    begin                : 2016.10.25
    copyright            : GPL (C) 2016-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "defs.h"
#include "scriptdialog.h"

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

  auto lay_h = new QVBoxLayout;
  lay_v->addLayout( lay_h );

  sced = new QTextEdit( this );
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

  auto btRun = bbox->addButton( "&Run", QDialogButtonBox::ActionRole );
  connect( btRun, &QPushButton::pressed, this, &ScriptDialog::run );

  auto btClose = bbox->addButton( "&Close", QDialogButtonBox::ActionRole );
  connect( btClose, &QPushButton::pressed, this, &ScriptDialog::reject );

  auto btClearScript = bbox->addButton( "clear Script", QDialogButtonBox::ActionRole );
  connect( btClearScript, &QPushButton::pressed, this, &ScriptDialog::clearScript );

  auto btClearOutput = bbox->addButton( "clear Output", QDialogButtonBox::ActionRole );
  connect( btClearOutput, &QPushButton::pressed, this, &ScriptDialog::clearOutput );

  auto btSave = bbox->addButton( "Save", QDialogButtonBox::ActionRole );
  connect( btSave, &QPushButton::pressed, this, &ScriptDialog::save );

  auto btReload = bbox->addButton( "Reload", QDialogButtonBox::ActionRole );
  connect( btReload, &QPushButton::pressed, this, &ScriptDialog::reload );

  auto btRevert = bbox->addButton( "Revert", QDialogButtonBox::ActionRole );
  connect( btRevert, &QPushButton::pressed, this, &ScriptDialog::revert );
}

void ScriptDialog::accept()
{
  QDialog::accept();
}

void ScriptDialog::run()
{
  errstr = QString();
  if( !model ) {
    errstr = QSL("No model detected!");
    update();
    return;
  }
  scr = sced->toPlainText();
  out = model->runScript( scr );
  outed->setText( out );
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
  scr = sced->toPlainText();
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


