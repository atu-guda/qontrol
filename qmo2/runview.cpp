/***************************************************************************
                          runview.cpp  -  description
                             -------------------
    begin                : Sat Aug 18 2001
    copyright            : (C) 2001-2014 by atu
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
#include <QSemaphore>

#include "miscfun.h"
#include "dataset.h"
#include "tmodel.h"
#include "simul.h"

#include "runview.h"

using namespace std;

RunView::RunView( TModel *a_mod, QWidget *parent )
          : QDialog( parent ), model( a_mod ), timer( new QTimer( this ) ),
            sem_io( 1 )
{
  s_h = 40;
  // setBackgroundMode( Qt::NoBackground );
  sch = model->getActiveScheme();
  setCursor( Qt::CrossCursor );
  connect( timer, &QTimer::timeout, this, &RunView::slotRunNext );
  model->reset();
  getSchemeData();

  for( auto &ks : keys_state ) {
    ks = 0;
  }

  setMinimumSize( 500, s_h );
  setFocusPolicy( Qt::StrongFocus );
  setFocus();
  if( autoStart ) {
    slotStartRun();
  }
}

RunView::~RunView()
{
  delete timer; timer = nullptr;
  delete runner; runner = nullptr;
}

QSize RunView::sizeHint() const
{
  return QSize( 500, s_h );
}

void RunView::slotStartRun()
{
  int rc;
  if( !sch  ||  state != stateGood ) {
    return;
  }
  i_tot = 0;  state = stateRun;
  rc = model->startRun();
  if( !rc ) {
    model->reset();
    state = stateBad;
    DBG1( "warn: model->startRun failed" );
    return;
  };

  runner = new ModelRunner( model, &sem_io, this );
  if( !runner ) {
    return;
  }
  // connect signals

  if( syncRT ) {
    setMouseTracking(1);
  }
  s_time = get_real_time();
  timer->start( (int)(io_t*1000) );
  runner->start();
  //runner->start( QThread::Priority::LowPriority );
}

void RunView::slotStopRun()
{
  timer->stop();
  if( runner ) {
    runner->requestInterruption();
  }
  setMouseTracking( 0 );
  state = stateBad;
  update();
}

void RunView::slotRunNext()
{
  int m_state;
  if( !sch  ||  state != stateRun ) {
    timer->stop();
    DBG1( "warn: nextRun w/o start?" );
    return;
  };

  sem_io.acquire( 1 );
  t = model->get_t();
  il1 = model->get_il1();
  il2 = model->get_il2();
  i_tot = model->get_i_tot();
  // fillVars();
  m_state = model->getState();
  state = m_state;
  sem_io.release( 1 );

  // model->nextSteps( n_iosteps );
  // DBGx( "dbg: m_state: %d i_tot: %d n_tot: %d", m_state, i_tot, n_tot );

  if( state != stateRun ) {
    DBGx( "dbg: final! state: %d m_state: %d", state, m_state );
    timer->stop();
    setMouseTracking( 0 );
    if( s_time > 0  &&  ( get_real_time() - s_time ) > 10 ) {
      qApp->beep();
    }
  };
  update();
}


void RunView::resizeEvent( QResizeEvent *e )
{
  w_h = height(); w_w = width();
  g_h = w_h - 20; g_w = w_w;
  c_x = g_w / 2; c_y = g_h / 2;
  gkx = g_w / 2; gky = g_h / 2;
  QWidget::resizeEvent( e );
}

void RunView::paintEvent( QPaintEvent * /*pe*/ )
{
  static const QColor bco[4] = { QColor(96,0,0),  QColor(0,64,128),
                          QColor(32,196,64), QColor(0,0,64) };
  QPainter p( this );
  p.fillRect( 0, 0, width(), height(), QBrush( bco[state] ) );
  drawAll( p );
  p.end();
}


void RunView::drawAll( QPainter &p )
{
  int gauge_w;
  double rt;
  QString s;

  gauge_w = int( double(i_tot) * g_w / n_tot );
  p.fillRect( 0, g_h+1, gauge_w, 18, Qt::red );
  p.setPen( Qt::white );

  if( syncRT ) {
    p.drawLine( 0, c_y, g_w-1, c_y ); p.drawLine( c_x, 0, c_x, g_h );
  };

  if( s_time > 0 ) {
    rt = get_real_time() - s_time;
  } else {
    rt = 0;
  }

  s.sprintf( "%5s  t: %012.3f; m: [% .2f; % .2f]; rt: %7.2f  i: %7d (%3d:%3d);",
    getStateString(state), /*runType,*/ t, mouse_x, mouse_y, rt,
    i_tot, il1, il2  );
  p.drawText( 10, 14, s );

  // TODO: misc objects
  // if( syncRT ) {
  // };
}



void RunView::mousePressEvent( QMouseEvent *me )
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

void RunView::mouseReleaseEvent( QMouseEvent *me )
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

void RunView::mouseMoveEvent( QMouseEvent *me )
{
  int x, y;
  double vx, vy;
  x = me->x(); y = me->y();
  vis2phys( x, y, &vx, &vy );
  mouse_x = vx; mouse_y = vy;
}


void RunView::keyPressEvent( QKeyEvent *ke )
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

void RunView::keyReleaseEvent( QKeyEvent *ke )
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


void RunView::phys2vis( double x, double y, double *vx, double *vy )
{
  if( vx != 0 ) {
    *vx = c_x + x * gkx;
  };
  if( vy != 0 ) {
    *vy = c_y - y * gky;
  };
}

void RunView::phys2vis( double x, double y, int *ix, int *iy )
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

void RunView::vis2phys( int ix, int iy, double *x, double *y )
{
  if( x != 0 ) {
    *x = ( ix - c_x ) / gkx;
  };
  if( y != 0 ) {
    *y = ( c_y - iy ) / gky;
  };
}

void RunView::getSchemeData()
{
  Simulation *c_sim = model->getActiveSimulation();
  if( c_sim ) {
    n_tot = c_sim->getDataD( "n_tot", 100 );
    n_iosteps = c_sim->getDataD( "n_iosteps", 1 );
    T = c_sim->getDataD( "T", 1.0 );
    N = c_sim->getDataD( "N", 1 );
    syncRT = c_sim->getDataD( "syncRT", 0 );
    autoStart = c_sim->getDataD( "autoStart", 0 );
    io_t = c_sim->getDataD( "io_t", 0.5 );
  } else {
    DBG1( "warn: no active simulation!" );
  }

  // if( syncRT ) {
  //   io_t = T / N;
  // }

  state = model->getState();
  s_h = syncRT ? 520 : 40;
  DBGx( "dbg: n_tot: %d, n_iosteps: %d syncRT: %d autoStart: %d io_t: %lg T: %lf N: %d",
      n_tot, n_iosteps, syncRT, autoStart, io_t, T, N );
}


void RunView::fillVars()
{
  if( state != stateRun || !syncRT ) {
    return;
  }
  // TODO: real work or drop at all?
}


// end of runview.cpp

