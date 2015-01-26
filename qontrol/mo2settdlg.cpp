#include <QtWidgets>

#include "mo2settdlg.h"


Mo2SettDlg::Mo2SettDlg( Mo2Settings &se,  QWidget* parent )
    : QDialog( parent ),
      pse(se), ts(se)
{
  setWindowTitle( tr( "Settings" ) );

  lay = new QGridLayout;

  btnMainFont = new QPushButton( tr( "&Main Font..." ), this );
  lay->addWidget( btnMainFont, 0, 0 );

  btnSmallFont = new QPushButton(  tr( "&Small Font..." ), this );
  lay->addWidget( btnSmallFont, 1, 0 );

  btnPlotFont = new QPushButton( tr( "&Plot Font..." ), this );
  lay->addWidget( btnPlotFont, 2, 0 );

  btnStructFont = new QPushButton( tr( "Str&ucture Font..." ), this );
  lay->addWidget( btnStructFont, 3, 0 );

  chkMaximize = new QCheckBox( tr( "Ma&ximize at start" ), this );
  chkMaximize->setChecked(ts.showmax);
  lay->addWidget( chkMaximize, 0, 1 );

  auto lbl = new QLabel( "Editor cmd" );
  lay->addWidget( lbl, 4, 0 );
  ed_editCmd = new QLineEdit( this );
  ed_editCmd->setText( ts.editCmd );
  lay->addWidget( ed_editCmd, 4, 1 );

  auto bbox
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, 6, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::accepted, this, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, this, &QDialog::reject );

  setLayout( lay );

  connect( btnMainFont,   &QPushButton::clicked, this, &Mo2SettDlg::mainFontSlot );
  connect( btnSmallFont,  &QPushButton::clicked, this, &Mo2SettDlg::smallFontSlot );
  connect( btnPlotFont,   &QPushButton::clicked, this, &Mo2SettDlg::mainFontSlot );
  connect( btnStructFont, &QPushButton::clicked, this, &Mo2SettDlg::structFontSlot );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Mo2SettDlg::~Mo2SettDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

void Mo2SettDlg::accept()
{
  ts.showmax = chkMaximize->isChecked();
  ts.editCmd = ed_editCmd->text();
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

