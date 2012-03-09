#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qfontdialog.h>

#include "mo2settdlg.h"


Mo2SettDlg::Mo2SettDlg( Mo2Settings &se,  QWidget* parent )
    : QDialog( parent, "Mo2SettDlg", true, 0 ),
      pse(se), ts(se)
{
    Mo2SettDlgLayout = new QVBoxLayout( this, 11, 6, "Mo2SettDlgLayout"); 

    layMain = new QHBoxLayout( 0, 0, 6, "layMain"); 

    layFontBox = new QVBoxLayout( 0, 0, 6, "layFontBox"); 

    btnMainFont = new QPushButton( this, "btnMainFont" );
    layFontBox->addWidget( btnMainFont );

    btnSmallFont = new QPushButton( this, "btnSmallFont" );
    layFontBox->addWidget( btnSmallFont );

    btnPlotFont = new QPushButton( this, "btnPlotFont" );
    layFontBox->addWidget( btnPlotFont );

    btnStructFont = new QPushButton( this, "btnStructFont" );
    layFontBox->addWidget( btnStructFont );
    layMain->addLayout( layFontBox );

    layChecks = new QVBoxLayout( 0, 0, 6, "layChecks"); 

    chkMaximize = new QCheckBox( this, "chkMaximize" );
    chkMaximize->setChecked(ts.showmax);
    layChecks->addWidget( chkMaximize );
    spacer1 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layChecks->addItem( spacer1 );
    layMain->addLayout( layChecks );
    Mo2SettDlgLayout->addLayout( layMain );
    spacer2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Mo2SettDlgLayout->addItem( spacer2 );

    layOkCanc = new QHBoxLayout( 0, 0, 6, "layOkCanc"); 

    btnOK = new QPushButton( this, "btnOK" );
    btnOK->setDefault( TRUE );
    layOkCanc->addWidget( btnOK );

    btnCancel = new QPushButton( this, "btnCancel" );
    layOkCanc->addWidget( btnCancel );
    Mo2SettDlgLayout->addLayout( layOkCanc );
    languageChange();
    clearWState( WState_Polished );

    // signals and slots connections
    connect( btnOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( btnMainFont, SIGNAL( clicked() ), this, SLOT( mainFontSlot() ) );
    connect( btnSmallFont, SIGNAL( clicked() ), this, SLOT( smallFontSlot() ) );
    connect( btnPlotFont, SIGNAL( clicked() ), this, SLOT( mainFontSlot() ) );
    connect( btnStructFont, SIGNAL( clicked() ), this, SLOT( structFontSlot() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Mo2SettDlg::~Mo2SettDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Mo2SettDlg::languageChange()
{
    setCaption( tr( "Settings" ) );
    btnMainFont->setText( tr( "&Main Font..." ) );
    btnMainFont->setAccel( QKeySequence( tr( "Alt+M" ) ) );
    btnSmallFont->setText( tr( "&Small Font..." ) );
    btnSmallFont->setAccel( QKeySequence( tr( "Alt+S" ) ) );
    btnPlotFont->setText( tr( "&Plot Font..." ) );
    btnPlotFont->setAccel( QKeySequence( tr( "Alt+P" ) ) );
    btnStructFont->setText( tr( "Str&ucture Font..." ) );
    btnStructFont->setAccel( QKeySequence( tr( "Alt+U" ) ) );
    chkMaximize->setText( tr( "Ma&ximize at start" ) );
    chkMaximize->setAccel( QKeySequence( tr( "Alt+X" ) ) );
    btnOK->setText( tr( "&OK" ) );
    btnOK->setAccel( QKeySequence( tr( "Alt+O" ) ) );
    btnCancel->setText( tr( "Cancel" ) );
    btnCancel->setAccel( QKeySequence( tr( "Esc" ) ) );
}

void Mo2SettDlg::accept()
{
  ts.showmax = chkMaximize->isChecked();
  pse = ts;
  QDialog::accept();
}

void Mo2SettDlg::mainFontSlot()
{
  bool ok;
  QFont of;
  of.fromString( ts.mainFont );
  QFont mf = QFontDialog::getFont( &ok, of );
  if( ok ) {
    ts.mainFont = mf.toString();
  }
}

void Mo2SettDlg::smallFontSlot()
{
  bool ok;
  QFont of;
  of.fromString( ts.smallFont );
  QFont mf = QFontDialog::getFont( &ok, of );
  if( ok ) {
    ts.smallFont = mf.toString();
  }
}

void Mo2SettDlg::plotFontSlot()
{
  bool ok;
  QFont of;
  of.fromString( ts.plotFont );
  QFont mf = QFontDialog::getFont( &ok, of );
  if( ok ) {
    ts.plotFont = mf.toString();
  }
}

void Mo2SettDlg::structFontSlot()
{
  bool ok;
  QFont of;
  of.fromString( ts.structFont );
  QFont mf = QFontDialog::getFont( &ok, of );
  if( ok ) {
    ts.structFont = mf.toString();
  }
}

