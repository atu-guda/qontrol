/***************************************************************************
                          qplotview.cpp  -  description
                             -------------------
    begin                : Sat Aug 18 2001
    copyright            : (C) 2001 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <math.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpen.h>
#include <qbrush.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qlabel.h>
#include <q3paintdevicemetrics.h>
#include <qtimer.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>

#include "resource.h"
#include "miscfun.h"
#include "dataset.h"
#include "tmodel.h"

#include "qrunview.h"

QRunView::QRunView( TModel *amodel, int atype, 
                    QWidget *parent, const char *name, Qt::WFlags wf )
          : QDialog( parent, name, 1, wf )
{
  int i;
  model = amodel; run_type = atype; s_time = 0;
  data = 0; pix = 0;
  s_h = 40;
  setBackgroundMode( Qt::NoBackground );
  setCursor( Qt::crossCursor );
  timer = new QTimer();
  connect( timer, SIGNAL(timeout()), this, SLOT(slotRunNext()) );
  model->reset();
  getModelData();
  mouse_x = mouse_y = 0; mouse_l = mouse_r = mouse_m = 0;
  for( i=0; i < 10; i++ ) keys_state[i] = 0;
  sound_i_left = sound_i_right = 0;
  joy_x = joy_y = joy_btn = 0;
  for( i=0; i<20; i++ ) auxs[i] = 0;
  i_tot = 0; n_tot = 1; 
  setMinimumSize( 500, s_h );
  setFocusPolicy( Qt::StrongFocus );
  setFocus();
}

QRunView::~QRunView()
{
  delete pix; pix = 0;
  delete timer; timer = 0;
}

QSize QRunView::sizeHint(void) const
{
  return QSize( 500, s_h );
}

void QRunView::slotStartRun()
{
  int rc;
  if( state != stateGood ) return;
  i_tot = 0;  state = stateRun;
  model->reset();
  rc = model->startRun( run_type );
  if( rc ) {
    model->reset();
    state = stateBad;
    return;
  };
  model->getDataSI( "n_tot", &n_tot, 0 );
  if( use_sync )
    setMouseTracking(1);
  s_time = get_real_time();
  timer->start( 0, 0 );
}

void QRunView::slotStopRun(void)
{
  timer->stop();
  setMouseTracking( 0 );
  state = stateBad;
  model->stopRun( 1 );
  update();
}

void QRunView::slotRunNext(void)
{
  int m_state;
  if( state != stateRun ) {
    timer->stop(); return;
  };
  fillVars();
  model->nextSteps( n_steps );
  m_state = model->getState();
  switch( m_state ) {
    case stateRun: 
      model->getDataSI( "i_tot", &i_tot, 0 );
      if( i_tot >= n_tot ) {
	model->stopRun(0);
	state = model->getState();
      };
      break;
    case stateDone: 
      model->stopRun(0); state = stateDone; 
      break;
    default:
      model->stopRun(1); state = stateBad;
  };
  if( state != stateRun ) {
    timer->stop();
    setMouseTracking( 0 );
    if( s_time > 0  &&  ( get_real_time() - s_time ) > 10 )
      qApp->beep();
  };
  update();
  // repaint( 0 );
}


void QRunView::resizeEvent( QResizeEvent *e )
{
  w_h = height(); w_w = width();
  g_h = w_h - 20; g_w = w_w;
  c_x = g_w / 2; c_y = g_h / 2;
  gkx = g_w / 2; gky = g_h / 2;
  delete pix;
  pix = new QPixmap( w_w, w_h );
  QWidget::resizeEvent( e );
}

void QRunView::paintEvent( QPaintEvent * /*pe*/ )
{
  static const QColor bco[4] = { QColor(96,0,0),  QColor(0,64,128), 
                          QColor(32,196,64), QColor(0,0,64) };
  if( pix == 0 )
    pix = new QPixmap( width(), height() );
  pix->fill( bco[state] );
  QPainter p( pix );
  drawAll( p );
  p.end();
  bitBlt( this, QPoint(0,0), pix );
}


void QRunView::drawAll( QPainter &p )
{
  int gauge_w, il1, il2;
  double t = 0, rt;
  QString s;
  
  gauge_w = int( double(i_tot) * g_w / n_tot );
  p.fillRect( 0, g_h+1, gauge_w, 18, Qt::red );
  p.setPen( Qt::white );
  if( use_sync ) {
    p.drawLine( 0, c_y, g_w-1, c_y ); p.drawLine( c_x, 0, c_x, g_h );
  };
  if( s_time > 0 )
    rt = get_real_time() - s_time;
  else
    rt = 0;
  model->getDataSD( "t", &t, 0 );
  model->getDataSI( "il1", &il1, 0 );
  model->getDataSI( "il2", &il2, 0 );
  s.sprintf( "%5s  tp: %d t: %012.3f; m: [% .2f; % .2f]; rt: %7.2f  i: %7d (%3d:%3d);", 
      getStateString(state), run_type, t, mouse_x, mouse_y, rt, 
      i_tot, il1, il2  );
  p.drawText( 10, 14, s );
  if( use_sync ) {
    drawVbar( p );
    drawGbar( p );
    drawLED( p );
    drawCross( p );
  };
}


void QRunView::drawCross( QPainter &p )
{
  int i, ix, iy;
  double x, y;
  static const QColor colors[3] = { Qt::white, Qt::red, Qt::green };
  for( i=0; i<6; i++ ) {
    if( oc[i] < 0 || oct[i] < 1 || oct[i] > 3 ) 
      continue;
    x = data[ oc[i] ]; y = data[ 1+oc[i] ];
    if( x > 1 ) x = 1; if( x < -1 ) x = -1;
    if( y > 1 ) y = 1; if( y < -1 ) y = -1;
    phys2vis( x, y, &ix, &iy );
    p.setPen( colors[ oct[i] - 1 ] );
    p.drawRect( ix-4, iy-4, 9, 9 );
    p.drawLine( ix, iy-10, ix, iy-2 );
    p.drawLine( ix, iy+10, ix, iy+2 );
    p.drawLine( ix-10, iy, ix-5, iy );
    p.drawLine( ix+10, iy, ix+5, iy );
  };
}

void QRunView::drawVbar( QPainter &p )
{
  int i, ix, iy;
  double x, y;
  static const QColor colors[4] = { Qt::white, Qt::red, Qt::green, Qt::yellow };
  for( i=0; i<6; i++ ) {
    if( oc[i] < 0 || oct[i] < 4 || oct[i] > 7 ) 
      continue;
    x = -0.9 + (oct[i] - 4) * 0.4 ; y = data[ oc[i] ];
    if( y > 1 ) y = 1; if( y < -1 ) y = -1;
    phys2vis( x, y, &ix, &iy );
    p.fillRect( ix, c_y, 30, iy - c_y, colors[ oct[i] - 4 ]  );
  };

}

void QRunView::drawGbar( QPainter &p )
{
  int i, ix, iy;
  double x, y;
  static const QColor colors[4] = { Qt::white, Qt::red, Qt::green, Qt::yellow };
  for( i=0; i<6; i++ ) {
    if( oc[i] < 0 || oct[i] < 8 || oct[i] > 11 ) 
      continue;
    x = data[ oc[i] ];y = 0.9 - (oct[i] - 8) * 0.4 ;
    if( x > 1 ) x = 1; if( x < -1 ) x = -1;
    phys2vis( x, y, &ix, &iy );
    p.fillRect( c_x, iy, ix - c_x, 30, colors[ oct[i] - 8 ] );
  };
}

void QRunView::drawLED( QPainter &p )
{
  int i;
  double x;
  for( i=0; i<6; i++ ) {
    if( oc[i] < 0 || oct[i] < 12 || oct[i] > 15 ) 
      continue;
    x = data[ oc[i] ]; 
    p.fillRect( 50 * ( 1 + oct[i] - 12 ), g_h-19, 20, 20,
       ( x > 0.1 ) ? Qt::green : Qt::darkRed );
  };
}
   
void QRunView::mousePressEvent( QMouseEvent *me )
{
  int x, y, btn;
  double vx, vy;
  x = me->x(); y = me->y(); btn = me->button();
  vis2phys( x, y, &vx, &vy );
  switch( btn ) {
    case Qt::LeftButton:  mouse_l = 1;  break;
    case Qt::RightButton: mouse_r = 1;  break;
    case Qt::MidButton:   mouse_m = 1;  break;
    default:         break;
  };
  mouse_x = vx; mouse_y = vy;
  if( state != stateRun )
    update();
}

void QRunView::mouseReleaseEvent( QMouseEvent *me )
{
  int x, y, btn;
  double vx, vy;
  x = me->x(); y = me->y(); btn = me->button();
  vis2phys( x, y, &vx, &vy );
  switch( btn ) {
    case Qt::LeftButton:  mouse_l = 0;  break;
    case Qt::RightButton: mouse_r = 0;  break;
    case Qt::MidButton:   mouse_m = 0;  break;
    default:         break;
  };
  mouse_x = vx; mouse_y = vy;
}

void QRunView::mouseMoveEvent( QMouseEvent *me )
{
  int x, y;
  double vx, vy;
  x = me->x(); y = me->y(); 
  vis2phys( x, y, &vx, &vy );
  mouse_x = vx; mouse_y = vy;
}


void QRunView::keyPressEvent( QKeyEvent *ke )
{
  int k, st, btnCtrl, btnShift, mul_btn;
  k = ke->key(); st = ke->state();
  btnShift = (( st & Qt::ShiftModifier ) != 0 ) ? 2 : 1;
  btnCtrl = (( st & Qt::ControlModifier ) != 0 ) ? 4 : 1;
  mul_btn = btnShift * btnCtrl;
  if( state == stateDone ) {
    accept();
    return;
  };
  if( state == stateBad ) {
    reject();
    return;
  };
  if( state == stateGood ) {
    slotStartRun();
  } else {  
    switch( k ) {
      case Qt::Key_Left:  keys_state[0] = mul_btn;	 break;
      case Qt::Key_Right: keys_state[1] = mul_btn;	 break;
      case Qt::Key_Up:    keys_state[2] = mul_btn;	 break;
      case Qt::Key_Down:  keys_state[3] = mul_btn;	 break;
      case Qt::Key_Space: keys_state[4] = mul_btn;	 break;
      case Qt::Key_Enter: keys_state[5] = mul_btn;	 break;
      case Qt::Key_Escape: slotStopRun(); 	         break;
      default: ke->ignore();
    };
  };
}

void QRunView::keyReleaseEvent( QKeyEvent *ke )
{
  int k;
  k = ke->key(); 
  if( state != stateRun ) {
    ke->ignore();
    return;
  } else {  
    switch( k ) {
      case Qt::Key_Left:  keys_state[0] = 0;	 break;
      case Qt::Key_Right: keys_state[1] = 0;	 break;
      case Qt::Key_Up:    keys_state[2] = 0;	 break;
      case Qt::Key_Down:  keys_state[3] = 0;	 break;
      case Qt::Key_Space: keys_state[4] = 0;	 break;
      case Qt::Key_Enter: keys_state[5] = 0;	 break;
      default: ke->ignore();
    };
  };
}


void QRunView::phys2vis( double x, double y, double *vx, double *vy )
{
  if( vx != 0 ) {
    *vx = c_x + x * gkx; 
  };
  if( vy != 0 ) {
    *vy = c_y - y * gky; 
  };
}

void QRunView::phys2vis( double x, double y, int *ix, int *iy )
{
  double tx, ty;
  phys2vis( x, y, &tx, &ty );
  if( ix != 0 ) {
    *ix = (int) rint( tx ); 
  };
  if( iy != 0 ) {
    *iy = (int) rint( ty ); 
  };
}

void QRunView::vis2phys( int ix, int iy, double *x, double *y )
{
  if( x != 0 ) {
    *x = ( ix - c_x ) / gkx;
  };
  if( y != 0 ) {
    *y = ( c_y - iy ) / gky;
  };
}

void QRunView::getModelData(void)
{
  model->getDataSI( "n_tot", &n_tot, 0 );
  model->getDataSI( "n_steps", &n_steps, 0 );
  model->getDataSI( "use_sync", &use_sync, 0 );
  model->getDataSI( "ic_mouse", &ic_mouse, 0 );
  model->getDataSI( "ic_key", &ic_key, 0 );
  model->getDataSI( "ic_joy", &ic_joy, 0 );
  model->getDataSI( "ic_sound", &ic_sound, 0 );
  model->getDataSI( "ic_aux", &ic_aux, 0 );
  model->getDataSI( "oc_0", oc, 0 );
  model->getDataSI( "oc_1", oc+1, 0 );
  model->getDataSI( "oc_2", oc+2, 0 );
  model->getDataSI( "oc_3", oc+3, 0 );
  model->getDataSI( "oc_4", oc+4, 0 );
  model->getDataSI( "oc_5", oc+5, 0 );
  model->getDataSI( "oct_0", oct, 0 );
  model->getDataSI( "oct_1", oct+1, 0 );
  model->getDataSI( "oct_2", oct+2, 0 );
  model->getDataSI( "oct_3", oct+3, 0 );
  model->getDataSI( "oct_4", oct+4, 0 );
  model->getDataSI( "oct_5", oct+5, 0 );
  data = model->getVars();
  state = model->getState();
  s_h = use_sync ? 520 : 40;
}

void QRunView::getJoyVal(void)
{
  if( use_sync && ic_joy >=0 && ic_joy < MODEL_NVAR - 10 ) {
    data[ ic_joy ]      = joy_x = 0; // TODO: fill from real joystick data
    data[ ic_joy + 1 ]  = joy_y = 0;
    data[ ic_joy + 2 ]  = joy_btn = 0;
  };  
}

void QRunView::getSoundVal(void)
{
  if( use_sync && ic_sound >=0 && ic_sound < MODEL_NVAR - 4 ) {
    data[ic_sound]   = sound_i_left  = 0; // TODO: fill from real sound data
    data[ic_sound+1] = sound_i_right = 0; 
  };  
}

void QRunView::getAuxVal(void)
{
  int i;
  if( use_sync && ic_aux >=0 && ic_aux < MODEL_NVAR - 24 ) {
    for( i=0; i<20; i++ )
      data[i+ic_aux] = auxs[i] = 0; // TODO: fill by some real device;
  };  
}

void QRunView::fillVars(void)
{
  int i;
  if( state != stateRun || !use_sync || data == 0 )
    return;
  if( ic_mouse >= 0 && ic_mouse < MODEL_NVAR - 10 ) {
    data[ic_mouse]   = mouse_x; 
    data[ic_mouse+1] = mouse_y; 
    data[ic_mouse+2] = mouse_l; 
    data[ic_mouse+3] = mouse_r; 
    data[ic_mouse+4] = mouse_m; 
  };
  if( ic_key >=0 && ic_key < MODEL_NVAR - 10 ) {
    for( i=0; i<6; i++ )
      data[ic_key+i] = double( keys_state[i] );
  };
  getSoundVal();
  getJoyVal();
  getAuxVal();
}


// end of qplotview.cpp

