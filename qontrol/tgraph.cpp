/***************************************************************************
                          tgraph.cpp  -  description
                             -------------------
    begin                : Sat Aug 5 2000
    copyright            : (C) 2000-2015 by atu
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


#include "miscfun.h"
#include "tgraph.h"
#include "tmodel.h"
#include "toutarr.h"

using namespace std;

const char* GraphElem::helpstr = "<H1>GraphElem</H1>\n"
 "Defines one source and output type for onw  line";

const char *role_name[] = {
  "none", "X axis", "Y axis", "Z axis", "Plot", "C0", "C1", "C2", "C3", "C4", "C5", "Sz", "??1"
};

STD_CLASSINFO(GraphElem,clpElem);

CTOR(GraphElem,TDataSet)
{
}

GraphElem::~GraphElem()
{
  reset();
}

QVariant GraphElem::dataObj( int col, int role ) const
{
  if( role == Qt::StatusTipRole ) { // used for button labels in dialogs
    if( col != 0 ) {
      return QVariant();
    }

    QString s = src;
    QStringList el = getEnumStrings( "DataType" );
    s += " (" % el.at( type );
    if( is2D ) {
      s += ",2D";
    }
    if( ! extra.isEmpty() ) {
      s += QSL("," ) % extra.cval();
    }
    s += ") ";
    if( ! label.isEmpty() ) {
      s += QSL(" \"" ) % label.cval() % QSL( "\"" );
    }
    return s;
  }

  return TDataSet::dataObj( col, role );
}


void GraphElem::do_reset()
{
  role = LineRole::none;
  nn = 0; nx = 0; ny = 1; ig = -1;
  delete md; md = nullptr;
  ve = nullptr;
}

LineRole GraphElem::fillForPlot( int &g_nn, int &g_ny, int igc )
{
  LineRole rl = LineRole::none;
  ig = -1;
  TModel *model = getAncestorT<TModel>();
  if( !model ) {
    qCritical() << "not found model " << NWHE;
    return rl;
  }
  if( src.isEmpty() ) {
    return LineRole::none;
  }
  TOutArr *arr = model->getOutArr( src );
  if( !arr ) {
    return rl;
  }

  nn = arr->getDataD( "n", 0 );
  if( nn < 1 ) {
    return rl;
  }
  ny = arr->getDataD( "ny", 1 );
  ve = arr->getArray();
  if( ny < 1 ) {
    return rl;
  }
  nx = nn / ny;

  if( g_nn == 0 ) { // first non-empty array defined all other
    g_nn = nn; g_ny = ny;
  }
  if( g_nn != nn  && g_ny != ny ) {
    return rl;
  }

  v_min = arr->getDataD( "dmin", 0.0 );
  v_max = arr->getDataD( "dmax", 0.0 );

  // QString label_c = label.isEmpty() ? ( QString( "y_%1" ).arg( dl.size() )) : label;
  QString label_c = label.isEmpty() ? QString( "y" ) : label;
  pl_label = label_c.toStdString();

  int i_cc = QColor(color).rgba();

  QString extra_c;
  if( noColor ) {
    extra_c = extra;
  } else {
    extra_c = color2style( i_cc, lw, extra );
  }
  pl_extra = extra_c.toStdString();

  pl_opt = opt.toStdString();


  switch( type ) {
    case GraphElem::DataType::DataNone :
      break;
    case GraphElem::DataType::DataAxisX:
      rl = LineRole::axisX;
      break;
    case GraphElem::DataType::DataAxisY :
      rl = LineRole::axisY;
      break;
    case GraphElem::DataType::DataAxisZ :
      rl = LineRole::axisZ;
      break;
    case GraphElem::DataType::DataPlot :
    case GraphElem::DataType::DataStep :
    case GraphElem::DataType::DataTape :
    case GraphElem::DataType::DataStem :
    case GraphElem::DataType::DataBars :
    case GraphElem::DataType::DataBarh :
    case GraphElem::DataType::DataTens :
    case GraphElem::DataType::DataArea :
    case GraphElem::DataType::DataRegion :
    case GraphElem::DataType::DataOHLC :
    case GraphElem::DataType::DataBoxPlot :
    case GraphElem::DataType::DataCandle :
    case GraphElem::DataType::DataCones :
    case GraphElem::DataType::DataError :
    case GraphElem::DataType::DataMark :
    case GraphElem::DataType::DataTube :
       rl = LineRole::plot; ig = igc;
       break;
    case GraphElem::DataType::DataSurf :
    case GraphElem::DataType::DataSurfC :
    case GraphElem::DataType::DataSurfA :
    case GraphElem::DataType::DataMesh :
    case GraphElem::DataType::DataFall :
    case GraphElem::DataType::DataBelt :
    case GraphElem::DataType::DataDens :
    case GraphElem::DataType::DataCont :
    case GraphElem::DataType::DataContF :
    case GraphElem::DataType::DataContD :
      rl = LineRole::plot; ig = igc;
      is2D = 1;
      break;
    case GraphElem::DataType::DataC0 :
      rl = LineRole::c0;
      break;
    case GraphElem::DataType::DataC1 :
      rl = LineRole::c1;
      break;
    case GraphElem::DataType::DataC2 :
      rl = LineRole::c2;
      break;
    case GraphElem::DataType::DataC3 :
      rl = LineRole::c3;
      break;
    case GraphElem::DataType::DataC4 :
      rl = LineRole::c4;
      break;
    case GraphElem::DataType::DataC5 :
      rl = LineRole::c5;
      break;
    default:
      qWarning() << "unknown type "<< (int)(type) << " label " << label << NWHE;
      break;
  }


  role = rl;
  return rl;
}

DEFAULT_FUNCS_REG(GraphElem);

// --------------------------------------------------------------------

const char* TGraph::helpstr = "<H1>TGraph</H1>\n"
 "Define, which outputs will be used for plotting and dumping. \n"
 "All outputs must be the same type";

STD_CLASSINFO(TGraph,clpSpecial);

CTOR(TGraph,TDataSet) ,
     scd( new ScaleData( "scd", this, 0, "scale", "scale data", "sep=blockend" ) )
{
  allowed_types = "GraphElem,GraphLabel,+SPECICAL";
  scd->setImmutable();
  reset();
}

TGraph::~TGraph()
{
  reset();
}

QVariant TGraph::dataObj( int col, int role ) const
{

  if( role == Qt::ToolTipRole ) {
    if( col != 0 ) {
      return QVariant();
    }
    QString s = QSL("[" );

    for( auto ge : TCHILD(GraphElem*) ) {
      if( ge->type == GraphElem::DataType::DataNone ) { continue; }
      s += ge->src.cval() % QSL( "," );
    }

    s += QSL("]" );
    return s;
  }

  // if( role == Qt::StatusTipRole ) { // used for button labels in dialogs
  //   if( col != 0 ) {
  //     return QVariant();
  //   }
  //
  // }

  return TDataSet::dataObj( col, role );
}

void TGraph::do_reset()
{
  nn = 0, nx = 0, ny = 1;    // common for all datas
  c_nn = nn; c_nx = nx; c_ny = ny;
  for( auto &tl : tli ) { tl = nullptr; }

  pli.clear();

  prepared = false; was_2D = false; need_c_axis = false;
  v_min = 0, v_max = 1; // cross!
}

int TGraph::prepare()
{
  reset();
  double vmin = DMAX, vmax = DMIN; // cross! (local copy of v_min, v_max)
  bool was_x = false, was_y = false;
  vector<double> ve_fx, ve_fy; // arrays for missing data

  for( auto ge : TCHILD(GraphElem*) ) {
    LineRole ro = ge->fillForPlot( nn, ny, pli.size() );

    if( ro == LineRole::none ) {  continue;   }

    tli[ro] = ge;

    if( ro == LineRole::axisX ) {
      was_x = true;  continue;
    }
    if( ro == LineRole::axisY ) {
      was_y = true;  continue;
    }

    if( ro == LineRole::plot ) {
      if( ge->type == GraphElem::DataType::DataSurfC ||
          ge->type == GraphElem::DataType::DataTens     ) {
        need_c_axis = true;
      }
      pli.push_back( ge );
      if( ge->is2D ) { was_2D = true; }
      if( ge->v_min < vmin ) {
        vmin = ge->v_min;
      }
      if( ge->v_max > vmax ) {
        vmax = ge->v_max;
      }
    }
  }
  nx = nn / ny;
  c_nn = nn; c_nx = nx; c_ny = ny;

  if( pli.size() < 1  ||  nn < 1 || nx < 1 ) {
    qWarning() << "nothing to plot: pli.size=" << (int)pli.size()
               << "nn=" << (int)nn << "nx=" << (int)nx << NWHE;
    return 0;
  }

  if( ! was_x ) { // create fake X array
    ve_fx.resize( nn );
    for( int j=0; j<ny; ++j ) {
      for( int i=0; i<nx; ++i ) {
        ve_fx[i+j*nx] = i;
      }
    }
    ge_fx  = make_shared<GraphElem>( "_fx",  nullptr, efInner, "fake X data", "fake X", "" );
    ge_fx->type = GraphElem::DataType::DataAxisX;
    ge_fx->is2D = was_2D; ge_fx->label = "X_n";
    ge_fx->role = LineRole::axisX;
    ge_fx->nn = nn; ge_fx->nx = nx; ge_fx->ny = ny;
    ge_fx->pl_label = "X_n";
    ge_fx->v_min = 0; ge_fx->v_max = nx-1.0;
    ge_fx->ve = &ve_fx;

    tli[LineRole::axisX] = ge_fx.get();
  }

  if( was_2D && !was_y ) { // make fake y array if at least 1 2D plot
    ve_fy.resize( nn );
    for( int j=0; j<ny; ++j ) {
      for( int i=0; i<nx; ++i ) {
        ve_fy[i+j*nx] = j;
      }
    }
    ge_fy  = make_shared<GraphElem>( "_fy",  nullptr, efInner, "fake Y data", "fake Y", "" );
    ge_fy->type = GraphElem::DataType::DataAxisY;
    ge_fy->is2D = was_2D; ge_fy->label = "Y_n";
    ge_fy->role = LineRole::axisY;
    ge_fy->nn = nn; ge_fy->nx = nx; ge_fy->ny = ny;
    ge_fy->pl_label = "Y_n";
    ge_fy->v_min = 0; ge_fy->v_max = ny;
    ge_fy->ve = &ve_fy;

    tli[LineRole::axisY] = ge_fy.get();
  }
  v_min = vmin; v_max = vmax;

  vector<uint8_t> plp( nn, 0 ); // flags: point worth to draw
  int np = fillSqueeze( plp );
  // qDebug() << "squeeze res nn= " << (int)nn << "np= " np << NWHE;

  // create fake zero array
  auto mdz = new mglData( nx, ny );
  for( int i=0; i<np; ++i ) { mdz->a[i] = 0.0; };
  ge_zero  = make_shared<GraphElem>( "_zero",  nullptr, efInner, "zero data", "zero", "" );
  ge_zero->type = GraphElem::DataType::DataNone;
  ge_zero->is2D = was_2D; ge_zero->label = "0!";
  ge_zero->role = LineRole::none;
  ge_zero->nn = nn; ge_zero->nx = nx; ge_zero->ny = ny;
  ge_zero->pl_label = "0!";
  ge_zero->v_min = 0; ge_zero->v_max = 0.1;
  ge_zero->ve = nullptr;
  ge_zero->md = mdz;

  for( auto ge : TCHILD(GraphElem*) ) { // copy squeezed data
    mglData *md = new mglData( nx, ny );
    ge->md = md;

    const dvector *ve = ge->ve;
    if( !ve ) {
      qWarning() << " no data, used fake in " << ge->getFullName() << WHE;
      continue;
    }

    for( int i=0, j=0; i<nn && j<np ; ++i ) {
      if( plp[i] ) {
        md->a[j] = (*ve)[i];
        ++j;
      }
    }
    ge->ve = nullptr; // unused from now
  }
  if( ge_fx ) { // the same for fake X, if exist. TODO: common array
    const dvector *ve = ge_fx->ve;
    auto md = new mglData( nx, ny );
    ge_fx->md = md;
    if( ve ) {
      for( int i=0, j=0; i<nn && j<np ; ++i ) {
        if( plp[i] ) {
          md->a[j] = (*(ge_fx->ve))[i];
          ++j;
        }
      }
    }
    ge_fx->ve = nullptr;
  }
  if( ge_fy ) { // the same for fake X, if exist. TODO: common array
    const dvector *ve = ge_fy->ve;
    auto md = new mglData( nx, ny );
    ge_fy->md = md;
    if( ve ) {
      for( int i=0, j=0; i<nn && j<np ; ++i ) {
        if( plp[i] ) {
          md->a[j] = (*(ge_fy->ve))[i];
          ++j;
        }
      }
    }
    ge_fy->ve = nullptr;
  }
  nn = np; nx = nn / ny;
  c_nn = nn; c_nx = nx; c_ny = ny;


  // all unfilled set to ge_zero
  // int iii = 0;
  for( auto &ge : tli ) {
    if( !ge ) {
      ge = ge_zero.get();
    }
    // DBGx( "dbg: %d %s ge \"%s\" %s nx: %d ny: %d min: %lf max :%lf",
    //     iii, role_name[iii], qP(ge->label), role_name[ge->role],
    //     (int)ge->md->nx, (int)ge->md->ny, ge->v_min, ge->v_max );
    // ++iii;
  }

  double x0 = tli[LineRole::axisX]->v_min, x1= tli[LineRole::axisX]->v_max;
  double y0 = v_min, y1 = v_max;
  if( was_2D ) {
    y0 = tli[LineRole::axisY]->v_min; y1= tli[LineRole::axisY]->v_max;
  }
  double z0 = tli[LineRole::axisZ]->v_min, z1= tli[LineRole::axisZ]->v_max;
  if( was_2D ) {
    z0 = v_min; z1 = v_max;
  }

  pr_min = { x0, y0, z0 };
  pr_max = { x1, y1, z1 };
  pr_dlt = pr_max - pr_min; // recalced in plot, but may be used before
  // default for effective, recalc on plot
  pe_min = pr_min; pe_max = pr_max; pe_dlt = pr_dlt;

  prepared = true;
  return pli.size();
}

int TGraph::fillSqueeze( vector<uint8_t> &plp )
{
  const int max_nn_nosqz = 2000;
  GraphElem *ge_x = tli[LineRole::axisX];
  if( !ge_x ) { // unlikely
    qWarning() << "warn: X axis not found!!!" << NWHE;
    return 0;
  }
  const dvector *ve_x = ge_x->ve;
  if( !ve_x ) { // unlikely
    qWarning() << "X axis array not found!!!" << NWHE;
    return 0;
  }

  if( nn <= max_nn_nosqz || ny != 1 ) {
    plp.assign( nn, 1 ); // all point are good
    return nn;
  }

  int np = 0; // number of selectd points;
  const int nd0 = 4; // inifial filling delta in pixels
  double mdlt = scd->maxErr * ( v_max - v_min ) / scd->h0; // max allowd delta
  int stp0 = nn * nd0 / scd->w0; // step for initial filling grid

  // initial grid
  for( int i=0; i<nn; i+=stp0 ) {
    plp[i] = 1;  ++np;
  }
  if( ! plp[nn-1] ) {
    plp[nn-1] = 1;  ++np;
  }
  // DBGx( "dbg: squeeze base: np= %d mdlt= %lf h0= %d stp0= %d v_min= %lf v_max= %lf",
  //       np, mdlt, (int)scd->h0, stp0, (double)v_min, (double)v_max );

  int was_add = 1;
  for( int n_add = 0; was_add && n_add < 10; ++n_add ) { // 10 : max iterations to split
    was_add = 0;
    for( auto ge : TCHILD(GraphElem*) ) {
      if( ge->role == LineRole::none || ge->role == LineRole::axisX ) { continue; }

      const dvector* yyc = ge->ve;
      if( ! yyc ) {
        // DBGx( "warn: no array in squeeze \"%s\"", qP(ge->label) );
        continue;
      }
      for( int i0 = 0, i1 = 0; i0 < nn-1; i0 = i1 ) {
        for( i1 = i0+1; !plp[i1] && i1 < nn; ++i1 ) /* NOP: find next set*/;
        double x0 = (*ve_x)[i0], x1 = (*ve_x)[i1];
        double y0 = (*yyc)[i0], y1 = (*yyc)[i1];
        if( x0 == x1 ) {
          continue;
        }
        double kxy = ( y1 - y0 ) / ( x1 - x0 );

        // find point of maximum delta
        int i_cm = 0; // index of current maximum
        double dly_cm = 0; // value of this max
        for( int i2=i0+1; i2<i1-1; ++i2 ) {
          double yc = y0 + kxy * ( (*ve_x)[i2] - x0 );
          double dly = fabs( yc - (*yyc)[i2] );
          if( dly > dly_cm ) {
            dly_cm = dly; i_cm = i2;
          }
        }
        if( dly_cm > mdlt  && ! plp[i_cm]  ) { // may be set by other
          plp[i_cm] = 1; ++np; ++was_add;
        }
      }
    }
    // DBGx( " dbg: np mid = %d  n_add= %d was_add= %d" , np, n_add, was_add );
  }

  nx = np; // as ny==1 was checked
  return np;
}


void TGraph::plotTo( mglGraph *gr, const ViewData *a_vd, const ScaleData *scda )
{
  if( !gr ) { return; }
  if( !scda ) { scda = scd; }
  if( !prepared  && prepare() < 1 ) {
    return;
  }

  gr->DefaultPlotParam();

  gr->SetFontSize( scda->fontSise );
  gr->SetPlotFactor( scda->plotFactor );
  gr->Rotate( scda->phi, scda->theta );
  gr->Light( scda->useLight );
  gr->SetAlphaDef( scda->alpha );
  gr->Alpha( (bool)(scda->useAlpha) );

  pr_dlt = pr_max - pr_min;
  pe_min = pr_min; pe_max = pr_max;

  if( ! scda->autoScX ) {
    pe_min.x = scda->plotMinX; pe_max.x = scda->plotMaxX;
  }
  if( ! scda->autoScY ) {
    pe_min.y = scda->plotMinY; pe_max.y = scda->plotMaxY;
  }
  if( ! scda->autoScZ ) {
    pe_min.z = scda->plotMinZ; pe_max.z = scda->plotMaxZ;
  }
  pe_dlt = pe_max - pe_min;


  ViewData vd;
  if( a_vd ) {
    vd = *a_vd;
  }
  mglPoint ve_dlt = pe_dlt / vd.mag; // realy * by coords
  mglPoint ve_min = pe_min + pe_dlt / vd.ofs;
  mglPoint ve_max = ve_min + ve_dlt;


  gr->SetRanges( ve_min, ve_max );
  if( need_c_axis ) {
    gr->SetRange( 'c', tli[LineRole::c0]->v_min, tli[LineRole::c0]->v_max );
    // DBGx( "dbg: C axis: %lf %lf", tli[LineRole::c0]->v_min, tli[LineRole::c0]->v_max  );
  }

  gr->SetTicks( 'x', -(scda->gridX), scda->tickX );
  gr->SetTicks( 'y', -(scda->gridY), scda->tickY );

  string axis_style = color2style( scda->axis_color.toInt(), 1 ).toStdString();
  string grid_style = color2style( scda->grid_color.toInt(), 1, "=" ).toStdString();

  QColor bg_c = scda->bgcolor;
  gr->Clf( bg_c.redF(), bg_c.greenF(), bg_c.blueF() );

  const mglData *d_x = tli[LineRole::axisX]->md;
  const mglData *d_y = tli[LineRole::axisY]->md;
  // const mglData *d_z = tli[LineRole::axisZ]->md;
  const mglData *d_c0 = tli[LineRole::c0]->md;

  int ig = 0;
  for( auto pl : pli ) {
    uint64_t msk = 1ull << ig;
    if( vd.off & msk ) {
      ++ig;
      continue;
    }

    const char *ext = pl->pl_extra.c_str();
    const char *opt = pl->pl_opt.c_str();

    switch( pl->type ) {
      case GraphElem::DataType::DataPlot :
        if( pl->is2D ) {
          gr->Plot( *d_x, *d_y, *(pl->md), ext, opt );
          break;
        }
        gr->Plot( *d_x, *(pl->md), ext, opt );
        break;

      case GraphElem::DataType::DataStep :
        if( pl->is2D ) {
          gr->Step( *d_x, *d_y, *(pl->md), ext, opt );
          break;
        }
        gr->Step( *d_x, *(pl->md), ext, opt );
        break;

      case GraphElem::DataType::DataTape :
        if( pl->is2D ) {
          gr->Tape( *d_x, *d_y, *(pl->md), ext, opt );
          break;
        }
        gr->Tape( *d_x, *(pl->md), ext, opt );
        break;

      case GraphElem::DataType::DataStem :
        if( pl->is2D ) {
          gr->Stem( *d_x, *d_y, *(pl->md), ext, opt );
          break;
        }
        gr->Stem( *d_x, *(pl->md), ext, opt );
        break;

      case GraphElem::DataType::DataBars :
        if( pl->is2D ) {
          gr->Bars( *d_x, *d_y, *(pl->md), ext, opt );
          break;
        }
        gr->Bars( *d_x, *(pl->md), ext, opt );
        break;

      case GraphElem::DataType::DataBarh :
        if( pl->is2D ) {
          gr->Barh( *(pl->md), *d_x, ext, opt );
          break;
        }
        gr->Barh( *(pl->md), *d_x, ext, opt );
        break;

      case GraphElem::DataType::DataTens : // C0: color
        if( pl->is2D ) {
          gr->Tens( *d_x, *d_y, *(pl->md), *d_c0, ext, opt );
          break;
        }
        gr->Tens( *d_x, *(pl->md), *d_c0, ext, opt );
        break;

      case GraphElem::DataType::DataArea :
        if( pl->is2D ) {
          gr->Area( *d_x, *d_y, *(pl->md), ext, opt );
          break;
        }
        gr->Area( *d_x, *(pl->md), ext, opt );
        break;
      case GraphElem::DataType::DataRegion :
        break; // unknown for now
      case GraphElem::DataType::DataOHLC :
        break; // unknown for now
      case GraphElem::DataType::DataBoxPlot :
        break; // unknown for now
      case GraphElem::DataType::DataCandle :
        break; // unknown for now
      case GraphElem::DataType::DataCones :
        break; // unknown for now
      case GraphElem::DataType::DataError :
        break; // unknown for now
      case GraphElem::DataType::DataMark : // C0: sz
        if( pl->is2D ) {
          gr->Mark( *d_x, *d_y, *(pl->md), *d_c0, ext, opt );
          break;
        }
        gr->Mark( *d_x, *(pl->md), *d_c0, ext, opt  );
        break;

      case GraphElem::DataType::DataTube : // C0: r
        if( pl->is2D ) {
          gr->Tube( *d_x, *d_y, *(pl->md), *d_c0, ext, opt );
          break;
        }
        gr->Tube( *d_x, *(pl->md), *d_c0, ext, opt );
        break;

      case GraphElem::DataType::DataSurf :
        gr->Surf( *d_x, *d_y, *(pl->md), ext, opt );
        break;
      case GraphElem::DataType::DataSurfC :
        gr->SurfC( *d_x, *d_y, *(pl->md), *d_c0, ext, opt );
        break;
      case GraphElem::DataType::DataSurfA :
        gr->SurfA( *d_x, *d_y, *(pl->md), *d_c0, ext, opt );
        break;
      case GraphElem::DataType::DataMesh :
        gr->Mesh( *d_x, *d_y, *(pl->md), ext, opt );
        break;
      case GraphElem::DataType::DataFall :
        gr->Fall( *d_x, *d_y, *(pl->md), ext, opt );
        break;
      case GraphElem::DataType::DataBelt :
        gr->Belt( *d_x, *d_y, *(pl->md), ext, opt );
        break;
      case GraphElem::DataType::DataDens :
        gr->Dens( *d_x, *d_y, *(pl->md), ext, opt );
        break;
      case GraphElem::DataType::DataCont :
        gr->Cont( *d_x, *d_y, *(pl->md), ext, opt );
        break;
      case GraphElem::DataType::DataContF : // C0: v
        gr->ContF( *d_c0, *d_x, *d_y, *(pl->md),  ext, opt );
        break;
      case GraphElem::DataType::DataContD : // C0: d
        gr->ContD( *d_c0, *d_x, *d_y, *(pl->md), ext, opt );
        break;
      default: break;
    }
    gr->AddLegend( pl->pl_label.c_str(), ext );
    ++ig;
  }



  if( scda->drawBox ) { gr->Box( axis_style.c_str() ); }
  if( scda->drawAxis ) {
    gr->Grid( "xyz", grid_style.c_str() );
    gr->Axis( "xyzU3AKDTVISO",  axis_style.c_str() );
    char main_label_axis = 'y';
    gr->Label( 'x', tli[LineRole::axisX]->pl_label.c_str() );
    if( was_2D ) {
      gr->Label( 'y', tli[LineRole::axisY]->pl_label.c_str() );
      main_label_axis = 'z';
    }
    if( ! scda->mainLabel.isEmpty() ) {
      gr->Label( main_label_axis, scda->mainLabel.c_str() );
    }
  }

  if( scda->drawMark ) {
    mglPoint mark_point( scda->markX, scda->markY, scda->markZ );
    gr->Mark( mark_point, "3r+" );
  }
  if( scda->drawBase ) {
    mglPoint base_point( scda->baseX, scda->baseY, scda->baseZ );
    gr->Error( base_point, pe_dlt, "B" );
  }
  if( scda->drawColorbar ) {
    gr->Colorbar();
  }

  if( scda->legend_pos < 4 ) {
    gr->Legend( scda->legend_pos, "#" );
  }
}

bool TGraph::fillViewData( ViewData *da )
{
  if( !da || !prepared ) {
    return false;
  }
  da->reset();
  da->pv_min = pr_min; da->pv_max = pr_max; da->pv_dlt = pr_dlt;
  da->ng = pli.size(); da->nn = nn; da->nx = nx; da->ny = ny;
  return true;
}

bool TGraph::getPointAt( int ig, int ip, mglPoint *p ) const
{
  if( !prepared || !p || ig<0 || ig >=(int)pli.size() || ip <0 || ip >= nn ) {
    return false;
  }
  double x = tli[LineRole::axisX]->md->a[ip];
  double y = 0, z = 0;
  if( was_2D ) {
    y = tli[LineRole::axisY]->md->a[ip];
    z = pli[ig]->md->a[ip];
  } else {
    y = pli[ig]->md->a[ip];
  }

  *p =  mglPoint( x, y, z );
  return true;
}

int TGraph::findNearest( const mglPoint &p, int ig ) const
{
  if( !prepared || ig<0 || ig >=(int)pli.size() ) {
    return -1;
  }
  mglPoint p0 = p, p1;

  if( !was_2D ) {
    p0.z = 0;
  }

  double l_min = DMAX; int i_min = 0;
  const mglData *d_x = tli[LineRole::axisX]->md;
  const mglData *d_y = tli[LineRole::axisY]->md;
  const mglData *d_v = pli[ig]->md;

  for( int i=0; i<nn; ++i ) {
    if( was_2D ) {
      p1 = { d_x->a[i], d_y->a[i], d_v->a[i] };
    } else {
      p1 = { d_x->a[i], d_v->a[i], 0 };
    }
    double l = mglLen( p0, p1 );
    if( l < l_min ) {
      l_min = l; i_min = i;
    }
  }

  return i_min;
}

void TGraph::viewForPoints( const mglPoint &p0, const mglPoint &p1, ViewData &vd ) const
{
  mglPoint pd = p1 - p0, p0e = p0;
  if( pd.norm() < 1e-5  ) {
    pd = 0.2 * pe_dlt / vd.mag;
    p0e -= 0.5 * pd;
  }
  vd.mag = mglPoint( pd.x / pe_dlt.x, pd.y / pe_dlt.y, pd.z / pe_dlt.z );
  mglPoint od = p0e - pe_min;
  vd.ofs = mglPoint( od.x / pe_dlt.x, od.y / pe_dlt.y, od.z / pe_dlt.z );
}

QString TGraph::getPlotLabel( int ig ) const
{
  if( ig < 0 || ig >= (int)pli.size() ) {
    return QString();
  }
  return pli[ig]->label;
}

QString TGraph::getPrintInfo( int ig ) const
{
  if( ig < 0 || ig >= (int)pli.size() ) {
    return QString();
  }
  return pli[ig]->md->PrintInfo();
}

void TGraph::plotToPng( const QString &fn )
{
  mglGraph gr( 0, scd->w0, scd->h0 );
  plotTo( &gr, nullptr, scd );
  string fn_s = fn.toStdString();
  gr.WritePNG( fn_s.c_str(), title.c_str() );
}

int TGraph::fillDatasInfo( DatasInfo *di ) const
{
  if( !di ) {
    return 0;
  }

  TModel *model = getAncestorT<TModel>();
  if( !model ) {
    qCritical() << "not found model " << NWHE;
    return 0;
  }

  di->reset();
  int nn = IMAX, ny = 0; // 0 is special: not found

  for( auto ge : TCHILD(GraphElem*) ) {
    QString s = ge->getDataD( "src", QString() );
    if( s.isEmpty() ) {
      continue;
    }

    TOutArr *arr = model->getOutArr( s );
    if( !arr ) {
      continue;
    }
    int nn_c = arr->getDataD( "n", 0 );
    if( nn_c < 1 ) {
      continue;
    }
    if( ny < 1 ) {
      ny = arr->getDataD( "ny", 0 ); // default 0 = not found / bad
    }
    const dvector *ve = arr->getArray();
    if( !ve ) {
      continue;
    }
    if( nn_c < nn ) {
      nn = nn_c;
    }
    QString lbl = ge->getDataD( "label",  QString( "v_%1" ).arg( QSN( di->ves.size() ) ) );
    di->labels.push_back( lbl );
    di->ves.push_back( ve );
  }

  if( di->ves.size() < 1 || nn < 1 ) {
    return 0;
  }
  if( ny < 1 ) { // unlikely
    ny = 1;
  }

  di->title = objectName();
  di->nn = nn; di->nx = nn / ny; di->ny = ny;

  return nn;
}


int  TGraph::dump( const QString &fn, const QString &delim )
{
  int n;
  DatasInfo di;
  n = fillDatasInfo( &di );
  if( !n ) {
    return 0;
  }

  QFile of( fn );
  if( ! of.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
    qWarning() << "fail to open file "<< fn << NWHE;
    return 0;
  }
  QTextStream os( &of );

  n = di.dump( os, delim );
  return n;
}

int TGraph::addOutArr( const QString &o_name )
{
  static constexpr const unsigned dc_num = 14;
  static const unsigned def_col[dc_num] =
    {        0, 0x880000, 0x008800, 0x000088, 0x888800, 0x880088, 0x000088,
      0x404040, 0xAA4444, 0x44AA44, 0x4444AA, 0xAAAA44, 0xAA44AA, 0x4444AA };

  bool was_x = false, was_y = false;

  int ne = 0; // plot elements
  for( auto ge : TCHILD(GraphElem*) ) {
    int dtype = ge->getDataD( "type", (int)GraphElem::DataType::DataNone );
    if( dtype == GraphElem::DataType::DataAxisX ) {
      was_x = true;
      continue;
    }
    if( dtype == GraphElem::DataType::DataAxisY ) {
      was_y = true;
      continue;
    }
    if( dtype >= GraphElem::DataType::DataPlot && dtype < GraphElem::DataType::DataC0 ) {
      ++ne;
    }
  }

  // find good name
  QString nm0 = "y", nm = "y0";
  if( ! was_x ) {
    nm0 = nm = "x";
  } else if( was_y ) {
    nm0 = "z"; nm = "z0";
  }

  for( int j=1; j<100; ++j ) {
    if( ! getElemT<GraphElem*>( nm ) ) {
      break;
    }
    nm = nm0 + QSN( j );
  }

  GraphElem *ge = addElemT<GraphElem>( nm );
  if( !ge ) {
    qWarning() << " Fail to create GraphElem " <<  nm << NWHE;
    return 0;
  }
  ge->setData( "src", o_name );
  int dtype = GraphElem::DataType::DataPlot;
  if( !was_x ) {
    dtype = GraphElem::DataType::DataAxisX;
  }
  ge->setData( "type", dtype );
  unsigned nc = ne;
  if( nc >= dc_num ) {
    nc = 0;
  }
  ge->setData( "color", def_col[nc] );

  // now we have element, but can improve it

  TModel *mod = getAncestorT<TModel>();
  if( !mod ) {
    return 1;
  }

  TOutArr *arr = mod->getOutArr( o_name );
  if( !arr ) {
    return 1;
  }
  QString lbl = arr->getDataD( "label", nm );
  ge->setData( "label", lbl );
  int otype = arr->getDataD( "type", 0 );
  if( otype == TOutArr::OutArrType::outParm2 ) {
    ge->setData( "is2D", 1 );
  }


  return 1;
}

// ---------------- misc funcs -------------------------- 

QString color2style( int color, int lw, const QString &extra )
{
  QColor cc = QRgb( color );
  QString s_cc;
  s_cc.sprintf( "%d{x%02X%02X%02X}", lw, cc.red(),  cc.green(), cc.blue() );
  s_cc += extra;
  return s_cc;
}

QString toQString( const mglPoint &p )
{
  QString s = QString( "[ %1; %2; %3 ]" ).arg( p.x ).arg( p.y ).arg( p.z );
  return s;
}

double mglLen( const mglPoint &a, const mglPoint &b )
{
  return sqrt( (b.x-a.x)*(b.x-a.x) +(b.y-a.y)*(b.y-a.y) +(b.z-a.z)*(b.z-a.z) );
}



DEFAULT_FUNCS_REG(TGraph);

