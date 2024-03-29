/***************************************************************************
                          mglview.cpp - window to show plots via MathGL
                             -------------------
    begin                : 2014.11.29
    copyright            : (C) 2014-2022 by atu
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

#include <QtWidgets>
#include <QString>
#include <QPrinter>
#include <QPrintDialog>

#include "miscfun.h"
#include "prog_opts.h"
#include "labowin.h"
#include "toutarr.h"
#include "tgraph.h"
#include "tmodel.h"
#include "datawidget.h"

#include "mglview.h"

using namespace std;
// ------------------------- MglView ------------------------

MglView::MglView( TGraph *agra, QWidget *parent )
          : QWidget( parent ),
          gra( agra ),
          scd ( make_shared<ScaleData>( QSL("scd"), nullptr, 0, QSL("scale"), QSL("current scale data") ) ),
          pa_fnt ( SETTINGS->getAsFont( QSL("plotFont") ) )
{
  QFontMetrics fm( pa_fnt );
  em = fm.horizontalAdvance( QSL("W") );
  ex = fm.height();
  bottom_h = 4 * ex;

  Reload();
  // QSize sz0 = getSize0();
  // resize( sz0 + QSize( 0, bottom_h ) );

  setMinimumSize( 480, 420 );
  setCursor( Qt::CrossCursor );
  setFocusPolicy( Qt::StrongFocus );
}

MglView::~MglView()
{
  resetData();
  gra = nullptr; // not delete, we are not owner, just for debug
}


void MglView::resetData()
{
  data_loaded = false;
  vd.sel = 0;
  linkPlot = -1; linkIdx = 0;
}

QSize MglView::getSize0() const
{
  return QSize( scd->w0, scd->h0 );
}


void MglView::Reload( )
{
  resetData();

  if( !gra ) {
    qWarning() << "no TGraph passed to MglDrawer" << WHE;
    return;
  }

  scd_o =  gra->getObjT<ScaleData*>( QSL("scd") );
  if( scd_o ) {
    QString s = scd_o->toString();// terrble copy
    scd->fromString( s );
  }
  gra->prepare();
  if( ! gra->fillViewData( &vd ) ) {
    qWarning() << "Fail to fillViewData " << WHE;
    return;
  }
  pr_min = vd.pv_min; pr_max = vd.pv_max; pr_dlt = vd.pv_dlt;
  data_loaded = true;
}


void MglView::zoom()
{
  if( !data_loaded ) {
    return;
  }

  mglPoint p0 = scd->getBase();
  mglPoint p1 = scd->getMark();
  gra->viewForPoints( p0, p1, vd );
  update();
}

void MglView::zoomReset()
{
  if( !data_loaded ) {
    return;
  }
  vd.reset();

  update();
}


QSize MglView::sizeHint() const
{
  return getSize0() + QSize( 0, show_footer ? bottom_h : 0 );
}

void MglView::drawAll( QPainter &p )
{
  wg = width();
  h_tot = height();
  hg = h_tot;
  if( show_footer ) {
    hg -= bottom_h;
  }
  QImage timg( wg, hg, QImage::Format_RGB32 );
  // qWarning() << "renderTo: " << wg  << " x " << hg << WHE;
  gsz = gra->renderTo( timg, &vd, scd.get() );

  p.drawImage( 0, 0, timg );

  if( show_footer ) {
    drawFooter( p );
  }
}

void MglView::paintEvent( QPaintEvent * /*pe*/ )
{
  QPainter p( this );
  drawAll( p );
  p.end();
}

void MglView::drawFooter( QPainter &p )
{
  p.setFont( pa_fnt );
  QColor bg_c = QColor( scd->bgcolor );
  QColor fg_c = QColor( 0,0,0 );
  if( bg_c.value() < 128 ) {
    fg_c = QColor( 255, 255, 255 );
  }

  p.fillRect( 0, hg, wg, bottom_h, QBrush( bg_c ) );
  p.setPen( fg_c );
  p.drawLine( 0, hg, wg, hg );

  QString s = getInfo();

  p.drawText( ex, hg, wg-2*ex, bottom_h, Qt::AlignLeft, s );
}

void MglView::mousePressEvent( QMouseEvent *me )
{
  if( !data_loaded ) {
    return QWidget::mousePressEvent( me );
  }

  mouse_x = me->x(); mouse_y = me->y();
  int btn = me->button();
  mglPoint po = gra->CalcXYZ( mouse_x, mouse_y, wg, hg, &vd, scd.get() );
  switch( btn ) {
    case Qt::LeftButton:
      unlinkFromPlot();
      scd->setMark( po );
      mark_screen_x = mouse_x; mark_screen_y = mouse_y;
      update();
      break;
    case Qt::RightButton:
      {
        QMenu *menu = createPopupMenu();
        menu->exec( mapToGlobal( QPoint( mouse_x, mouse_y ) ) );
        delete menu;
      }
      break;
    case Qt::MiddleButton:
      return QWidget::mousePressEvent( me );
    default:
      return QWidget::mousePressEvent( me );
  };

}

QMenu* MglView::createPopupMenu()
{
  auto menu = new QMenu( this );

  QAction *act;
  act = menu->addAction( QSL("Help (F1)") );
  act->setShortcut( Qt::Key_F1 );
  connect( act, &QAction::triggered, this, &MglView::showHelp );
  menu->addSeparator();

  act = menu->addAction( QSL("Reset scale (&r)") );
  connect( act, &QAction::triggered, this, &MglView::resetScale );
  act = menu->addAction( QSL("set Base (&b)") );
  connect( act, &QAction::triggered, this, &MglView::markToBase );
  menu->addSeparator();

  act = menu->addAction( QSL("Zoom base-mark (&z)") );
  connect( act, &QAction::triggered, this, &MglView::zoom );
  act = menu->addAction( QSL("reset zoom (Z)") );
  connect( act, &QAction::triggered, this, &MglView::zoomReset );
  act = menu->addAction( QSL("Set scale (&s)") );
  connect( act, &QAction::triggered, this, &MglView::setScale );
  act = menu->addAction( QSL("Save scale (S)") );
  connect( act, &QAction::triggered, this, &MglView::saveScale );
  menu->addSeparator();

  act = menu->addAction( QSL("Print (&p)") );
  connect( act, &QAction::triggered, this, &MglView::print );
  act = menu->addAction( QSL("Export (&e)") );
  connect( act, &QAction::triggered, this, &MglView::exportPlot );
  act = menu->addAction( QSL("Export data (&d)") );
  connect( act, &QAction::triggered, this, &MglView::exportPlottedData );
  act = menu->addAction( QSL("Info (&i)") );
  connect( act, &QAction::triggered, this, &MglView::showInfo );
  menu->addSeparator();

  act = menu->addAction( QSL("Toggle plot (&o)") );
  connect( act, &QAction::triggered, this, &MglView::togglePlot );
  act = menu->addAction( QSL("Toggle &All plots (O)") );
  connect( act, &QAction::triggered, this, &MglView::toggleAllPlots );
  act = menu->addAction( QSL("Link to plot (&l)") );
  connect( act, &QAction::triggered, this, &MglView::linkToPlot );
  act = menu->addAction( QSL("Toggle labels (&v)") );
  connect( act, &QAction::triggered, this, &MglView::toggleLabels );
  menu->addSeparator();

  act = menu->addAction( QSL("X to clipboard (&x)") );
  connect( act, &QAction::triggered, this, &MglView::setClipboardX );
  act = menu->addAction( QSL("Y to clipboard (&y)") );
  connect( act, &QAction::triggered, this, &MglView::setClipboardY );
  act = menu->addAction( QSL("[X;Y] to clipboard") );
  connect( act, &QAction::triggered, this, &MglView::setClipboardXY );
  act = menu->addAction( QSL("Add label at mark (&t)") );
  connect( act, &QAction::triggered, this, &MglView::addLabel );

  return menu;
}

void MglView::keyPressEvent( QKeyEvent *ke )
{
  constexpr unsigned Sh = 0x40000000;
  constexpr unsigned Ct = 0x80000000;
  unsigned k = ke->key(), st = ke->modifiers();
  if( st & Qt::ShiftModifier ) {
      k |= Sh;
  }
  if( st & Qt::ControlModifier ) {
      k |= Ct;
  }
  // qWarning(  "k= %08X st= %08X, r= %08X", k, st, (Qt::Key_Minus | Sh ) );

  switch( k ) {
    case Qt::Key_F1:
      showHelp();
      break;
    case Qt::Key_I:
      showInfo();
      break;
    case Qt::Key_B:
      markToBase();
      break;
    case Qt::Key_B | Sh:
      scd->drawBase = !scd->drawBase;
      break;
    case Qt::Key_M | Sh:
      scd->drawMark = !scd->drawMark;
      break;
    case Qt::Key_F:
      scd->useLight = !scd->useLight;
      break;
    case Qt::Key_U:
      show_footer = ! show_footer;
      break;
    case Qt::Key_O:
      togglePlot();
      break;
    case Qt::Key_V:
      toggleLabels();
      break;
    case Qt::Key_O | Sh:
      toggleAllPlots();
      break;
    case Qt::Key_Q:
      emit closeMe();
      break;
    case Qt::Key_R:
      resetScale();
      break;
    case Qt::Key_T:
      addLabel();
      break;
    case Qt::Key_R | Sh:
      Reload();
      break;
    case Qt::Key_P | Ct:
      print();
      break;
    case Qt::Key_E:
      exportPlot();
      break;
    case Qt::Key_D:
      exportPlottedData();
      break;
    case Qt::Key_G:
      setMark();
      break;
    case Qt::Key_S:
      setScale();
      break;
    case Qt::Key_S | Sh:
      saveScale();
      break;
    case Qt::Key_A:
      setAlpha( 0.1, true );
      break;
    case Qt::Key_A | Sh :
      setAlpha( -0.1, true );
      break;
    case Qt::Key_X:
      setClipboardX();
      break;
    case Qt::Key_Y:
      setClipboardY();
      break;
    // link/point
    case Qt::Key_L:
      linkToPlot();
      break;
    case Qt::Key_L | Sh :
      unlinkFromPlot();
      break;
    case Qt::Key_BracketLeft:
      nextPointInPlot( -1 );
      break;
    case Qt::Key_BracketRight:
      nextPointInPlot( 1 );
      break;
    case Qt::Key_BraceLeft | Sh:
      nextPointInPlot( -vd.nx / 10 );
      break;
    case Qt::Key_BraceRight | Sh:
      nextPointInPlot( vd.nx / 10 );
      break;

    // Shift
    case Qt::Key_Up:
      vd.ofs.y -= vd.mag.y * scale_step;
      update();
      break;
    case Qt::Key_Down:
      vd.ofs.y += vd.mag.y * scale_step;
      update();
      break;
    case Qt::Key_Left:
      vd.ofs.x += vd.mag.x * scale_step;
      update();
      break;
    case Qt::Key_Right:
      vd.ofs.x -= vd.mag.x * scale_step;
      update();
      break;
    case Qt::Key_PageUp:
      vd.ofs.z -= vd.mag.z * scale_step;
      update();
      break;
    case Qt::Key_PageDown:
      vd.ofs.z += vd.mag.z * scale_step;
      break;

    // Rotate
    case Qt::Key_Up | Sh:
      setPhi( angle_step, true );
      break;
    case Qt::Key_Down | Sh:
      setPhi( -angle_step, true );
      break;
    case Qt::Key_Left | Sh:
      setTheta( angle_step, true );
      break;
    case Qt::Key_Right | Sh:
      setTheta( -angle_step, true );
      break;

    // Move mark
    case Qt::Key_Up | Ct:
      moveMark( 0, -1 );
      break;
    case Qt::Key_Down | Ct:
      moveMark( 0, 1 );
      break;
    case Qt::Key_Left | Ct:
      moveMark( -1, 0 );
      break;
    case Qt::Key_Right | Ct:
      moveMark( 1, 0 );
      break;

    case Qt::Key_Plus:
      setXmag( mag_step, true );
      break;
    case Qt::Key_Minus:
      setXmag( 1.0/mag_step, true );
      break;
    case Qt::Key_Plus | Sh:
      setYmag( mag_step, true );
      break;
    case Qt::Key_Minus | Sh:
    case 0x40002212 : //  = dash on some keyboard layouts
      setYmag( 1.0/mag_step, true );
      break;
    case Qt::Key_Plus | Ct:
      setZmag( mag_step, true );
      break;
    case Qt::Key_Minus | Ct:
      setZmag( 1.0/mag_step, true );
      break;
    case Qt::Key_Z:
      zoom();
      break;
    case Qt::Key_Z | Sh:
      zoomReset();
      break;

    case Qt::Key_0: vd.sel = 0;  break;
    case Qt::Key_1: vd.sel = 1;  break;
    case Qt::Key_2: vd.sel = 2;  break;
    case Qt::Key_3: vd.sel = 3;  break;
    case Qt::Key_4: vd.sel = 4;  break;
    case Qt::Key_5: vd.sel = 5;  break;
    case Qt::Key_6: vd.sel = 6;  break;
    case Qt::Key_7: vd.sel = 7;  break;
    case Qt::Key_8: vd.sel = 8;  break;
    case Qt::Key_9: vd.sel = 9;  break;
    case Qt::Key_BracketLeft  | Ct: --vd.sel; if( vd.sel <  0     ) { vd.sel = 0; }; break;
    case Qt::Key_BracketRight | Ct: ++vd.sel; if( vd.sel >= vd.ng ) { vd.sel = 0; }; break;
  }

  update(); // even on unused keys
}

void MglView::resizeEvent( QResizeEvent *e )
{
  // QSize cs = size();
  // alc_x = cs.width(); alc_y = cs.height();
  // pb.resize( alc_x * alc_y * 4 ); // sub bottom_h ?

  QWidget::resizeEvent( e );
}

void MglView::resetScale()
{
  vd.reset();
  update();
}

void MglView::markToBase()
{
  scd->setBase( scd->getMark() );
  update();
}

void MglView::setPhi( double a_phi, bool add )
{
  if( add ) {
    scd->phi += a_phi;
  } else {
    scd->phi  = a_phi;
  }
  scd->phi = limitAngleDeg( scd->phi );
  update();
}

void MglView::setTheta( double a_theta, bool add )
{
  if( add ) {
    scd->theta += a_theta;
  } else {
    scd->theta  = a_theta;
  }
  scd->theta = limitAngleDeg( scd->theta );
  update();
}

void MglView::setXmag( double amag, bool mul )
{
  if( mul ) {
    vd.mag.x *= amag;
  } else {
    vd.mag.x = amag;
  }
  update();
}

void MglView::setYmag( double amag, bool mul )
{
  if( mul ) {
    vd.mag.y *= amag;
  } else {
    vd.mag.y = amag;
  }
  update();
}

void MglView::setZmag( double amag, bool mul )
{
  if( mul ) {
    vd.mag.z *= amag;
  } else {
    vd.mag.z = amag;
  }
  update();
}


void MglView::setAlpha( double al, bool rel )
{
  if( rel ) {
    scd->alpha += al;
  } else {
    scd->alpha  = al;
  }
  scd->alpha = clamp( scd->alpha.cval(), 0.0, 1.0 );
  update();
}


void MglView::setMark()
{
  auto dia = new QDialog( this );
  dia->setWindowTitle( QSL("Set mark to point: ") );
  auto lay = new QGridLayout( dia );

  auto markX_ed = new QLineEdit( dia );
  markX_ed->setValidator( new QDoubleValidator() );
  markX_ed->setText( QSN( scd->markX ) );
  lay->addWidget( markX_ed, 0, 1 );

  auto markY_ed = new QLineEdit( dia );
  markY_ed->setValidator( new QDoubleValidator() );
  markY_ed->setText( QSN( scd->markY ) );
  lay->addWidget( markY_ed, 1, 1 );

  auto markZ_ed = new QLineEdit( dia );
  markZ_ed->setValidator( new QDoubleValidator() );
  markZ_ed->setText( QSN( scd->markZ ) );
  lay->addWidget( markZ_ed, 2, 1 );

  auto la_x = new QLabel( QSL("&X value"), dia );
  la_x->setBuddy( markX_ed );
  lay->addWidget( la_x, 0, 0 );
  auto la_y = new QLabel( QSL("&Y value"), dia );
  la_y->setBuddy( markY_ed );
  lay->addWidget( la_y, 1, 0 );
  auto la_z = new QLabel( QSL("&Z value"), dia );
  la_z->setBuddy( markZ_ed );
  lay->addWidget( la_z, 2, 0 );

  auto bbox
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, 3, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

  int rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    scd->markX = toDoubleEx( markX_ed->text() );
    scd->markY = toDoubleEx( markY_ed->text() );
    scd->markZ = toDoubleEx( markZ_ed->text() );
  };
  delete dia;
  update();

}

void MglView::setScale()
{
  auto dia = new DataDialog( *scd, this );

  int rc = dia->exec();
  delete dia;
  if( rc == QDialog::Accepted ) {
    zoomReset();
    update();
  };
}

void MglView::saveScale()
{
  if( !scd_o ) {
    return;
  }
  if( QMessageBox::question( this, "Save scale settings",
        "Are you sure to save current scale settings?" )
      == QMessageBox::Yes )
  {
    // terrble copy
    QString s = scd->toString();
    scd_o->fromString( s );
  }
}

void MglView::togglePlot()
{
  if( ! isInBounds( 0, vd.sel, (int)sizeof(vd.off)*8 ) ) {
    return;
  }
  static_assert( sizeof(long long) == sizeof(vd.off) );
  decltype(vd.off) msk = 1ull << vd.sel;
  vd.off ^= msk;

  update();
}

void MglView::toggleAllPlots()
{
  if( ! isInBounds( 0, vd.sel, (int)sizeof(vd.off)*8 ) ) {
    return;
  }
  decltype(vd.off) msk = ~( 1ull << vd.sel );
  vd.off ^= msk;

  update();
}

void MglView::linkToPlot()
{
  if( ! isInBounds( 0, vd.sel, vd.ng ) ) {
    return;
  }

  linkPlot = vd.sel;
  mglPoint mp = scd->getMark();
  linkIdx = gra->findNearest( mp, vd.sel );
  setMarkToLink(); // bound and update included
}

void MglView::unlinkFromPlot()
{
  linkPlot = -1; linkIdx = 0;
  update();
}

void MglView::nextPointInPlot( int step )
{
  if( linkPlot < 0 ) {
    return;
  }
  linkIdx += step;
  setMarkToLink(); // bound and update in it
}

void MglView::toggleLabels()
{
  vd.show_labels = ! vd.show_labels;
  update();
}

void MglView::setClipboardX()
{
  setClipboardStr( QSN( scd->markX ) );
}

void MglView::setClipboardY()
{
  setClipboardStr( QSN( scd->markY ) );
}

void MglView::setClipboardXY()
{
  setClipboardStr( QSL("[ ") % QSN( scd->markX ) % QSL("; ") % QSN(scd->markY) % QSL(" ]") );
}

void MglView::addLabel()
{
  if( ! gra ) {
    return;
  }
  auto mp =  scd->getMark();
  gra->addLabel( mp, QSL(" ($X, $Y)") );
}

void MglView::setMarkToLink()
{
  if( linkPlot < 0 ) {
    return;
  }
  linkIdx = clamp( linkIdx, 0l, vd.nn-1 );
  mglPoint targ;
  if( ! gra->getPointAt( linkPlot, linkIdx, &targ ) ) {
    qWarning() << "unknown point " << linkIdx << "line " << linkPlot << WHE;
    return;
  }
  scd->setMark( targ );
  update();
}


void MglView::print()
{
  if( ! MAINWIN ) {
    return;
  }
  QPrinter *pr = MAINWIN->getPrinter();
  if( !pr ) {
    return;
  }

  QPrintDialog pr_dialog( pr, this );
  if( pr_dialog.exec() ) {
    pr->setFullPage( false );
    pr->newPage();
    QPainter p( pr );
    drawAll( p );
    p.end();
  };

}

void MglView::exportPlot()
{
  QString fn0 = gra->hintFileName() % QSL(".png");
  QString fn = QFileDialog::getSaveFileName( this, QSL("Save picture"), fn0,
               QSL("PNG files (*.png);;All files (*)" ) );
  if( fn.isEmpty() ) {
    return;
  };
  // gra->plotToPng( fn );

  QImage timg( width(), height(), QImage::Format_RGB32 );
  timg.fill( 0xFF );
  gra->renderTo( timg, &vd, scd.get() );

  if( ! timg.save( fn, "PNG", 50 ) ) {
    QString err = strerror( errno );
    HANDLE_ERROR( QSL("Fail fo save picture file: \"%1\": %2").arg( fn, err ) );
  }

}

void MglView::exportPlottedData()
{
  QString fn0 = gra->hintFileName() % QSL(".txt");
  QString fn = QFileDialog::getSaveFileName( this, QSL("Save plotted data"), fn0,
               QSL("Text files (*.txt);;All files (*)") );
  if( fn.isEmpty() ) {
    return;
  };

  gra->dumpPlotted( fn );
}


QString MglView::getInfo( bool more ) const
{
  if( ! data_loaded ) {
    return QSL("No Data");
  }
  QString nl = QSL("\n"), nl1 = QSL(" ");
  if( more ) {
    nl = QSL("<br/>\n"); nl1 = nl;
  }

  mglPoint mark_point = scd->getMark();
  mglPoint base_point = scd->getBase();
  mglPoint rel_p = mark_point - base_point;
  double kyx = 0.0, byx = 0.0, kzx = 0.0, bzx = 0.0;
  if( fabs( rel_p.x )  > D_EPS ) {
    kyx = rel_p.y / rel_p.x;
    byx = mark_point.y - kyx * mark_point.x;
    kzx = rel_p.z / rel_p.x;
    bzx = mark_point.z - kzx * mark_point.x;
  }

  QString s = QSL( "mark: ") % toQString( mark_point )
     % QSL("  base: ") % toQString( base_point ) % nl1
     % QSL("  rel: ") % toQString( rel_p )
     % QSL(" kyx: ") % QSN( kyx ) % QSL( " byx= " ) % QSN( byx )
     % QSL(" kzx: ") % QSN( kzx ) % QSL( " bzx= " ) % QSN( bzx )
     % QSL("  [ ") % QSN( vd.nx ) % QSL(" x ") % QSN( vd.ny ) % QSL(" ]")
     % nl
     % QSL("  mag: ") % toQString( vd.mag ) %  QSL(" ofs: ") % toQString( vd.ofs )
     % nl1
     % QSL("real: ") % toQString( pr_min ) % QSL(" - ") % toQString( pr_max )
     % nl
     % QSN( vd.sel ) % QSL(" ");

  s += gra->getPlotLabel( vd.sel ) % QSL("  ");

  if( linkPlot > -1 ) {
    s += QSL("  Link: ") % QSN( linkPlot) % " "
       % gra->getPlotLabel( linkPlot ) % QSL(" [") % QSN( linkIdx ) % QSL("]  ");
  }
  s += QChar( 0x03C6 ) % QSL(": ") % QSN( scd->phi ) % QSL(" ")
     % QChar( 0x03B8 ) % QSL(": ") % QSN( scd->theta )
     % QSL(" wg= ") % QSN( wg ) % QSL( " hg= " ) % QSN( hg ) % QSL(" h_tot= ") % QSN( h_tot )
     % QSL(" mx= ") % QSN( mouse_x ) % QSL( " my= " ) % QSN( mouse_y ) % nl;

  return s;
}

void MglView::moveMark( int dx, int dy )
{
  mark_screen_x += dx;
  mark_screen_y += dy;
  mglPoint po = gra->CalcXYZ( mark_screen_x, mark_screen_y, wg, hg, &vd, scd.get() );
  scd->setMark( po );
  update();
}

void MglView::showInfo()
{
  QString s = "<p>" + getInfo( true ) + "\n</p>\n<pre>\n";

  if( data_loaded ) {
    s +=  gra->getPrintInfo( vd.sel );
  }
  s += "\n</pre>\n";

  for( const auto &la : gra->TCHILD(PlotLabel*) ) {
    if( !la ) { continue; }
    s += QSL("<p>") % la->objectName() % QSL("= \"") % la->textVisual() % QSL("\"</p>\n");
  }

  QMessageBox::information( this,"Data info", s );
}

static const QString plot_helpstr = QSL("<b>Hot keys:</b><br/>\n"
 "<b>q</b> - close <br/>\n"
 "<b>F1</b> - this help <br/>\n"
 "<b>a/A</b> - change alpha <br/>\n"
 "<b>o/O</b> - on/off non-selected plots <br/>\n"
 "<b>Ctrl [ ]</b> - next/prev plot <br/><hr/>\n"
 "<b>s/S</b> - scale dialog / save current scale <br/>\n"
 "<b>Ctrl-P</b> - print<br/>\n"
 "<b>r</b> - reset and redraw<br/>\n"
 "<b>R</b> - Reload add redraw<br/>\n"
 "<b>e</b> - export image  <br/>\n"
 "<b>b</b> - set base to mark<br/>\n"
 "<b>B</b> - hide/show base lines<br/>\n"
 "<b>f</b> - toggle light<br/>\n"
 "<b>u</b> - toggle footer<br/>\n"
 "<b>v</b> - toggle labels<br/>\n"
 "<b>g</b> - set mark to given point <br/>\n"
 "<b>l/L</b> - link/Unlink to data <br/>\n"
 "<b>i</b> - data info <br/>\n"
 "<b>t</b> - add label <br/>\n"
 "<b>z</b> - zoom base - mark, or near mark <br/>\n"
 "<b>Z</b> - zoom reset <br/><hr/>\n"
 "<b>PageUp, PageDown</b> - move Up/Down <br>\n"
 "<b>+/- Shift, Ctrl</b> - Zoom on X (Y,Z) <br>\n"
 "<b> - not in link state: - </b> <br/>\n"
 "<b>Left, Top, Right, Bottom</b> - Move <br/>\n"
 "<b>[Shift]{Left, Top, Right, Bottom}</b> - Rotate <br>\n"
 "<b>Ctrl {Left, Top, Right, Bottom}</b> - shift plot <br>\n"
 "<b>- in link state: -</b>  <br>\n"
 "<b>[ ] </b> - previous/next point <br>\n");

void MglView::showHelp()
{
  QMessageBox::information( this,"Hot keys in plot window", plot_helpstr );
}

// ============================== MglSubwin =======================================


MglSubwin::MglSubwin( QWidget *a_par, LaboDoc *a_doc, TGraph *a_gra  )
    : CommonSubwin( a_par, a_doc, a_gra->objectName() )
{
  main_win = false;
  title_prefix = QSL("plot");

  mview = new MglView( a_gra, this );

  connect( mview, &MglView::closeMe, this, &MglSubwin::closeMe );

  setCentralWidget( mview );
}

MglSubwin::~MglSubwin()
{
}

bool MglSubwin::callSlot( const char *nm )
{
  return QMetaObject::invokeMethod( mview, nm, Qt::AutoConnection );
}

bool MglSubwin::checkSlot( const char *nm )
{
  return checkSlotSub( mview, nm );
}

int MglSubwin::getLevel() const
{
  return mview->getSelNum();
}


