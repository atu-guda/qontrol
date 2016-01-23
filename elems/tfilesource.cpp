/***************************************************************************
         tfilesource.c  - element to read text data from file (implementation)
                          -------------------
    begin                : 2015.01.21
    copyright            : (C) 2015-2016 by atu
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

#include "tfilesource.h"

const char* TFileSource::helpstr = "<H1>TFileSource</H1>\n"
 "<p>Reads data from file in textual form<br>\n";

STD_CLASSINFO(TFileSource,clpElem);

CTOR(TFileSource,TMiso)
  , lin( buf_sz, 0 )
{
}

TFileSource::~TFileSource()
{
  all_close();
}

double TFileSource::f() noexcept
{
  double dt;
  while( ( dt = ct - t0 ) >= tau_e ) {
    if( ! next_tau_e() ) {
      return v[0];
    }
  }

  for( int i=0; i<rnc; ++i ) {
    switch( itype.cval() ) {
      case InterpolateType::step :
        v[i] = scale * d0.v[i]; break;
      case InterpolateType::linear :
        v[i] = scale * ( d0.v[i] + d_c.v[i] * dt ); break;
      default:
        v[i] = 0; break;
    }
  }
  return v[0];
}

int TFileSource::next_tau_e()
{
  ++cl;
  d0 = d1; t0 = t1;

  if( cl >= ncl ) {
    readLines( greed );
  }
  if( ncl < 1 ) {
    d1 = d0; t1 = tau_e = 1e100;
    d_c.reset();
    return 0;
  }


  d1 = pv[cl];
  if( time_col >=0 && time_col < max_col ) {
    t1 = d1.v[time_col];
    tau_e = t1 - t0;
  } else {
    t1 += tau_e;
  }

  // qDebug() << "Next: cl= " << cl << " t0= " << t0 << " t1=" << t1 << NWHE;

  for( int i=0; i<rnc; ++i ) { // may be not need in 'step' case
    d_c.v[i] = ( d1.v[i] - d0.v[i] ) / tau_e;
  }

  return 1;
}

int TFileSource::do_preRun()
{
  file.setFileName( filename );
  pv.resize( greed + 1 ); // always need place for 2 lines
  n_ofs = ncl = cl = 0; tau_e = 1; // safe value

  if( time_col >=0  && time_col < max_col ) {
    tau_e = 0.0; // tmp fake value - rewrited on start
  } else {
    if( tau > 0 ) {
      tau_e = tau;
    } else {
      tau_e = ctdt;
    }
  }
  // qDebug() << "Effective tau is " << tau_e << NWHE;

  return 1;
}


int TFileSource::do_postRun( int /*good*/ )
{
  all_close();
  return 1;
}


int TFileSource::miso_startLoop( int /*acnx*/, int /*acny*/ )
{
  if( asProc.cval() ) {
    p_fi = popen( filename.c_str(), "r" );
    if( ! p_fi ) {
      qWarning() << "Fail to start program " << filename << NWHE;
      return 0;
    }
    if( ! file.open( p_fi, QIODevice::ReadOnly | QIODevice::Text ) ) {
      qWarning() << "Fail to open data stream " << filename << NWHE;
      return 0;
    }

  } else {
    if( ! file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
      qWarning() << "Fail to open data file " << file.fileName() << NWHE;
      return 0;
    }
  }

  wasEOF = false;
  n_ofs = ncl = cl = 0;
  int rl_first = ( greed > 1 ) ? greed : 2;
  readLines( rl_first );
  if( ncl < 1 ) {
    qWarning() << "No data found in file " << filename << NWHE;
    return 0;
  }

  // qDebug() << "Start: ncl= " << ncl << NWHE;

  d0 = pv[0]; t0 = 0;
  if( ncl > 1 ) {
    d1 = pv[1];
    if( time_col >=0 ) {
      t1 = d1.v[time_col]; tau_e = t1 - t0;
    } else {
      t1 = t0 + tau_e;
    }
  } else {   // degenerated case -- the only line
    d1 = d0; t1 = tau_e = 1e100;
  }

  for( int i=0; i<rnc; ++i ) { // may be not need in 'step' case
    d_c.v[i] = ( d1.v[i] - d0.v[i] ) / tau_e;
  }
  return 1;
}

int TFileSource::do_endLoop()
{
  all_close();
  return 1;
}



int TFileSource::readLines( int ltr )
{
  if( ltr > greed && ltr > 2 ) { // 2 is for measure_tau
    ltr = greed;
  }

  char sep_c = sep.cval()[0].toLatin1();
  if( !sep_c ) {
    sep_c = ' '; // workaround bug: collapsed spaces-only strings
  }
  cl = 0;

  for( ncl=0; ncl<ltr; /*NOP*/ ) {
    // if( p_fi ) {
    //   if( ! file.waitForReadyRead( 10000 ) ) { // TODO: param
    //     qWarning() << "Fail to wait input data " << filename
    //       << " n_ofs=" <<  n_ofs << "ncl=" << ncl << NWHE;
    //     return 0;
    //   }
    // }
    if( file.atEnd() ) {
      wasEOF = true;
      qDebug() << "EOF! n_ofs=" << n_ofs << " ncl= " << ncl << NWHE;
      break;
    }
    lin = file.readLine( buf_sz ).simplified();
    if( lin.size() < 1 || lin[0] == '#' || lin[0] == ';' ) { // empty or comment
      continue;
    }

    QList<QByteArray> lba = lin.split( sep_c );
    // qDebug() << "Split: [" << lba.size() << "] " << lba << " c=" << (int)sep_c <<  NWHE;
    int i = 0, nr = 0;
    pv[ncl].reset();
    for( auto s : lba ) {
      bool ok;
      double a = s.toDouble( &ok );
      if( ok ) {
        pv[ncl].v[i] = a; ++nr;
      }
      ++i;
    }
    if( nr > 0 ) {
      ++ncl;
    }
  }

  n_ofs += ncl;
  // qDebug() << "After read: n_ofs= " << n_ofs << " bcl= " << ncl << NWHE;
  return ncl;
}

void TFileSource::all_close()
{
  file.close();
  if( p_fi ) {
    pclose( p_fi );
    p_fi = nullptr;
  }
}

DEFAULT_FUNCS_REG(TFileSource)


// end of tfilesource.cpp

