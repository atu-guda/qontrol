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
  connect( timer, &QTimer::timeout, this, &QRunView::slotRunNext );
  model->reset();
  getModelData();
  for( i=0; i < 10; i++ )
    keys_state[i] = 0;
  i_tot = 0; n_tot = 1;
  setMinimumSize( 500, s_h );
  setFocusPolicy( Qt::StrongFocus );
  setFocus();
  if( autoStart ) {
    slotStartRun();
  }
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


void QRunView::drawCross( QPainter & /*p*/ )
{
}

void QRunView::drawVbar( QPainter & /*p*/ )
{

}

void QRunView::drawGbar( QPainter & /*p*/ )
{
}

void QRunView::drawLED( QPainter & /*p*/ )
{
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
  model->getData( "autoStart", &autoStart );
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
  if( state != stateRun || !use_sync )
    return;
}


// end of qplotview.cpp

