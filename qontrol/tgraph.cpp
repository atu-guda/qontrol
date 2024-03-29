/***************************************************************************
                          tgraph.cpp  -  description
                             -------------------
    begin                : Sat Aug 5 2000
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

#include <gsl/gsl_math.h>
#include <mgl2/base.h>

#include "tmodel.h"
#include "miscfun.h"
#include "tgraph.h"
#include "toutarr.h"
#include "rootdata.h"

#include <QPainter>
#include <QTextDocument>

using namespace std;

const char* GraphElem::helpstr = "<H1>GraphElem</H1>\n"
 "Defines one source and output type for one  line";

const char *role_name[] = {
  "none", "X axis", "Y axis", "Z axis", "Plot", "C0", "C1", "C2", "C3", "C4", "C5", "Sz", "??1"
};

STD_CLASSINFO(GraphElem,0);

CTOR(GraphElem,TDataSet)
{
}

GraphElem::~GraphElem()
{
  reset();
}

QVariant GraphElem::dataObj( int col, int a_role ) const
{
  if( a_role == Qt::StatusTipRole ) { // used for button labels in dialogs
    if( col > 1 ) {
      return QVariant();
    }
    // if( col == 1 ) {
    //   return TDataSet::dataObj( col, a_role ); // for buttons
    // }

    // real statustip + buttons
    QString s = objectName() % QSL(":") % src.cval();
    QStringList el = getEnumStrings( QSL("DataType") );
    s += QSL(" (") % el.at( type );
    if( is2D ) {
      s += QSL(",2D");
    }
    if( ! extra.isEmpty() ) {
      s += QSL(",") % extra.cval();
    }
    s += QSL(") ");
    if( ! label.isEmpty() ) {
      s += QSL(" \"" ) % label.cval() % QSL( "\"" );
    }
    return s;
  }

  return TDataSet::dataObj( col, a_role );
}


void GraphElem::do_reset()
{
  role = LineRole::none;
  nn = 0; nx = 0; ny = 1; ig = -1;
  delete md; md = nullptr;
  ve = nullptr;
}

LineRole GraphElem::fillForPlot( long &g_nn, long &g_ny, int igc )
{
  LineRole rl = LineRole::none;
  ig = -1;
  TModel *model = getAncestorT<TModel>();
  if( !model ) {
    qCritical() << QSL("model not found!") << NWHE;
    return rl;
  }
  if( src.isEmpty() ) {
    return LineRole::none;
  }
  TOutArr *arr = model->getOutArr( src );
  if( !arr ) {
    return rl;
  }
  if( ! arr->isAllFinite() ) {
    return LineRole::none;
  }

  nn = arr->getDataD( QSL("n"), 0 );
  if( nn < 1 ) {
    return rl;
  }
  ny = arr->getDataD( QSL("ny"), 1 );
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

  v_min = arr->getDataD( QSL("dmin"), 0.0 );
  v_max = arr->getDataD( QSL("dmax"), 0.0 );

  if( label == QSL(".") ||  label == QSL(" ") ) {
    pl_label = label.toStdString();
  } else {
    QString label_c = QSL("\\i{"); // TODO: better!
    label_c += label.isEmpty() ? QSL("y") : label;
    label_c += QSL("}");
    pl_label = label_c.toStdString();
  }

  int i_cc = QColor(color).rgba();

  QString extra_c;
  if( noColor ) {
    extra_c = extra.cval();
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
      qWarning() << QSL("unknown type ") << (int)(type) << QSL(" label ") << label << NWHE;
      break;
  }


  role = rl;
  return rl;
}

DEFAULT_FUNCS_REG(GraphElem);

// --------------------------------------------------------------------
const char* PlotLabel::helpstr = "<H1>PlotLabel</H1>\n"
 "Label for plot";


STD_CLASSINFO(PlotLabel,0);

CTOR(PlotLabel,LinkedObj)
{
  needReadInputsRecurse = true;
}

PlotLabel::~PlotLabel()
{
}

QString PlotLabel::textVisual() const
{
  prepareText();
  return labelWithSubst;
}

void PlotLabel::do_post_set()
{
  LinkedObj::do_post_set();
  handleStructChanged();
  readInputs();
}

void PlotLabel::prepareText() const
{
  if( labelReady ) {
    return;
  }

  QString s = text.cval();
  if( substVals ) {
    QString s1 = s
      .replace( QRegExp( QSL("\\$X\\b") ), QSN( labelX.cval() ) )
      .replace( QRegExp( QSL("\\$Y\\b") ), QSN( labelY.cval() ) )
      .replace( QRegExp( QSL("\\$Z\\b") ), QSN( labelZ.cval() ) );
    TModel *model = getAncestorT<TModel>();
    s = substValues( s1, model );
  }

  if( labelType == LabelMiniTeX ) {
    labelWithSubst = tex2label( s, false );
  } else {
    labelWithSubst = s;
  }
  labelReady = true;
}

QString PlotLabel::getMetaStr() const
{
  prepareText();
  if( addToMeta ) {
    return labelWithSubst;
  }
  return QString();
}

QString PlotLabel::getFnAddStr() const
{
  prepareText();
  if( addToName ) {
    return QSL("_") % labelWithSubst % QSL("_");
  }
  return QString();
}

bool PlotLabel::render( QImage *img, mglGraph *gr, bool onGr ) const
{
  if(  onGr && labelType != LabelMGL ) { return false; }

  // if( !onGr && labelType == LabelMGL ) { return false; } // not here, need cross

  prepareText();

  if( ! drawLabel ) {
    return true;
  }

  if( !img || !gr ) { return false; }

  QPoint p0 {0, 0};
  int w = img->width(), h = img->height();
  mglPoint p0m { labelX, labelY, labelZ };

  mglPoint p_min { 0, 0, 0 };
  mglPoint p_max { 1, 1, 1 };
  mglPoint p0p   { 0, 0, 0 };
  const mglBase *gb = gr->Self();
  if( gb ) {
    p_min = gb->Min;  p_max = gb->Max;
  }

  switch( coordType ) {
    case CoordGraph:
      p0m.x = p_min.x + labelX * ( p_max.x - p_min.x );
      p0m.y = p_min.y + labelY * ( p_max.y - p_min.y );
      p0m.z = p_min.z + labelZ * ( p_max.z - p_min.z );
      p0p = CalcScr( p0m, *gr );
      p0 = QPoint( p0p.x, p0p.y );
      break;
    case CoordScreen:
      p0 = QPoint( (int)( w * labelX ), (int)( h * ( 1.0 - labelY ) ) );
      p0m = gr->CalcXYZ( p0.x(), p0.y() );
      break;
    case CoordFirst:
      p0p = CalcScr( p0m, *gr );
      p0 = QPoint( p0p.x, p0p.y );
      break;
    default:
      break;
  }

  // debug coordinate tranform
  // QString ds = QSL("pMin: " ) % toQString( p_min )  % QSL("  pMax: " ) % toQString( p_max  );
  // qWarning() << ds << WHE;
  // ds = QSL("p0m: " ) % toQString( p0m )  % QSL("  p0p: " ) % toQString( p0p  );
  // qWarning() << ds << NWHE;

  if( drawCross ) {
    QPainter px( img ); // TODO create common
    px.setPen( crossColor.cval() );
    px.drawLine( p0.x()-3,   p0.y(), p0.x()+3, p0.y()   );
    px.drawLine(   p0.x(), p0.y()-3,   p0.x(), p0.y()+3 );
  }

  bool rc = false;
  switch( labelType ) {
    case LabelPlain:
      rc = renderPlain( img, p0 );
      break;
    case LabelHTML:
      rc = renderHTML( img, p0 );
      break;
    case LabelMiniTeX:
      rc = renderMiniTeX( img, p0 );
      break;
    case LabelMGL:
      if( onGr ) {
        rc = renderMGL( gr, p0m );
      } else {
        rc = true;
      }
      break;
    case LabelTeX:
      rc = renderTeX( img, p0 );
      break;
    default:
      qWarning() << QSL("Unknown label type: ") << labelType << NWHE;
      break;
  }

  if( drawCross ) {
    QPainter px( img ); // TODO create common
    px.setPen( crossColor.cval() );
    px.drawLine( p0.x()-3,   p0.y(), p0.x()+3, p0.y()   );
    px.drawLine(   p0.x(), p0.y()-3,   p0.x(), p0.y()+3 );
  }

  return rc;
}

bool PlotLabel::renderPlain( QImage *img, QPoint p0 ) const
{
  if( !img ) { return false; }
  QPainter px( img );
  int w = img->width(), h = img->height();
  px.setFont( labelFont.cval() );

  int xl = p0.x(), yl = p0.y();

  QRect r0( xl, yl, w-xl, h-yl );
  QRect brect = px.boundingRect( r0, Qt::AlignLeft, labelWithSubst );
  int r_h = brect.height();
  brect.translate( 0, 2-r_h );  // More space is above
  brect += QMargins( 1, 1, 1, 1 ); // TODO: config
  brect &= QRect( 0, 0, w, h );

  if( drawBG ) {
    px.fillRect( brect, labelBgColor.cval() );
  }

  px.setPen( labelColor.cval() );
  if( drawFrame ) {
    px.drawRect( brect );
  }

  px.drawText( xl, yl, labelWithSubst );

  // if( xl > 0 && yl > 0 && xl < w-2  && yl < h-2 ) {
  //   px.fillRect( xl-1, yl-1, 2, 2, Qt::red ); // Debug: point 0
  // }
  // px.drawRect( r0 );

  return false;
}

bool PlotLabel::renderHTML( QImage *img, QPoint p0 ) const
{
  if( !img ) { return false; }
  QPainter px( img );
  int w = img->width(), h = img->height();
  int xl = p0.x(), yl = p0.y();
  int lw = (int)( w * labelW );
  int lh = (int)( h * labelH );

  // if( xl > 0 && yl > 0 && xl < w-2  && yl < h-2 ) {
  //   px.fillRect( xl-1, yl-1, 2, 2, Qt::red ); // Debug: point 0
  // }
  yl -= lh;

  px.translate( xl, yl );
  QRect r0( 0, 0, lw, lh );

  if( drawBG ) {
    px.fillRect( r0, labelBgColor.cval() );
  }
  px.setPen( labelColor.cval() );
  if( drawFrame ) {
    px.drawRect( r0 );
  }

  r0 += QMargins( -1, -1, -1, -1 ); // TODO: config

  px.setClipRect( r0, Qt::ReplaceClip );

  QTextDocument ldoc;
  ldoc.setUndoRedoEnabled( false );
  ldoc.setDefaultFont( labelFont.cval() );
  ldoc.setHtml( labelWithSubst );
  ldoc.drawContents( &px );
  return true;
}

bool PlotLabel::renderMiniTeX( QImage *img, QPoint p0 ) const
{
  return renderHTML( img, p0 ); // conversion is done beforehand
}

bool PlotLabel::renderMGL( mglGraph *gr, const mglPoint &p0m ) const
{
  if( !gr ) { return false;  }
  wstring ws = labelWithSubst.toStdWString();
  string fs = labelFontMGL.toStdString();

  gr->Putsw( p0m, ws.data(),  fs.c_str(), labelSizeMGL.cval() );
  // gr->Ball( p0m, 'r' );

  return true;
}

bool PlotLabel::renderTeX( QImage *img, QPoint p0 ) const
{
  return renderMiniTeX( img, p0 ); // TMP: TODO: real external TeX
}


DEFAULT_FUNCS_REG(PlotLabel);


// --------------------------------------------------------------------
const char* PlotFlippery::helpstr = "<H1>PlotFlippery</H1>\n"
 "Flipperies for the plot";


STD_CLASSINFO(PlotFlippery,0);

CTOR(PlotFlippery,LinkedObj)
{
  needReadInputsRecurse = true;
}

PlotFlippery::~PlotFlippery()
{
}

void PlotFlippery::do_post_set()
{
  LinkedObj::do_post_set();
  handleStructChanged(); // to re-read changed values after dialog TODO: separate fun
  readInputs();
}

bool PlotFlippery::render( mglGraph *gr ) const
{
  if( !gr ) {
    return false;
  }
  if( ! drawFlippery ) {
    return true;
  }
  mglPoint p0( x0, y0, z0 );
  mglPoint p1( x1, y1, z1 );
  mglPoint p2( x2, y2, z2 );
  mglPoint p3( x3, y3, z3 );

  string style_str = flStyle.toStdString();
  const char *style = style_str.c_str();
  char sc0 = style[0];


  switch( flType ) {

    case FlipperyBall:
      gr->Ball( p0, sc0 );
      break;

    case FlipperyCircle:
      gr->Circle( p0, v_0, style );
      break;

    case FlipperySphere:
      gr->Sphere( p0, v_0, style );
      break;

    case FlipperyEllipse:
      gr->Ellipse( p0, p1, v_0, style );
      break;

    case FlipperyDrop:
      gr->Drop( p0, p1, v_0, style, v_1, v_2 );
      break;

    case FlipperyLine:
      gr->Line( p0, p1, style, nl );
      break;

    case FlipperyError:
      gr->Error( p0, p1, style );
      break;

    case FlipperyCurve:
      gr->Curve( p0, p1, p2, p3, style );
      break;

    case FlipperyFace:
      gr->Face( p0, p1, p2, p3, style );
      break;

    case FlipperyRect:
      gr->Face( p0, p1, p2, p3, style ); // TODO: fix
      break;

    case FlipperyCone:
      gr->Cone( p0, p1, v_0, v_1, style );
      break;

    case FlipperyRhomb:
      gr->Rhomb( p0, p1, v_0, style );
      break;

    case FlipperyArc:
      gr->Arc( p0, p1, p2, v_0, style );
      break;

    case FlipperyPolygon:
      gr->Polygon( p0, p1, nl, style );
      break;

    default:
      break;
  }

  return true;
}


DEFAULT_FUNCS_REG(PlotFlippery);

// --------------------------------------------------------------------

const char* TGraph::helpstr = "<H1>TGraph</H1>\n"
 "Define, which outputs will be used for plotting and dumping. \n"
 "All outputs must be the same type";

STD_CLASSINFO(TGraph,clpSpecial);

CTOR(TGraph,LinkedObj) ,
     scd( new ScaleData( QSL("scd"), this, 0, QSL("scale"), QSL("scale data"), QSL("sep=blockend") ) ),
     tli( LineRole::sz, nullptr )
{
  allowed_types = QSL("GraphElem,PlotLabel,PlotFlippery,+SPECICAL");
  scd->setImmutable();
  needReadInputsRecurse = true;
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
    return textVisual();
  }

  // if( role == Qt::StatusTipRole ) { // used for button labels in dialogs
  //   if( col != 0 ) {
  //     return QVariant();
  //   }
  //
  // }

  return TDataSet::dataObj( col, role );
}

QString TGraph::textVisual() const
{
  QString s { QSL("[") }, sep;
  for( auto ge : TCHILD(GraphElem*) ) {
    if( ge->type == GraphElem::DataType::DataNone ) { continue; }
    s += sep % ge->src.cval();
    sep = QSL(",");
  }
  s += QSL("](" ) % QSN(nx) % QSL("x") % QSN(ny) % QSL(")");
  return s;
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
  readAllInputs();
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
    qWarning() << "nothing to plot: pli.size=" << (long)pli.size()
               << "nn=" << (long)nn << "nx=" << (long)nx << NWHE;
    return 0;
  }

  if( ! was_x ) { // create fake X array
    ve_fx.resize( nn );
    for( long j=0; j<ny; ++j ) {
      for( long i=0; i<nx; ++i ) {
        ve_fx[i+j*nx] = i;
      }
    }
    ge_fx  = make_shared<GraphElem>( QSL("_fx"),  nullptr, efInner, QSL("fake X data"), QSL("fake X"), QES );
    ge_fx->type = GraphElem::DataType::DataAxisX;
    ge_fx->is2D = was_2D; ge_fx->label = QSL("X_n");
    ge_fx->role = LineRole::axisX;
    ge_fx->nn = nn; ge_fx->nx = nx; ge_fx->ny = ny;
    ge_fx->pl_label = "X_n";
    ge_fx->v_min = 0; ge_fx->v_max = nx - 1.0;
    ge_fx->ve = &ve_fx;

    tli[LineRole::axisX] = ge_fx.get();
  }

  if( was_2D && !was_y ) { // make fake y array if at least 1 2D plot
    ve_fy.resize( nn );
    for( long j=0; j<ny; ++j ) {
      for( long i=0; i<nx; ++i ) {
        ve_fy[i+j*nx] = j;
      }
    }
    ge_fy  = make_shared<GraphElem>( QSL("_fy"),  nullptr, efInner, QSL("fake Y data"), QSL("fake Y"), QES );
    ge_fy->type = GraphElem::DataType::DataAxisY;
    ge_fy->is2D = was_2D; ge_fy->label = QSL("Y_n");
    ge_fy->role = LineRole::axisY;
    ge_fy->nn = nn; ge_fy->nx = nx; ge_fy->ny = ny;
    ge_fy->pl_label = "Y_n";
    ge_fy->v_min = 0; ge_fy->v_max = ny;
    ge_fy->ve = &ve_fy;

    tli[LineRole::axisY] = ge_fy.get();
  }
  v_min = vmin; v_max = vmax;

  vector<uint8_t> plp( nn, 0 ); // flags: point worth to draw
  long np = fillSqueeze( plp );
  // qDebug() << "squeeze res nn= " << (int)nn << "np= " np << NWHE;

  // create fake zero array
  auto mdz = new mglData( nx, ny );
  for( long i=0; i<np; ++i ) { mdz->a[i] = 0.0; };
  ge_zero  = make_shared<GraphElem>( QSL("_zero"),  nullptr, efInner, QSL("zero data"), QSL("zero"), QES );
  ge_zero->type = GraphElem::DataType::DataNone;
  ge_zero->is2D = was_2D; ge_zero->label = QSL("0!");
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

    for( long i=0, j=0; i<nn && j<np ; ++i ) {
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
      for( long i=0, j=0; i<nn && j<np ; ++i ) {
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
      for( long i=0, j=0; i<nn && j<np ; ++i ) {
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
  // long iii = 0;
  for( auto &ge : tli ) {
    if( !ge ) {
      ge = ge_zero.get();
    }
    // DBGx( "dbg: %d %s ge \"%s\" %s nx: %d ny: %d min: %lf max :%lf",
    //     iii, role_name[iii], qP(ge->label), role_name[ge->role],
    //     (long)ge->md->nx, (long)ge->md->ny, ge->v_min, ge->v_max );
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

  // extend plot area to all Flipperies
  for( const auto flp : TCHILD(PlotFlippery*) ) {
    const double fl_x0 = flp->getDataD( QSL("x0" ), x0 );
    const double fl_y0 = flp->getDataD( QSL("y0" ), y0 );
    const double fl_z0 = flp->getDataD( QSL("z0" ), z0 );
    if( fl_x0 < x0 ) { x0 = fl_x0; }; // TODO: common action
    if( fl_x0 > x1 ) { x1 = fl_x0; };
    if( fl_y0 < y0 ) { y0 = fl_y0; };
    if( fl_y0 > y1 ) { y1 = fl_y0; };
    if( fl_z0 < z0 ) { z0 = fl_z0; };
    if( fl_z0 > z1 ) { z1 = fl_z0; };
  }

  pr_min = { x0, y0, z0 };
  pr_max = { x1, y1, z1 };
  pr_dlt = pr_max - pr_min; // recalced in plot, but may be used before
  // default for effective, recalc on plot
  pe_min = pr_min; pe_max = pr_max; pe_dlt = pr_dlt;

  prepared = true;
  return pli.size();
}

long TGraph::fillSqueeze( vector<uint8_t> &plp )
{
  const long max_nn_nosqz = 2000;
  GraphElem *ge_x = tli[LineRole::axisX];
  if( !ge_x ) { // unlikely
    qWarning() << QSL("warn: X axis not found!!!") << NWHE;
    return 0;
  }
  const dvector *ve_x = ge_x->ve;
  if( !ve_x ) { // unlikely
    qWarning() << QSL("X axis array not found!!!") << NWHE;
    return 0;
  }

  if( nn <= max_nn_nosqz || ny != 1 ) {
    plp.assign( nn, 1 ); // all point are good
    return nn;
  }

  long np = 0; // number of selectd points;
  const int nd0 = 4; // inifial filling delta in pixels
  double mdlt = scd->maxErr * ( v_max - v_min ) / scd->h0; // max allowd delta
  long stp0 = nn * nd0 / scd->w0; // step for initial filling grid

  // initial grid
  for( long i=0; i<nn; i+=stp0 ) {
    plp[i] = 1;  ++np;
  }
  if( ! plp[nn-1] ) {
    plp[nn-1] = 1;  ++np;
  }
  // DBGx( "dbg: squeeze base: np= %d mdlt= %lf h0= %d stp0= %d v_min= %lf v_max= %lf",
  //       np, mdlt, (long)scd->h0, stp0, (double)v_min, (double)v_max );

  int was_add = 1;
  for( long n_add = 0; was_add && n_add < 10; ++n_add ) { // 10 : max iterations to split
    was_add = 0;
    for( auto ge : TCHILD(GraphElem*) ) {
      if( ge->role == LineRole::none || ge->role == LineRole::axisX ) { continue; }

      const dvector* yyc = ge->ve;
      if( ! yyc ) {
        // DBGx( "warn: no array in squeeze \"%s\"", qP(ge->label) );
        continue;
      }
      for( long i0 = 0, i1 = 0; i0 < nn-1; i0 = i1 ) {
        for( i1 = i0+1; !plp[i1] && i1 < nn; ++i1 ) /* NOP: find next set*/;
        double x0 = (*ve_x)[i0], x1 = (*ve_x)[i1];
        double y0 = (*yyc)[i0], y1 = (*yyc)[i1];
        if( x0 == x1 ) {
          continue;
        }
        double kxy = ( y1 - y0 ) / ( x1 - x0 );

        // find point of maximum delta
        long i_cm = 0; // index of current maximum
        double dly_cm = 0; // value of this max
        for( long i2=i0+1; i2<i1-1; ++i2 ) {
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

void TGraph::addMetaData( QImage &img, const ViewData *a_vd, const ScaleData *scda ) const
{
  QString model_file = QSL("Unknown");
  TRootData *root = getAncestorT<TRootData>();
  if( root ) {
    model_file = root->getFilePath();
  }
  QString sim_name = QSL("Unknown");
  TModel *model = getAncestorT<TModel>();
  if( model ) {
    Simulation *sim = model->getActiveSimulation();
    if( sim ) {
      sim_name = sim->objectName();
    }
  }
  img.setText( QSL("Graph"),      objectName() );
  img.setText( QSL("Model_file"), model_file );
  img.setText( QSL("Simulation"), sim_name );
  img.setText( QSL("Title"),      title.cval() );
  img.setText( QSL("Decription"), descr.cval() );
  img.setText( QSL("Creator"),    QSL(PACKAGE "-" VERSION) );

  if( a_vd ) {
    img.setText( QSL("x_min"), QSND( a_vd->pv_min.x ) );
    img.setText( QSL("x_max"), QSND( a_vd->pv_max.x ) );
    img.setText( QSL("y_min"), QSND( a_vd->pv_min.y ) );
    img.setText( QSL("y_max"), QSND( a_vd->pv_max.y ) );
    img.setText( QSL("z_min"), QSND( a_vd->pv_min.z ) );
    img.setText( QSL("z_max"), QSND( a_vd->pv_max.z ) );
  }

  if( scda ) {
    img.setText( QSL("phi"),    QSND( scda->phi.cval()        ) );
    img.setText( QSL("theta"),  QSND( scda->theta.cval()      ) );
    img.setText( QSL("factor"), QSND( scda->plotFactor.cval() ) );
  }

  for( auto lbl : TCHILD(PlotLabel*) ) {
    QString s = lbl->getMetaStr();
    if( s.isEmpty() ) { continue; }
    img.setText( QSL("label_") % lbl->objectName(), s.toLatin1() );
  }
}

QSize TGraph::renderTo( QImage &img, const ViewData *a_vd, const ScaleData *scda )
{
  int w = img.width(), h = img.height();
  img.fill( 0xFF );

  if( !prepared  && prepare() < 1 ) {
    return QSize( w, h );
  }

  if( !scda ) { scda = scd; }
  ViewData vd;
  if( a_vd ) { vd = *a_vd; } // default values

  mglGraph gr( 0, w, h );
  int gw = gr.GetWidth(), gh = gr.GetHeight();
  // qWarning() << "w= " << w << " gw= " << gw << " h= " << h << " gh= " << gh << WHE;
  setupMglGraph( gr, &vd, scda, true );

  plotTo( gr, &vd, scda );

  for( auto flp : TCHILD(PlotFlippery*) ) {
    flp->render( &gr );
  }

  if( a_vd->show_labels ) {
    for( auto lbl : TCHILD(PlotLabel*) ) {
      lbl->render( &img, &gr, true ); // only MGL plots
    }
  }

  gr.GetBGRN( img.bits(), 4 * w * h );

  if( a_vd->show_labels ) {
    for( auto lbl : TCHILD(PlotLabel*) ) {
      lbl->render( &img, &gr, false );
    }
  }

  addMetaData( img, &vd, scda );

  return QSize( gw, gh );
}

void TGraph::plotTo( mglGraph &gr, const ViewData *a_vd, const ScaleData *scda )
{
  if( !scda || ! a_vd ) { return; }
  if( !prepared  && prepare() < 1 ) {
    return;
  }

  string axis_style = color2style( scda->axis_color.toInt(), 1 ).toStdString();
  string grid_style = color2style( scda->grid_color.toInt(), 1, "=" ).toStdString();

  QColor bg_c = scda->bgcolor;
  gr.Clf( bg_c.redF(), bg_c.greenF(), bg_c.blueF() );


  d_x = tli[LineRole::axisX]->md;
  d_y = tli[LineRole::axisY]->md;
  // d_z = tli[LineRole::axisZ]->md;
  d_c0 = tli[LineRole::c0]->md;

  if( ! scda->mglScript.isEmpty() ) {
    mglParse parser;
    string script_str = scda->mglScript.toStdString();
    parser.Execute( &gr, script_str.c_str() );
  }

  int ig = -1;
  int sel = a_vd->sel;
  for( auto pl : pli ) {
    ++ig;
    uint64_t msk = 1ull << ig;
    bool is_selected = sel == ig;
    bool is_masked = a_vd->off & msk;
    auto lbl = pl->pl_label + " ("s +  to_string( ig ) + ")"s;
    if( is_selected ) {
      lbl += is_masked ? '.' : '*';
    } else {
      lbl += is_masked ? '-' : ' ';
    }
    const char *clbl = lbl.c_str();
    // qWarning() << "clbl= " << clbl << NWHE;
    if( ! ( pl->pl_label == "."s || pl->pl_label == " "s  ) ) {
      gr.AddLegend( clbl, pl->pl_extra.c_str() );
    }
    if( is_selected ) { // selected plotted last
      continue;
    }

    if( ! is_masked ) {
      plot1( gr, pl );
    }
  }

  if( sel >=0  &&  sel < (int)pli.size() ) {
    plot1( gr, pli[sel] );
  }

  gr.SetTuneTicks( scda->tuneTick.cval(), scda->multPos.cval() );

  if( scda->drawBox ) { gr.Box( axis_style.c_str() ); }
  gr.SetRotatedText( false );

  if( scda->drawAxis ) {
    char main_label_axis = 'y';
    gr.Label( 'x', tli[LineRole::axisX]->pl_label.c_str() );
    string a_type;
    if( was_2D ) {
      gr.Grid( "xyz", grid_style.c_str() );
      gr.Label( 'y', tli[LineRole::axisY]->pl_label.c_str() );
      main_label_axis = 'z';
      a_type = "xyz"s + scda->axisType.toStdString();
    } else {
      gr.Grid( "xy", grid_style.c_str() );
      a_type = "xy"s + scda->axisType.toStdString();
    }
    gr.Axis( a_type.c_str(),  axis_style.c_str() );

    if( ! scda->mainLabel.isEmpty() ) {
      gr.SetFontSize( scda->fontSise * scda->mainScale );
      if( main_label_axis == 'y' ) {
        mglPoint p0m { 0.0, 1.0, 0.0 };
        mglPoint p_min { 0, 0, 0 };
        mglPoint p_max { 1, 1, 1 };
        const mglBase *gb = gr.Self();
        if( gb ) {
          p_min = gb->Min;  p_max = gb->Max;
        }
        p0m.x = p_min.x + -0.10 * ( p_max.x - p_min.x );
        p0m.y = p_min.y +  1.04 * ( p_max.y - p_min.y );
        p0m.z = 0;
        wstring ws = scda->mainLabel.toStdWString();
        gr.Putsw( p0m, ws.data(), ":L", 3 );
      } else {
        string s =  scda->mainLabel.toStdString();
        gr.Label( main_label_axis, s.c_str() );
      }
      gr.SetFontSize( scda->fontSise );
    }
  }


  mglPoint mark_point( scda->markX, scda->markY, scda->markZ );
  if( scda->drawMark ) {
    gr.Mark( mark_point, "3r+" );
  }
  if( scda->drawBase ) {
    mglPoint base_point( scda->baseX, scda->baseY, scda->baseZ );
    gr.Error( base_point, pe_dlt, "B" );
    if( scda->drawMark ) {
      gr.Line( base_point, mark_point, "b=", 1 );
    }
  }
  if( scda->drawColorbar ) {
    gr.Colorbar();
  }

  int l_pos = scda->legend_pos;
  //                                  0    1    2    3     4     5    6     7     8    9
  static const char* lg_t_chars[] = { "", "#", "-", "#-", "r", "r#", "r-", "r#=", "", " ", "?", "?" };
  gr.Push();
  gr.SetFontSize( scda->fontSise * scda->legend_scale );
  if( l_pos < 4 ) { // predefined pos 0-3
    gr.Legend( l_pos, "#" );
  } else if ( l_pos >= 10000  &&  l_pos <= 99999 ) { // manual coords: tXXYY
    int ltype = l_pos / 10000;
    l_pos %= 10000;
    int l_pos_x = ( l_pos / 100 ) - 25; // some magic to handle wierd MGL legend coords
    int l_pos_y = ( l_pos % 100 ) - 25;
    gr.Legend( 2.0*l_pos_x/100.0, 2.0*l_pos_y/100.0, lg_t_chars[ltype] );
    gr.ClearLegend(); // try to reset styles
    gr.AddLegend("","");
    gr.Legend( 2.0, 2.0, "" );
  }
  gr.SetFontSize( scda->fontSise ); // back??
  gr.Pop();

}

void TGraph::plot1( mglGraph &gr, const GraphElem *pl )
{
  const char *ext = pl->pl_extra.c_str();
  const char *opt = pl->pl_opt.c_str();

  switch( pl->type ) {
    case GraphElem::DataType::DataPlot :
      if( pl->is2D ) {
        gr.Plot( *d_x, *d_y, *(pl->md), ext, opt );
        break;
      }
      gr.Plot( *d_x, *(pl->md), ext, opt );
      break;

    case GraphElem::DataType::DataStep :
      if( pl->is2D ) {
        gr.Step( *d_x, *d_y, *(pl->md), ext, opt );
        break;
      }
      gr.Step( *d_x, *(pl->md), ext, opt );
      break;

    case GraphElem::DataType::DataTape :
      if( pl->is2D ) {
        gr.Tape( *d_x, *d_y, *(pl->md), ext, opt );
        break;
      }
      gr.Tape( *d_x, *(pl->md), ext, opt );
      break;

    case GraphElem::DataType::DataStem :
      if( pl->is2D ) {
        gr.Stem( *d_x, *d_y, *(pl->md), ext, opt );
        break;
      }
      gr.Stem( *d_x, *(pl->md), ext, opt );
      break;

    case GraphElem::DataType::DataBars :
      if( pl->is2D ) {
        gr.Bars( *d_x, *d_y, *(pl->md), ext, opt );
        break;
      }
      gr.Bars( *d_x, *(pl->md), ext, opt );
      break;

    case GraphElem::DataType::DataBarh :
      if( pl->is2D ) {
        gr.Barh( *(pl->md), *d_x, ext, opt );
        break;
      }
      gr.Barh( *(pl->md), *d_x, ext, opt );
      break;

    case GraphElem::DataType::DataTens : // C0: color
      if( pl->is2D ) {
        gr.Tens( *d_x, *d_y, *(pl->md), *d_c0, ext, opt );
        break;
      }
      gr.Tens( *d_x, *(pl->md), *d_c0, ext, opt );
      break;

    case GraphElem::DataType::DataArea :
      if( pl->is2D ) {
        gr.Area( *d_x, *d_y, *(pl->md), ext, opt );
        break;
      }
      gr.Area( *d_x, *(pl->md), ext, opt );
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
        gr.Mark( *d_x, *d_y, *(pl->md), *d_c0, ext, opt );
        break;
      }
      gr.Mark( *d_x, *(pl->md), *d_c0, ext, opt  );
      break;

    case GraphElem::DataType::DataTube : // C0: r
      if( pl->is2D ) {
        gr.Tube( *d_x, *d_y, *(pl->md), *d_c0, ext, opt );
        break;
      }
      gr.Tube( *d_x, *(pl->md), *d_c0, ext, opt );
      break;

    case GraphElem::DataType::DataSurf :
      gr.Surf( *d_x, *d_y, *(pl->md), ext, opt );
      break;
    case GraphElem::DataType::DataSurfC :
      gr.SurfC( *d_x, *d_y, *(pl->md), *d_c0, ext, opt );
      break;
    case GraphElem::DataType::DataSurfA :
      gr.SurfA( *d_x, *d_y, *(pl->md), *d_c0, ext, opt );
      break;
    case GraphElem::DataType::DataMesh :
      gr.Mesh( *d_x, *d_y, *(pl->md), ext, opt );
      break;
    case GraphElem::DataType::DataFall :
      gr.Fall( *d_x, *d_y, *(pl->md), ext, opt );
      break;
    case GraphElem::DataType::DataBelt :
      gr.Belt( *d_x, *d_y, *(pl->md), ext, opt );
      break;
    case GraphElem::DataType::DataDens :
      gr.Dens( *d_x, *d_y, *(pl->md), ext, opt );
      break;
    case GraphElem::DataType::DataCont :
      gr.Cont( *d_x, *d_y, *(pl->md), ext, opt );
      break;
    case GraphElem::DataType::DataContF : // C0: v
      gr.ContF( *d_c0, *d_x, *d_y, *(pl->md),  ext, opt );
      break;
    case GraphElem::DataType::DataContD : // C0: d
      gr.ContD( *d_c0, *d_x, *d_y, *(pl->md), ext, opt );
      break;
    default: break;
  }
}

void TGraph::setupMglGraph( mglGraph &grs, const ViewData *a_vd, const ScaleData *scda, bool full )
{
  grs.DefaultPlotParam();

  grs.SetFontSize( scda->fontSise );
  grs.SetPlotFactor( scda->plotFactor );
  grs.Rotate( scda->phi, scda->theta );

  pr_dlt = pr_max - pr_min;
  pe_min = pr_min; pe_max = pr_max;

  if( scda->autoScX ) {
    if( scda->beauScX ) {
      beautifyScale( pe_min.x, pe_max.x );
    }
  } else {
    pe_min.x = scda->plotMinX; pe_max.x = scda->plotMaxX;
  }

  if( scda->autoScY ) {
    if( scda->beauScY ) {
      beautifyScale( pe_min.y, pe_max.y );
    }
  } else {
    pe_min.y = scda->plotMinY; pe_max.y = scda->plotMaxY;
  }

  if( scda->autoScZ ) {
    if( scda->beauScY ) {
      beautifyScale( pe_min.z, pe_max.z );
    }
  } else {
    pe_min.z = scda->plotMinZ; pe_max.z = scda->plotMaxZ;
  }
  pe_dlt = pe_max - pe_min;


  ViewData vd;
  if( a_vd ) {
    vd = *a_vd;
  }
  ve_dlt = pe_dlt / vd.mag; // realy * by coords
  ve_min = pe_min + pe_dlt / vd.ofs;
  ve_max = ve_min + ve_dlt;

  grs.SetRanges( ve_min, ve_max );

  if( full ) {
    if( need_c_axis ) {
      grs.SetRange( 'c', tli[LineRole::c0]->v_min, tli[LineRole::c0]->v_max );
      // qWarning() << "dbg: C axis: " << tli[LineRole::c0]->v_min << tli[LineRole::c0]->v_max << NWHE;
    }

    grs.Light( scda->useLight );
    grs.SetAlphaDef( scda->alpha );
    grs.Alpha( (bool)(scda->useAlpha) );
    grs.SetTicks( 'x', -(scda->gridX), scda->tickX );
    grs.SetTicks( 'y', -(scda->gridY), scda->tickY );
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

bool TGraph::getPointAt( int ig, long ip, mglPoint *p ) const
{
  if( !prepared || !p || ! isGoodIndex( ig, pli ) || ! isInBounds( 0l, ip, nn-1 ) ) {
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

long TGraph::findNearest( const mglPoint &p, int ig ) const
{
  if( !prepared || ! isGoodIndex( ig, pli ) ) {
    return -1;
  }
  // mglPoint p0 = p, p1;
  //
  // if( !was_2D ) {
  //   p0.z = 0;
  // }

  double l_min = DMAX; long i_min = 0;
  const mglData *d_x = tli[LineRole::axisX]->md;
  // const mglData *d_y = tli[LineRole::axisY]->md;
  // const mglData *d_v = pli[ig]->md;

  // for( long i=0; i<nn; ++i ) {
  //   if( was_2D ) {
  //     p1 = { d_x->a[i], d_y->a[i], d_v->a[i] };
  //   } else {
  //     p1 = { d_x->a[i], d_v->a[i], 0 };
  //   }
  //   double l = mglLen( p0, p1 );
  //   if( l < l_min ) {
  //     l_min = l; i_min = i;
  //   }
  // }

  const double xp = p.x;
  for( long i=0; i<nn; ++i ) {
    const double l = fabs( xp - d_x->a[i] );
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
  if( ! isGoodIndex( ig, pli ) ) {
    return QString();
  }
  return pli[ig]->label;
}

QString TGraph::getPrintInfo( int ig ) const
{
  if( ! isGoodIndex( ig, pli ) ) {
    return QString();
  }
  return pli[ig]->md->PrintInfo();
}

mglPoint TGraph::CalcXYZ( int mx, int my, int w, int h,
    const ViewData *a_vd, const ScaleData *scda )
{
  if( !scda ) { scda = scd; }
  ViewData vd;
  if( a_vd ) { vd = *a_vd; }

  mglGraph gr( 0, w, h );
  setupMglGraph( gr, &vd, scda, false );
  return gr.CalcXYZ( mx, my ); // main return

  // mglPoint r = gr.CalcXYZ( mx, my ); // ----------- test CalcScr
  // mglPoint s = CalcScr( r, gr );
  // qWarning() << "mx= " << mx << " my= " << my << " r= " << toQString( r )
  //            << " s= " << toQString( s ) << WHE;
  // return r;
}




void TGraph::plotToPng( const QString &fn )
{
  QString efn = fn.isEmpty() ? (hintFileName() % QSL(".png")) : fn;

  reset(); // to reread all
  int w =  scd->w0, h = scd->h0;
  QImage timg( w, h, QImage::Format_RGB32 );
  renderTo( timg, nullptr, scd );

  if( ! timg.save( efn, "PNG", 50 ) ) {
    qWarning() << QSL("Fail to save png for plot") << NWHE;
  }
}

int TGraph::fillDatasInfo( DatasInfo *di ) const
{
  if( !di ) {
    return 0;
  }

  TModel *model = getAncestorT<TModel>();
  if( !model ) {
    qCritical() << QSL("model not found!") << NWHE;
    return 0;
  }

  di->reset();
  long nn = LMAX, ny = 0; // 0 is special: not found

  for( auto ge : TCHILD(GraphElem*) ) {
    const QString s = ge->getDataD( QSL("src"), QString() );
    if( s.isEmpty() ) {
      continue;
    }

    TOutArr *arr = model->getOutArr( s );
    if( !arr ) {
      continue;
    }
    long nn_c = arr->getDataD( QSL("n"), 0l );
    if( nn_c < 1 ) {
      continue;
    }
    if( ny < 1 ) {
      ny = arr->getDataD( QSL("ny"), 0l ); // default 0 = not found / bad
    }
    const dvector *ve = arr->getArray();
    if( !ve ) {
      continue;
    }
    if( nn_c < nn ) {
      nn = nn_c;
    }
    QString lbl = ge->getDataD( QSL("label"),  QSL("v_%1").arg( QSN( di->ves.size() ) ) );
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
    qWarning() << QSL("fail to open file ") << fn << NWHE;
    return 0;
  }
  QTextStream os( &of );

  n = di.dump( os, delim );
  return n;
}

int  TGraph::dumpPlotted( const QString &fn, const QString &delim )
{
  if( !prepared  && prepare() < 1 ) {
    qWarning() << QSL("Fail prepare data!") << NWHE;
    return 0;
  }

  QFile of( fn );
  if( ! of.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
    qWarning() << QSL("fail to open file ") << fn << NWHE;
    return 0;
  }
  QTextStream os( &of );

  bool is2D =  pli[0]->is2D;
  const int out_prec = 10;

  os << qSetFieldWidth( out_prec + 7 );
  os << ( QSL("# ") + tli[LineRole::axisX]->label + delim );
  if( is2D ) {
    os << ( tli[LineRole::axisY]->label + delim );
  }
  for( const auto pl : pli ) {
    os << ( delim + pl->label );
  }
  os << Qt::endl;

  int n = 0;
  for( int i=0; i<nn; ++i ) {
    os << ( QSNL( d_x->a[i], out_prec ) );
    if( is2D ) {
      os << ( delim + QSNL( d_x->a[i], out_prec ) );
    }
    for( const auto pl : pli ) {
      os << ( delim + QSNL( (pl->md)->a[i], out_prec ) );
    }
    os << Qt::endl;
  }

  return n;
}

int TGraph::addOutArr( const QString &o_name )
{
  static constexpr const unsigned dc_num = 14;
  static const unsigned def_col[dc_num] =
    { 0xFF000000, 0xFF880000, 0xFF008800, 0xFF000088, 0xFF888800, 0xFF880088, 0xFF000088,
      0xFF404040, 0xFFAA4444, 0xFF44AA44, 0xFF4444AA, 0xFFAAAA44, 0xFFAA44AA, 0xFF4444AA };

  bool was_x = false, was_y = false;

  int ne = 0; // plot elements
  for( auto ge : TCHILD(GraphElem*) ) {
    int dtype = ge->getDataD( QSL("type"), (int)GraphElem::DataType::DataNone );
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
  QString nm0 = QSL("y"), nm = QSL("y0");
  if( ! was_x ) {
    nm0 = nm = QSL("x");
  } else if( was_y ) {
    nm0 = QSL("z"); nm = QSL("z0");
  }

  for( int j=1; j<100; ++j ) {
    if( ! getObjT<GraphElem*>( nm ) ) {
      break;
    }
    nm = nm0 + QSN( j );
  }

  GraphElem *ge = addObjT<GraphElem>( nm );
  if( !ge ) {
    qWarning() << QSL("Fail to create GraphElem") <<  nm << NWHE;
    return 0;
  }
  ge->setData( QSL("src"), o_name );
  int dtype = GraphElem::DataType::DataPlot;
  if( !was_x ) {
    dtype = GraphElem::DataType::DataAxisX;
  }
  ge->setData( QSL("type"), dtype );
  unsigned nc = ne;
  if( nc >= dc_num ) {
    nc = 0;
  }
  ge->setData( QSL("color"), def_col[nc] );

  // now we have element, but can improve it

  TModel *mod = getAncestorT<TModel>();
  if( !mod ) {
    return 1;
  }

  TOutArr *arr = mod->getOutArr( o_name );
  if( !arr ) {
    return 1;
  }
  QString lbl = arr->getDataD( QSL("label"), nm );
  ge->setData( QSL("label"), lbl );
  int otype = arr->getDataD( QSL("type"), 0 );
  if( otype == TOutArr::OutArrType::outParm2 ) {
    ge->setData( QSL("is2D"), 1 );
  }


  return 1;
}

QString TGraph::hintFileName() const
{
  QString base = QSL("x");
  TRootData *root = getAncestorT<TRootData>();
  if( root ) {
    base = root->getFileBase();
  }
  base += QSL("-") % objectName();

  for( auto lbl : TCHILD(PlotLabel*) ) {
    QString s = lbl->getFnAddStr();
    if( s.isEmpty() ) { continue; }
    base += s.replace( '\\', '_' );
  }

  return base; // %  QSL(".png");
}

void TGraph::addLabel( const mglPoint &p, const QString &txt )
{
  auto nm = hintName( QSL("PlotLabel"), QSL("albl_") );
  auto pl = addObjT<PlotLabel>( nm );
  if( !pl ) {
    return;
  }
  pl->setD( QSL("text"), txt );
  pl->setD( QSL("substVals"), 1 );
  pl->setD( QSL("labelX"), p.x );
  pl->setD( QSL("labelY"), p.y );
  pl->setD( QSL("labelZ"), p.z );
  pl->setD( QSL("coordType"), PlotLabel::CoordType::CoordFirst );
  pl->setD( QSL("drawCross"), 1 );
}

// ---------------- misc funcs -------------------------- 

QString color2style( int color, int lw, const QString &extra )
{
  QColor cc = QRgb( color );
  QString s_cc = QString::asprintf( "%d{x%02X%02X%02X}", lw, cc.red(),  cc.green(), cc.blue() );
  s_cc += extra;
  return s_cc;
}

QString toQString( const mglPoint &p )
{
  QString s = QSL("[ %1; %2; %3 ]").arg( p.x ).arg( p.y ).arg( p.z );
  return s;
}

double mglLen( const mglPoint &a, const mglPoint &b )
{
  return gsl_hypot3( (b.x-a.x), (b.y-a.y), (b.z-a.z) );
}


mglPoint CalcScr( const mglPoint &p, mglGraph &gr )
{
  // gr must be set beforehand
  mglBase *base = gr.Self();

  mglPoint s = gr.CalcScr( p );

  // out of axis case: CalcScr is bad
  if( ( s.x < -1e6  || s.x > 1e6 ) &&  base != nullptr ) {
    double pf = fabs( (base->GetB())->pf ) + 0.01;
    const double alp = 0.8 / pf;
    mglPoint p_mid = 0.5 * ( base->Min + base->Max ); // real coord of center
    mglPoint s_mid = gr.CalcScr( p_mid );  // screed coord of center
    mglPoint p_x = alp * p + ( 1-alp ) * p_mid;
    mglPoint s_x = gr.CalcScr( p_x );
    s = ( 1.0 / alp ) * ( s_x - ( 1-alp ) * s_mid );
  }
  s.y = gr.GetHeight() - s.y; // < -hack to fix

  return s;
}

DEFAULT_FUNCS_REG(TGraph);

