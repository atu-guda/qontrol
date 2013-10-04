/***************************************************************************
                          qplotview.cpp  -  description
                             -------------------
    begin                : Sat Aug 18 2001
    copyright            : (C) 2001-2013 by atu
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

#include <QApplication>
#include <QtWidgets>

#include "miscfun.h"
#include "dataset.h"
#include "tmodel.h"

#include "qrunview.h"

using namespace std;

QRunView::QRunView( TModel *amodel, int atype, QWidget *parent )
          : QDialog( parent )
{
  int i;
  model = amodel; run_type = atype;
  s_h = 40;
  // setBackgroundMode( Qt::NoBackground );
  setCursor( Qt::CrossCursor );
  timer = new QTimer();
  connect( timer, SIGNAL(timeout()), this, SLOT(slotRunNext()) );
  model->reset();
  getModelData();
  for( i=0; i < 10; i++ )
    keys_state[i] = 0;
  for( i=0; i<20; i++ ) auxs[i] = 0;
  i_tot = 0; n_tot = 1;
  setMinimumSize( 500, s_h );
  setFocusPolicy( Qt::StrongFocus );
  setFocus();
}

QRunView::~QRunView()
{
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
  model->getData( "n_tot", &n_tot );
  if( use_sync )
    setMouseTracking(1);
  s_time = get_real_time();
  timer->start( 0 );
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
      model->getData( "i_tot", &i_tot );
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
  QWidget::resizeEvent( e );
}

void QRunView::paintEvent( QPaintEvent * /*pe*/ )
{
  static const QColor bco[4] = { QColor(96,0,0),  QColor(0,64,128),
                          QColor(32,196,64), QColor(0,0,64) };
  QPainter p( this );
  p.fillRect( 0, 0, width(), height(), QBrush( bco[state] ) );
  drawAll( p );
  p.end();
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
  model->getData( "t", &t );
  model->getData( "il1", &il1 );
  model->getData( "il2", &il2 );
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
    // x = (*data)[ oc[i] ]; y = (*data)[ 1+oc[i] ];
    x = 0; y = 0; // TODO use new interface
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
    //x = -0.9 + (oct[i] - 4) * 0.4 ; y = (*data)[ oc[i] ];
    x = 0; y = 0; // TODO use new interface
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
    //x = (*data)[ oc[i] ];y = 0.9 - (oct[i] - 8) * 0.4 ;
    x = 0; y = 0; // TODO use new interface
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
    // x = (*data)[ oc[i] ];
    x = 0; // TODO use new interface
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
  k = ke->key(); st = ke->modifiers();
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
      case Qt::Key_Left:  keys_state[0] = mul_btn;         break;
      case Qt::Key_Right: keys_state[1] = mul_btn;         break;
      case Qt::Key_Up:    keys_state[2] = mul_btn;         break;
      case Qt::Key_Down:  keys_state[3] = mul_btn;         break;
      case Qt::Key_Space: keys_state[4] = mul_btn;         break;
      case Qt::Key_Enter: keys_state[5] = mul_btn;         break;
      case Qt::Key_Escape: slotStopRun();                  break;
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
      case Qt::Key_Left:  keys_state[0] = 0;         break;
      case Qt::Key_Right: keys_state[1] = 0;         break;
      case Qt::Key_Up:    keys_state[2] = 0;         break;
      case Qt::Key_Down:  keys_state[3] = 0;         break;
      case Qt::Key_Space: keys_state[4] = 0;         break;
      case Qt::Key_Enter: keys_state[5] = 0;         break;
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
  model->getData( "n_tot", &n_tot );
  model->getData( "n_steps", &n_steps );
  model->getData( "use_sync", &use_sync );
  model->getData( "ic_mouse", &ic_mouse );
  model->getData( "ic_key", &ic_key );
  model->getData( "ic_joy", &ic_joy );
  model->getData( "ic_sound", &ic_sound );
  model->getData( "ic_aux", &ic_aux );
  model->getData( "oc_0", oc );
  model->getData( "oc_1", oc+1 );
  model->getData( "oc_2", oc+2 );
  model->getData( "oc_3", oc+3 );
  model->getData( "oc_4", oc+4 );
  model->getData( "oc_5", oc+5 );
  model->getData( "oct_0", oct );
  model->getData( "oct_1", oct+1 );
  model->getData( "oct_2", oct+2 );
  model->getData( "oct_3", oct+3 );
  model->getData( "oct_4", oct+4 );
  model->getData( "oct_5", oct+5 );
  state = model->getState();
  s_h = use_sync ? 520 : 40;
}

void QRunView::getJoyVal(void)
{
  // not now
}

void QRunView::getSoundVal(void)
{
  // not now
}

void QRunView::getAuxVal(void)
{
  // not now
}

void QRunView::fillVars(void)
{
  // int i;
  if( state != stateRun || !use_sync )
    return;
  /*
  if( ic_mouse >= 0 && ic_mouse < MODEL_NVAR - 10 ) {
    (*data)[ic_mouse]   = mouse_x;
    (*data)[ic_mouse+1] = mouse_y;
    (*data)[ic_mouse+2] = mouse_l;
    (*data)[ic_mouse+3] = mouse_r;
    (*data)[ic_mouse+4] = mouse_m;
  };
  if( ic_key >=0 && ic_key < MODEL_NVAR - 10 ) {
    for( i=0; i<6; i++ )
      (*data)[ic_key+i] = double( keys_state[i] );
  };
  */
  // TODO: use new interface
  //

  // getSoundVal();
  // getJoyVal();
  // getAuxVal();
}


// end of qplotview.cpp

