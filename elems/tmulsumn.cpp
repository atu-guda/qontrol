/***************************************************************************
        tmulsumn.h  - N-input multiplicator/summator + COG
                             -------------------
    begin                : 2015.11.15
    copyright            : (C) 2015-2015 by atu
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

#include "tmulsumn.h"

const char* TMulsumN::helpstr = "<h1>TMulsumN</h1>\n"
 "N-input multiplicator/summator + COG calculator: <br>\n"
 "p_i - input with subchannel 0,  <br>\n"
 "f_i - input with subchannel 1, <br>\n"
 "i   - channel number (i>=0), <br>\n"
 "spf = \\sum_i p_i \\cdot f_i ;<br>\n"
 "sp  = \\sum_i p_i ;<br>\n"
 "pge = spf / sp ;<br>\n"
 "ple,spfl,spl - local (near extremum variant);<br>\n"
 "pee,fee, - quadratic approximation;<br>\n"
 "ne - index of extremal point <br>\n"
 "pe - coordinate in extremal point <br>\n"
 "fe - function in extremal point <br>\n";

STD_CLASSINFO(TMulsumN,clpElem);

CTOR(TMulsumN,TMiso)
{
  allowed_types = "InputSimple,+SPECIAL";
}

double TMulsumN::f( double /* t */ )
{
  spf = 0; sp = 0; sf = 0;
  {
    double f_max = DMIN, p_max = DMIN;
    int i_max = -1, i = 0;
    for( auto pt : pf_ins ) {
      double p = *( pt.in_p );
      double f = *( pt.in_f );
      sp += p;
      sf += f;
      spf += p * f;
      if( f > f_max ) {
        f_max = f; i_max = i; p_max = p;
      }
      ++i;
    }
    pge = spf / sf;
    ne = i_max; fe = f_max;  pe = p_max;
  }

  int n = pf_ins.size(); // local ?? np?
  ple = pee = pe; spfl = fe * pe; spl = pe; fee = sfl = fe; // fallback
  if( ne > 0  &&  ne < n-1 ) { // not boundary
    double pl = *(pf_ins[ne-1].in_p);
    double pc = *(pf_ins[ne  ].in_p);
    double pr = *(pf_ins[ne+1].in_p);
    double fl = *(pf_ins[ne-1].in_f);
    double fc = *(pf_ins[ne  ].in_f);
    double fr = *(pf_ins[ne+1].in_f);
    spl = pl + pc + pr;
    sfl = fl + fc + fr;
    spfl = pl*fl + pc*fc + pr*fr;
    ple = spfl / sfl;
    // a-la tquadexrt
    double p_lt = pl - pc;
    double p_rt = pr - pc;
    double f_lt = fl - fc;
    double f_rt = fr - fc;
    double denom = p_lt * p_lt * p_rt - p_lt * p_rt * p_rt;
    if( fabs( denom ) > D_AZERO ) {
      double a_1 = ( f_rt * p_lt * p_lt - f_lt * p_rt * p_rt ) / denom;
      double a_2 = - ( f_rt * p_lt - f_lt * p_rt ) / denom;
      pee = pc - 0.5 * a_1 / a_2; // rel
      pee = qBound( pl, (double)pee, pr );
      double pee_t = pee - pc;
      fee = fc + a_2 * pee_t * pee_t + a_1 * pee_t;
    }
  }
  return spf;
}

int TMulsumN::do_preRun( int /*run_tp*/, int /*an*/, int /*anx*/, int /*any*/, double /*atdt*/ )
{
  QMap<int,ChInSim2> ipairs;
  pf_ins.clear();
  np = 0;
  for( auto i : TCHILD(InputSimple*) ) {
    int ch  = i->getDataD( "channel", -1 );
    int sch = i->getDataD( "subchannel", -1 );
    if( sch == 0 ) {
      ipairs[ch].in_p = i;
    } else if ( sch == 1 ) {
      ipairs[ch].in_f = i;
    } else {
      continue;
    }
  }
  // qWarning() << "ipairs.size= " << ipairs.size() << " max_ch" << max_ch << NWHE;

  for( auto pt : ipairs ) {
    if( pt.in_p != nullptr &&  pt.in_f != nullptr ) {
      pf_ins.append( pt );
    }
  }
  if( ipairs.size() != pf_ins.size() ) {
    qWarning() << "ipairs.size ("<< ipairs.size()
               <<  ") != pf_ins.size= " << pf_ins.size()  << NWHE;
  }
  np = pf_ins.size();

  return true;
}

DEFAULT_FUNCS_REG(TMulsumN)


// end of tsumn.cpp

