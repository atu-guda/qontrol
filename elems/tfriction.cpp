/***************************************************************************
                          tfriction.cpp  -  description
                             -------------------
    begin                : Jul 18 2003
    copyright            : (C) 2000-2022 by atu
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

STD_CLASSINFO(TFriction,clpElem);

CTOR(TFriction,TMiso)
{
}


int TFriction::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  x_old = out0_init;
  x2 = x_old * x_old;
  v = v_old = Ff = a = 0;
  bodyState = 0;
  return 1;
}

double TFriction::f() noexcept
{
  double f, x, f_fd, cf_mx;
  double fx = in_u;

  if( useMf ) {
    cf_mx = in_f_mx;
  } else {
    cf_mx = f_mx;
  }

  if( bodyState == 0 ) { // sleep
    if( fabs(fx) <= cf_mx * ( 1 + kf_mx ) ) {
      Ff = -fx;
      return x_old;  // continue to sleep
    } else { // launch
      f_fd = - cf_mx * sign(fx);
      Ff = f = fx + f_fd;  // skip viscous now ?
      a = f / mass;
      v = a * ctdt;
      x = x_old + v * ctdt / 2;
      bodyState = 1;  // begin to move;
    };
  } else { // moving
    f_fd = - sign(v_old) * cf_mx;
    double f_fv = - v_old * kfv;
    Ff = f = fx + f_fd + f_fv;
    a = f / mass;
    v = v_old + a * ctdt;
    if( v * v_old > 0 ) {
      x = x_old + ( v + v_old ) * ctdt / 2;
    } else { // start to  sleep
      a = -v / ctdt;
      v = 0; bodyState = 0;
      x = x_old + v_old * ctdt * fabs( v_old / (v_old - v) ) / 2;
    };
  };

  v_old = v; x_old = x;
  x2 = x * x;
  return x;
}

DEFAULT_FUNCS_REG(TFriction)



// end of tfriction.cpp


