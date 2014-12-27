/***************************************************************************
                          mglview.cpp - window to show plots via MathGL
                             -------------------
    begin                : 2014.11.29
    copyright            : (C) 2014-2014 by atu
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

#include <cmath>

#include <QtWidgets>
#include <QPrinter>
#include <QPrintDialog>

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
          scd ( new ScaleData( "scd", nullptr, 0, "scale", "current scale data" ) ),
          pa_fnt ( LaboWin::labowin->getPlotFont() )
{
  QFontMetrics fm( pa_fnt );
  em = fm.width( 'W' );
  ex = fm.height();
  bottom_h = 4 * ex;

  Reload();
  QSize sz0 = getSize0();
  resize( sz0 + QSize( 0, bottom_h ) );

  alc_x = sz0.height(); alc_y = sz0.width();
  pb.resize( alc_x * alc_y * 4 );

  setMinimumSize( 480, 420 );
  setCursor( Qt::CrossCursor );
}

MglView::~MglView()
{
  resetData();
  gra = nullptr; // not delete, we are not owner, just for debug
  delete scd; scd = nullptr;
}

void MglView::resetData()
{
  data_loaded = false;
  sel = 0;
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
    DBG1( "err: no TGraph passed to MglDrawer" );
    return;
  }

  scd_o =  gra->getElemT<ScaleData*>( "scd" );
  if( scd_o ) {
    QString s = scd_o->toString();// terrble copy
    scd->fromString( s );
  }
  gra->prepare();
  if( ! gra->fillViewData( &vd ) ) {
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
  if( p0 == p1 ) {
    p0 -= 0.25 * vd.pv_dlt; p1 += 0.25 * vd.pv_dlt;
  }
  vd.pv_min = p0; vd.pv_max = p1; vd.pv_dlt = vd.pv_max - vd.pv_min;
  vd.mag = mglPoint( vd.pv_dlt.x / pr_dlt.x, vd.pv_dlt.y / pr_dlt.y, vd.pv_dlt.z / pr_dlt.z );
  update();
}

void MglView::zoomReset()
{
  if( !data_loaded ) {
    return;
  }

  vd.mag = mglPoint( 1, 1, 1 );
  vd.pv_min = pr_min; vd.pv_max = pr_max; vd.pv_dlt = vd.pv_max - vd.pv_min;
  update();
}


QSize MglView::sizeHint() const
{
  return getSize0() + QSize( 0, bottom_h );
}

void MglView::drawAll( QPainter &p )
{
  int w = width(), h = height(), hg = h - bottom_h;
  gr.SetSize( w, hg );
  gra->plotTo( &gr, &vd, scd );

  gr.GetBGRN( pb.data(), 4*w*hg );

  QPixmap pic = QPixmap::fromImage( QImage( pb.data(), w, hg, QImage::Format_RGB32 ));

  p.setFont( pa_fnt );
  // p.fillRect( 0, 0, w, h, QBrush( QColor(128,255,255) ) );
  p.drawPixmap( 0, 0, pic );
  drawFooter( p, hg );
}

void MglView::paintEvent( QPaintEvent * /*pe*/ )
{
  QPainter p( this );
  drawAll( p );
  p.end();

}

void MglView::drawFooter( QPainter &p, int hg )
{
  int w = width();
  QColor bg_c = QColor( scd->bgcolor );
  QColor fg_c = QColor( 0,0,0 );
  if( bg_c.value() < 128 ) {
    fg_c = QColor( 255, 255, 255 );
  }

  p.fillRect( 0, hg, w, bottom_h, QBrush( bg_c ) );
  p.setPen( fg_c );
  p.drawLine( 0, hg, w, hg );

  QString s = getInfo();

  p.drawText( ex, hg, w-2*ex, bottom_h, Qt::AlignLeft, s );
}

void MglView::mousePressEvent( QMouseEvent *me )
{
  if( !data_loaded ) {
    return;
  }

  int mx = me->x(), my = me->y(), btn = me->button();
  mglPoint po = gr.CalcXYZ( mx, my );
  // DBGx( "dbg: mouse: x: %d y: %d ; graph: x: %lf y: %lf z: %lf",
  //        mx, my, mark_point.x, mark_point.y, mark_point.z );
  switch( btn ) {
    case Qt::LeftButton:
      unlinkFromPlot();
      scd->setMark( po );
         break;
    case Qt::RightButton:
         break;
    case Qt::MidButton:
         break;
    default:
         break;
  };
  update();

}

void MglView::keyPressEvent( QKeyEvent *ke )
{
  constexpr int Sh = 0x40000000;
  constexpr int Ct = 0x80000000;
  int k = ke->key(), st = ke->modifiers();
  if( st & Qt::ShiftModifier ) {
      k |= Sh;
  }
  if( st & Qt::ControlModifier ) {
      k |= Ct;
  }

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
    case Qt::Key_O:
      togglePlot();
      break;
    case Qt::Key_O | Sh:
      toggleAllPlots();
      break;
    case Qt::Key_Q:
      parentWidget()->close();
      break;
    case Qt::Key_R | Sh:
      resetScale();
      break;
    case Qt::Key_P | Ct:
      printPlot();
      break;
    case Qt::Key_E:
      exportPlot();
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
      setYbase( -vd.pv_dlt.y * scale_step, true );
      break;
    case Qt::Key_Down:
      setYbase( vd.pv_dlt.y * scale_step, true );
      break;
    case Qt::Key_Left:
      setXbase( vd.pv_dlt.x * scale_step, true );
      break;
    case Qt::Key_Right:
      setXbase( -vd.pv_dlt.x * scale_step, true );
      break;
    case Qt::Key_PageUp:
      setZbase( -vd.pv_dlt.z * scale_step, true );
      break;
    case Qt::Key_PageDown:
      setZbase( vd.pv_dlt.z * scale_step, true );
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

    case Qt::Key_0: sel = 0; update(); break;
    case Qt::Key_1: sel = 1; update(); break;
    case Qt::Key_2: sel = 2; update(); break;
    case Qt::Key_3: sel = 3; update(); break;
    case Qt::Key_4: sel = 4; update(); break;
    case Qt::Key_5: sel = 5; update(); break;
    case Qt::Key_6: sel = 6; update(); break;
    case Qt::Key_7: sel = 7; update(); break;
    case Qt::Key_8: sel = 8; update(); break;
    case Qt::Key_9: sel = 9; update(); break;
  }

  update();
}

void MglView::resizeEvent( QResizeEvent *e )
{
  QSize cs = size();
  alc_x = cs.width(); alc_y = cs.height();
  pb.resize( alc_x * alc_y * 4 ); // sub bottom_h ?

  QWidget::resizeEvent( e );
}

void MglView::resetScale()
{
  Reload();
  // TODO: more
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
  update();
}

void MglView::setTheta( double a_theta, bool add )
{
  if( add ) {
    scd->theta += a_theta;
  } else {
    scd->theta  = a_theta;
  }
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

void MglView::setXbase( double base, bool rel )
{
  if( rel ) {
    vd.pv_min.x += base;
  } else {
    vd.pv_min.x  = base;
  }
  update();
}

void MglView::setYbase( double base, bool rel )
{
  if( rel ) {
    vd.pv_min.y += base;
  } else {
    vd.pv_min.y  = base;
  }
  update();
}

void MglView::setZbase( double base, bool rel )
{
  if( rel ) {
    vd.pv_min.z += base;
  } else {
    vd.pv_min.z  = base;
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
  scd->alpha = qBound( 0.0, (double)(scd->alpha), 1.0 );
  update();
}


void MglView::setMark()
{
  QDialog *dia = new QDialog( this );
  dia->setWindowTitle( "Set mark to point: " );
  QGridLayout *lay = new QGridLayout( dia );

  QLineEdit *markX_ed = new QLineEdit( dia );
  markX_ed->setValidator( new QDoubleValidator() );
  markX_ed->setText( QSN( scd->markX ) );
  lay->addWidget( markX_ed, 0, 1 );

  QLineEdit *markY_ed = new QLineEdit( dia );
  markY_ed->setValidator( new QDoubleValidator() );
  markY_ed->setText( QSN( scd->markY ) );
  lay->addWidget( markY_ed, 1, 1 );

  QLineEdit *markZ_ed = new QLineEdit( dia );
  markZ_ed->setValidator( new QDoubleValidator() );
  markZ_ed->setText( QSN( scd->markZ ) );
  lay->addWidget( markZ_ed, 2, 1 );

  QLabel *la_x = new QLabel( "&X value", dia );
  la_x->setBuddy( markX_ed );
  lay->addWidget( la_x, 0, 0 );
  QLabel *la_y = new QLabel( "&Y value", dia );
  la_y->setBuddy( markY_ed );
  lay->addWidget( la_y, 1, 0 );
  QLabel *la_z = new QLabel( "&Z value", dia );
  la_z->setBuddy( markZ_ed );
  lay->addWidget( la_z, 2, 0 );

  QDialogButtonBox *bbox
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, 3, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

  int rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    scd->markX = markX_ed->text().toDouble();
    scd->markY = markY_ed->text().toDouble();
    scd->markZ = markZ_ed->text().toDouble();
  };
  delete dia;
  update();

}

void MglView::setScale()
{
  DataDialog *dia = new DataDialog( *scd, this );

  int rc = dia->exec();
  delete dia;
  if( rc == QDialog::Accepted ) {
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
  if( sel < 0 || sel >= (int)sizeof(vd.off)*8 ) {
    return;
  }
  uint64_t msk = 1ull << sel;
  vd.off ^= msk;

  update();
}

void MglView::toggleAllPlots()
{
  if( sel < 0 || sel >= (int)sizeof(vd.off)*8 ) {
    return;
  }
  uint64_t msk = ~(1ull << sel);
  vd.off ^= msk;

  update();
}

void MglView::linkToPlot()
{
  if( sel < 0 || sel >= vd.ng ) {
    return;
  }

  linkPlot = sel;
  mglPoint mp = scd->getMark();
  linkIdx = gra->findNearest( mp, sel );
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

void MglView::setMarkToLink()
{
  if( linkPlot < 0 ) {
    return;
  }
  linkIdx = qBound( 0, linkIdx, vd.nn-1 );
  mglPoint targ;
  if( ! gra->getPointAt( linkPlot, linkIdx, &targ ) ) {
    DBGx( "warn: unknown point %d line %d", linkIdx, linkPlot );
    return;
  }
  scd->setMark( targ );
  update();
}


void MglView::printPlot()
{
  QPrinter *pr;
  if( ! LaboWin::labowin ) {
    return;
  }
  pr = LaboWin::labowin->getPrinter();
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
  QString fn = QFileDialog::getSaveFileName( this, "Save Picture", "out.png",
               "PNG files (*.png);;All files (*)" );
  if( fn.isEmpty() ) {
    return;
  };

  QImage timg( width(), height(), QImage::Format_RGB32 );
  timg.fill( 0xFF );
  QPainter painter( &timg );
  render( &painter );

  if( ! timg.save( fn, "PNG", 50 ) ) {
    QString err = strerror(errno);
    QMessageBox::warning(this, "Fail to save picture",
        QString("Fail fo save picture file: \"%1\": %2").arg( fn ).arg( err ),
        QMessageBox::Ok );
  }


}

QString MglView::getInfo( bool more ) const
{
  if( ! data_loaded ) {
    return QString( "No Data" );
  }
  QString nl = "\n", nl1 = " ";
  if( more ) {
    nl = "<br/>\n"; nl1 = nl;
  }

  mglPoint mark_point = scd->getMark();
  mglPoint base_point = scd->getBase();
  mglPoint rel_p = mark_point - base_point;
  double kyx = 0.0, kzx = 0.0;
  if( rel_p.x != 0 ) {
    kyx = rel_p.y / rel_p.x;
    kzx = rel_p.z / rel_p.x;
  }

  QString s = QString( "mark: ") % toQString( mark_point )
     % "  base: " % toQString( base_point ) % nl1
     % "  rel: " % toQString( rel_p ) % " kyx: " % QSN( kyx ) % " kzx: " % QSN( kzx )
     % "  [ " % QSN( vd.nx ) % " x " % QSN( vd.ny ) % " ]"
     % nl
     % "vis: " % toQString( vd.pv_min ) % " - " % toQString( vd.pv_max )
     % nl1
     % " D: " % toQString( vd.pv_dlt )
     % "  mag: " % toQString( vd.mag ) % nl1
     % "real: " % toQString( pr_min ) % " - " % toQString( pr_max ) % nl
     % QSN( sel ) % " ";

  s += gra->getPlotLabel( sel );

  if( linkPlot > -1 ) {
    s += "  Link: " % QSN( linkPlot) % " "
       % gra->getPlotLabel( linkPlot ) % " [" % QSN( linkIdx ) % "]";
  }
  return s;
}

void MglView::showInfo()
{
  QString s = "<p>" + getInfo( true ) + "\n</p>\n<pre>\n";

  if( data_loaded ) {
    s +=  gra->getPrintInfo( sel );
  }
  s += "\n</pre>\n";

  QMessageBox::information( this,"Data info", s );
}

static const char plot_helpstr[] = "<b>Hot keys:</b><br/>\n"
 "<b>q</b> - close <br/>\n"
 "<b>F1</b> - this help <br/>\n"
 "<b>a/A</b> - change alpha <br/>\n"
 "<b>o/O</b> - on/off non-selected plots <br/>\n"
 "<b>s/S</b> - scale dialog / save current scale <br/>\n"
 "<b>Ctrl-P</b> - print<br/>\n"
 "<b>R</b> - reset and redraw<br/>\n"
 "<b>e</b> - export image to png <br/>\n"
 "<b>b</b> - set base to mark<br/>\n"
 "<b>B</b> -hide/how base lines<br/>\n"
 "<b>g</b> - set mark to given point <br/>\n"
 "<b>l/L</b> - link/Unlink to data <br/>\n"
 "<b>d</b> - data info <br/>\n"
 "<b>z</b> - zoom base - mark, or near mark <br/>\n"
 "<b>Z</b> - zoom reset <br/>\n"
 "<b>PageUp, PageDown</b> - move Up/Down <br>\n"
 "<b>+/- Shift, Ctrl</b> - Zoom on X (Y,Z) <br>\n"
 "<b> - not in link state: - </b> <br/>\n"
 "<b>Left, Top, Right, Bottom</b> - Move <br/>\n"
 "<b>[Shift]{Left, Top, Right, Bottom}</b> - Rotate <br>\n"
 "<b>Ctrl {Left, Top, Right, Bottom}</b> - shift plot <br>\n"
 "<b>- in link state: -</b>  <br>\n"
 "<b>[Shift]{Left, Top}</b> - prev/next point [more] <br>\n"
 "<b>[Shift]{Top, Bottom}}</b> - find [global] max/min <br>\n";

void MglView::showHelp()
{
  QMessageBox::information( this,"Hot keys in plot window", plot_helpstr );
}

