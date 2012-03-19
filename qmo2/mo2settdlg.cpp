#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qfontdialog.h>
#include <QGridLayout>

#include "mo2settdlg.h"


Mo2SettDlg::Mo2SettDlg( Mo2Settings &se,  QWidget* parent )
    : QDialog( parent, "Mo2SettDlg", true, 0 ),
      pse(se), ts(se)
{
    lay = new QGridLayout; 

    btnMainFont = new QPushButton( this );
    lay->addWidget( btnMainFont, 0, 0 );

    btnSmallFont = new QPushButton( this );
    lay->addWidget( btnSmallFont, 1, 0 );

    btnPlotFont = new QPushButton( this );
    lay->addWidget( btnPlotFont, 2, 0 );

    btnStructFont = new QPushButton( this );
    lay->addWidget( btnStructFont, 3, 0 );

    chkMaximize = new QCheckBox( this );
    chkMaximize->setChecked(ts.showmax);
    lay->addWidget( chkMaximize, 0, 1 );

    btnOK = new QPushButton( this );
    btnOK->setDefault( TRUE );
    lay->addWidget( btnOK, 4, 0 );

    btnCancel = new QPushButton( this );
    lay->addWidget( btnCancel, 4, 1 );
    languageChange();
    setLayout( lay );

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

