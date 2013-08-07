/***************************************************************************
                          tfriction.cpp  -  description
                             -------------------
    begin                : Jul 18 2003
    copyright            : (C) 2000-2013 by atu
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
#include <limits>
#include "miscfun.h"
#include "tfriction.h"

using namespace std;

const char* TFriction::helpstr = "<H1>TFriction</H1>\n"
 "Mass under action of external force <B>Fx</B> and friction (dry and viscous): <br>\n"
 " <B> mass * d2x/dt^2  = Fx + F_fd + F_fv </B><br>\n"
 " <B> mass -- </B> body mass <br>\n"
 " <B> F_fd -- </B> dry friction force <br>\n"
 " <B> f_mx -- </B> max value of dry friction force (if const) <br>\n"
 " <B> kf_mx -- </B> coeff of start dry friction force <br>\n"
 " <B> kfv -- </B> coeff of viscous friction force: F_fv = -kfv * v <br>\n"
 " <B> useMf -- </B> use u[1] as <B>f_mx</B> <br><HR>\n"
 " Input <B> u[0] -- </B> external force <BR>\n"
 " Input <B> u[1] -- f_mx </B> if enabled  <BR>\n"
 " Output <B> x -- </B> body coordinate. <BR>\n"
 ;

TClassInfo TFriction::class_info = {
  "TFriction", TFriction::create,
  &TMiso::class_info, helpstr, clpElem };


TFriction::TFriction( TDataSet* aparent )
        :TMiso( aparent )
{
  mass = 1; f_mx = 0.4; kf_mx = 0; kfv = 0.01; useMf = 0;
  v = v_old = x_old = Ff = 0;
  bodyState = 0;
}

TFriction::~TFriction()
{
}

TDataSet* TFriction::create( TDataSet* apar )
{
  return new TFriction( apar );
}


const TClassInfo* TFriction::getClassInfo(void) const
{
  return &class_info;
}

const char *TFriction::getHelp(void) const
{
  return helpstr;
}


int TFriction::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  x_old = v = v_old = Ff = 0;
  bodyState = 0;
  return 0;
}

double TFriction::f( double /* t */ )
{
  double f, x, f_fv, f_fd, cf_mx;
  double fx = *in_so[0];
  if( useMf )
    cf_mx = *in_so[1];
  else
    cf_mx = f_mx;

  if( bodyState == 0 ) { // sleep
    if( fabs(fx) <= cf_mx * ( 1 + kf_mx ) ) {
      Ff = -fx;
      return x_old;  // continue to sleep
    } else { // launch
      f_fd = - cf_mx * sign(fx);
      Ff = f = fx + f_fd;  // skip viscous now ?
      v = f * tdt / mass;
      x = x_old + v * tdt / 2; 
      bodyState = 1;  // begin to move;
    };
  } else { // moving
    f_fd = - sign(v_old) * cf_mx;
    f_fv = - v_old * kfv;
    Ff = f = fx + f_fd + f_fv;
    v = v_old + f * tdt / mass;
    if( v * v_old > 0 )
      x = x_old + ( v + v_old ) * tdt / 2;
    else { // start to  sleep
      v = 0; bodyState = 0;
      x = x_old + v_old * tdt * fabs( v_old / (v_old - v) ) / 2;
    };
  };
  v_old = v; x_old = x;
  return x;
}

int TFriction::registered = reg();

int TFriction::reg()
{
  return EFACT.registerElemType( &class_info  );
}



// end of tfriction.cpp

 
