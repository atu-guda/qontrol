#pragma once
/***************************************************************************
                          tgraph.h  -  description
                             -------------------
    begin                : Sat Aug 5 2000
    copyright            : (C) 2000-2016 by atu
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


#include <memory>

#include <QColor>
#include <QString>

#include <mgl2/mgl.h>

#include "linkedobj.h"
#include "scaledata.h"

class DatasInfo;
class TGraph;

enum LineRole {
  none = 0, axisX, axisY, axisZ, plot, c0, c1, c2, c3, c4, c5, sz
};

//* converts color and width to style
QString color2style( int color, int lw = 1, const QString &extra = QString() );
//* converts MathGL point to QString
QString toQString( const mglPoint &p );
//* distance beween points
double mglLen( const mglPoint &a, const mglPoint &b );
//* workaround about bad mgl CalcScr
mglPoint CalcScr( const mglPoint &p, mglGraph &gr );

//* decription of one line in plot

class GraphElem : public TDataSet {
   Q_OBJECT
   friend class TGraph; // to direct data access while plotting
 public:
   DCL_CTOR(GraphElem);
   virtual ~GraphElem() override;
   virtual QVariant dataObj( int col, int role = Qt::DisplayRole ) const override;
   DCL_CREATE;
   DCL_STD_INF;

   enum DataType {
     DataNone = 0, // just to ignore data
     DataAxisX, DataAxisY, DataAxisZ, // coords (Z unused for now)
     DataPlot,  DataStep, DataTape,
     DataStem, DataBars, DataBarh, DataTens, DataArea,
     DataRegion, DataOHLC, DataBoxPlot, DataCandle, DataCones,
     DataError, DataMark, DataTube,
     DataSurf, DataSurfC, DataSurfA,
     DataMesh, DataFall, DataBelt, DataDens,
     DataCont, DataContF, DataContD,
     DataC0, DataC1, DataC2, DataC3, DataC4, DataC5, // aux data for misc plots
   };
   Q_ENUMS( DataType );
   Q_CLASSINFO( "enum_DataType_0",   "None   " );     // DataNone
   Q_CLASSINFO( "enum_DataType_1",   "AxisX  " );     // DataAxisX
   Q_CLASSINFO( "enum_DataType_2",   "AxisY  " );     // DataAxisY
   Q_CLASSINFO( "enum_DataType_3",   "AxisZ  " );     // DataAxisZ
   Q_CLASSINFO( "enum_DataType_4",   "Plot   " );     // DataPlot
   Q_CLASSINFO( "enum_DataType_5",   "Step   " );     // DataStep
   Q_CLASSINFO( "enum_DataType_6",   "Tape   " );     // DataTape
   Q_CLASSINFO( "enum_DataType_7",   "Stem   " );     // DataStem
   Q_CLASSINFO( "enum_DataType_8",   "Bars   " );     // DataBars
   Q_CLASSINFO( "enum_DataType_9",   "Barh   " );     // DataBarh
   Q_CLASSINFO( "enum_DataType_10",  "Tens  1" );     // DataTens
   Q_CLASSINFO( "enum_DataType_11",  "Area   " );     // DataArea
   Q_CLASSINFO( "enum_DataType_12",  "Region 1" );    // DataRegion
   Q_CLASSINFO( "enum_DataType_13",  "OHLC  3" );     // DataOHLC
   Q_CLASSINFO( "enum_DataType_14",  "BoxPlot 5" );   // DataBoxPlot
   Q_CLASSINFO( "enum_DataType_15",  "Candle  3" );   // DataCandle
   Q_CLASSINFO( "enum_DataType_16",  "Cones  " );     // DataCones
   Q_CLASSINFO( "enum_DataType_17",  "Error 2" );     // DataError
   Q_CLASSINFO( "enum_DataType_18",  "Mark  1" );     // DataMark
   Q_CLASSINFO( "enum_DataType_19",  "Tube   " );     // DataTube
   Q_CLASSINFO( "enum_DataType_20",  "Surf 2D" );     // DataSurf
   Q_CLASSINFO( "enum_DataType_21",  "SurfC 2D" );    // DataSurfC
   Q_CLASSINFO( "enum_DataType_22",  "SurfA 2D" );    // DataSurfA
   Q_CLASSINFO( "enum_DataType_23",  "Mesh 2D" );     // DataMesh
   Q_CLASSINFO( "enum_DataType_24",  "Fall 2D" );     // DataFall
   Q_CLASSINFO( "enum_DataType_25",  "Belt 2D" );     // DataBelt
   Q_CLASSINFO( "enum_DataType_26",  "Dens 2D" );     // DataDens
   Q_CLASSINFO( "enum_DataType_27",  "Cont 2D" );     // DataCont
   Q_CLASSINFO( "enum_DataType_28",  "ContF 2D" );    // DataContF
   Q_CLASSINFO( "enum_DataType_29",  "ContD 2D" );    // DataContD
   Q_CLASSINFO( "enum_DataType_30",   "C0     " );    // DataC0
   Q_CLASSINFO( "enum_DataType_31",   "C1     " );    // DataC1
   Q_CLASSINFO( "enum_DataType_32",   "C2     " );    // DataC2
   Q_CLASSINFO( "enum_DataType_33",   "C3     " );    // DataC3
   Q_CLASSINFO( "enum_DataType_34",   "C4     " );    // DataC4
   Q_CLASSINFO( "enum_DataType_35",   "C5     " );    // DataC5     

 public:
   //* fills inner data, required for plot (seed inner:)
   LineRole fillForPlot( long &g_nn, long &g_ny, int igc );

 protected:
   virtual void do_reset() override;

   PRM_LIST(   type,  efNRC, "Type", "Data Type", "enum=DataType" );
   PRM_SWITCH( is2D,  efNRC, "2D", "Data for 2D plot", "" );
   PRM_STRING( src,   efNRC, "Source", "Name of source for values", "max=128\ncmpl=out" );
   PRM_STRING( label, efNRC, "Label", "Label", "max=128" );
   PRM_COLOR(  color, efNRC, "line color", "plot line color", "def=black" );
   PRM_INT(    lw,    efNRC, "line width", "plot line width", "def=1\nmin=0\nmax=9" );
   PRM_SWITCH( noColor,efNRC, "NO color", "ignore color and line width", "" );
   PRM_STRING( extra, efNRC, "Extra", "Extra pen params to plot", "max=128\ndef=-" );
   PRM_STRING( opt,   efNRC, "Options", "Options to plot", "max=128" );

   // inner: prepare to plot
   int ig = -1; // index
   LineRole role = LineRole::none;
   long nn = 0, nx = 0, ny = 1;
   std::string pl_label = "";
   std::string pl_extra = "";
   std::string pl_opt = "";
   double v_min = 0, v_max = 0;
   mglData *md = nullptr;
   const std::vector<double> *ve = nullptr; // until md is filled, but not owning

   DCL_DEFAULT_STATIC;
};


// --------------------------------------------------------------------

//* decription of view
struct ViewData {
  mglPoint mag { 1, 1, 1 }, pv_min { 0, 0, 0 }, pv_max{ 1, 1, 1 }, pv_dlt { 1, 1, 1 };
  mglPoint ofs { 0, 0, 0 };
  void reset() { mag.x = mag.y = mag.z = 1; ofs.x = ofs.y = ofs.z = 0; off = 0; sel = 0;};
  int  ng {0}; // only from TGraph to View
  long nn {0}, nx {0}, ny {1}; // same
  uint64_t off = 0; // lines, not to show (from View to TGraph while plot)
  int sel {0};
};

// --------------------------------------------------------------------


class PlotLabel : public LinkedObj {
   Q_OBJECT
 public:
   DCL_CTOR(PlotLabel);
   virtual ~PlotLabel() override;
   // virtual QVariant dataObj( int col, int role = Qt::DisplayRole ) const override;
   DCL_CREATE;
   DCL_STD_INF;
   virtual void do_reset() override { labelReady = false; labelWithSubst = QString(); }
   virtual void do_post_set() override;

   enum CoordType {
     CoordGraph = 0,
     CoordScreen,
     CoordFirst    // main coorrd system, a-la gnuplot
   };
   Q_ENUMS( CoordType );
   Q_CLASSINFO( "enum_CoordType_0",   "Graph " );    // CoordGraph
   Q_CLASSINFO( "enum_CoordType_1",   "Screen" );    // CoordScreen
   Q_CLASSINFO( "enum_CoordType_2",   "First " );    // CoordFirst

   enum LabelType {
     LabelPlain = 0,
     LabelHTML,
     LabelMiniTeX,
     LabelMGL,
     LabelTeX
   };
   Q_ENUMS( LabelType );
   Q_CLASSINFO( "enum_LabelType_0",   "Plain " );    // LabelPlain
   Q_CLASSINFO( "enum_LabelType_1",   "HTML" );      // LabelHTML,
   Q_CLASSINFO( "enum_LabelType_2",   "MiniTeX" );   // LabelMiniTeX
   Q_CLASSINFO( "enum_LabelType_3",   "MGL" );       // LabelMGL,
   Q_CLASSINFO( "enum_LabelType_4",   "TeX" );       // labelTeX

   virtual bool render( QImage *img, mglGraph *gr, bool onGr ) const;

   Q_INVOKABLE const QString& getLabelWithSubst() const { return labelWithSubst; }
   Q_INVOKABLE QString getMetaStr() const;
   Q_INVOKABLE QString getFnAddStr() const;

 protected:
   PRM_STRING( text, 0, "Text", "label text", "props=STRING,MLINE\nncol=-1\ndef=Label");

   PRM_LIST( labelType, 0, "Label type",  "method of label processing", "sep=block\nenum=LabelType" );
   PRM_SWITCH( substVals, 0, "Subst", "Substitute values", "def=0" );
   PRM_SWITCH( addToName, 0, "add to name", "add to default filename", "def=0" );
   PRM_SWITCH( addToMeta, 0, "add to meta", "add to file mealdata", "def=0" );

   PRM_SWITCH( drawLabel, 0, "Draw", "Draw label", "def=1\nsep=col" );
   PRM_SWITCH( drawFrame, 0, "Frame", "Draw frame around label", "def=0" );
   PRM_SWITCH( drawBG,  0, "Background", "Draw label background", "def=0" );

   PRM_COLOR( labelColor, 0, "Color", "Label color", "sep=col\ndef=black" );
   PRM_COLOR( labelBgColor, 0, "BgColor", "Label background color", "def=white" );
   PRM_FONT( labelFont, 0, "Font", "label font", "sep=col");
   PRM_STRING( labelFontMGL, 0, "MGL Font", "label font in MGL mode", "def=:L@");
   PRM_DOUBLE( labelSizeMGL, 0, "MGL Size", "label font size in MGL mode", "def=2.0");

   PRM_PARAMD( labelX, 0, "X", "label X coordinate", "def=0.9\nsep=block");
   PRM_PARAMD( labelY, 0, "Y", "label Y coordinate", "def=0.9\nsep=col");
   PRM_PARAMD( labelZ, 0, "Z", "label Z coordinate", "def=0.0\nsep=col");
   PRM_LIST( coordType, 0, "Coord",  "coordinate system for label", "sep=col\nenum=CoordType" );
   PRM_DOUBLE( labelW, 0, "W", "label width", "def=0.20\nsep=block");
   PRM_DOUBLE( labelH, 0, "H", "label height","def=0.15\nsep=col");

   void prepareText() const;
   bool renderPlain( QImage *img, QPoint p0 ) const;
   bool renderHTML( QImage *img, QPoint p0 ) const;
   bool renderMiniTeX( QImage *img, QPoint p0 ) const;
   bool renderMGL( mglGraph *gr, const mglPoint &p0m ) const;
   bool renderTeX( QImage *img, QPoint p0 ) const;

   mutable bool labelReady = false;
   mutable QString labelWithSubst;

   Q_CLASSINFO( "nameHintBase",  "lbl_" );
   DCL_DEFAULT_STATIC;
};

// --------------------------------------------------------------------


class PlotFlippery : public LinkedObj {
   Q_OBJECT
 public:
   DCL_CTOR(PlotFlippery);
   virtual ~PlotFlippery() override;
   DCL_CREATE;
   DCL_STD_INF;

   enum FlipperyType {
     FlipperyBall = 0,
     FlipperyCircle,
     FlipperySphere,
     FlipperyEllipse,
     FlipperyDrop,
     FlipperyLine,
     FlipperyError,
     FlipperyCurve,
     FlipperyFace,
     FlipperyRect,
     FlipperyCone,
     FlipperyRhomb,
     FlipperyArc,
     FlipperyPolygon
   };
   Q_ENUMS( FlipperyType );
   Q_CLASSINFO( "enum_FlipperyType_0",   "Ball(p0)"        );         // FlipperyBall
   Q_CLASSINFO( "enum_FlipperyType_1",   "Circle(p0,v_0)"  );         // FlipperyCircle
   Q_CLASSINFO( "enum_FlipperyType_2",   "Sphere(p0,v_0) " );         // FlipperySphere
   Q_CLASSINFO( "enum_FlipperyType_3",   "Ellipse(p0,p1,v_0)" );      // FlipperyEllipse
   Q_CLASSINFO( "enum_FlipperyType_4",   "Drop(p0,p1,v_0,v_1,v_2)" ); // FlipperyDrop
   Q_CLASSINFO( "enum_FlipperyType_5",   "Line (p0,p1,nl) " );        // FlipperyLine
   Q_CLASSINFO( "enum_FlipperyType_6",   "Error(p0,p1)" );            // FlipperyError
   Q_CLASSINFO( "enum_FlipperyType_7",   "Curve(p0,p1,p2,p3)" );      // FlipperyCurve
   Q_CLASSINFO( "enum_FlipperyType_8",   "Face(p0,p1,p2,p3)" );       // FlipperyFace
   Q_CLASSINFO( "enum_FlipperyType_9",   "Rect?   " );                // FlipperyRect
   Q_CLASSINFO( "enum_FlipperyType_10",  "Cone(p0,p1,v_0,v_1)" );     // FlipperyCone
   Q_CLASSINFO( "enum_FlipperyType_11",  "Rhomb(p0,p1,v_0)" );        // FlipperyRhomb
   Q_CLASSINFO( "enum_FlipperyType_12",  "Arc(p0,p1,p2,v_0)" );       // FlipperyArc
   Q_CLASSINFO( "enum_FlipperyType_13",  "Polygon( p0,p1,nl)" );      // FlipperyPolygon

   virtual bool render( mglGraph *gr ) const;


 protected:
   virtual void do_post_set() override;
   PRM_LIST( flType, 0, "Flippery type",  "Flippery type", "enum=FlipperyType" );
   PRM_STRING( flStyle, 0, "Style", "Flippery style in MGL terms", "sep=col\ndef=S@");

   PRM_PARAMD( x0, 0, "x0", "x0 coordinate", "def=0.1\nsep=block");
   PRM_PARAMD( y0, 0, "y0", "y0 coordinate", "def=0.5\nsep=col");
   PRM_PARAMD( z0, 0, "z0", "z0 coordinate", "def=0.0\nsep=col");

   PRM_PARAMD( x1, 0, "x1", "x1 coordinate", "def=0.3\nsep=block");
   PRM_PARAMD( y1, 0, "y1", "y1 coordinate", "def=0.7\nsep=col");
   PRM_PARAMD( z1, 0, "z1", "z1 coordinate", "def=0.0\nsep=col");

   PRM_PARAMD( x2, 0, "x2", "x2 coordinate", "def=0.2\nsep=block");
   PRM_PARAMD( y2, 0, "y2", "y2 coordinate", "def=0.62\nsep=col");
   PRM_PARAMD( z2, 0, "z2", "z2 coordinate", "def=0.0\nsep=col");

   PRM_PARAMD( x3, 0, "x3", "x3 coordinate", "def=0.25\nsep=block");
   PRM_PARAMD( y3, 0, "y3", "y3 coordinate", "def=0.65\nsep=col");
   PRM_PARAMD( z3, 0, "z3", "z3 coordinate", "def=0.0\nsep=col");

   PRM_PARAMD( v_0, 0, "v_0", "Value 0", "def=0.5\nsep=block");
   PRM_PARAMD( v_1, 0, "v_1", "Value 0", "def=0.25\nsep=col");
   PRM_PARAMD( v_2, 0, "v_2", "Value 0", "def=0.1\nsep=col");

   PRM_INT( nl, 0, "nl",  "Number of something", "sep=block\ndef=4" );
   PRM_SWITCH( drawFlippery, 0, "Draw", "Draw flippery", "def=1\nsep=col" );

   Q_CLASSINFO( "nameHintBase",  "flp_" );
   DCL_DEFAULT_STATIC;
};


// --------------------------------------------------------------------

/**Description of graph
  *@author atu
  */

class TGraph : public LinkedObj  {
  Q_OBJECT
 public:
   DCL_CTOR(TGraph);
   DCL_CREATE;
   DCL_STD_INF;
   virtual ~TGraph() override;
   virtual QVariant dataObj( int col, int role = Qt::DisplayRole ) const override;
   Q_INVOKABLE virtual QString textVisual() const override;
   using shared_GE = std::shared_ptr<GraphElem>;


   /** fills fields in DatasInfo structure, return number of elements (nn) */
   int fillDatasInfo( DatasInfo *di ) const;
   /** dumps data to file */
   Q_INVOKABLE int  dump( const QString &fn, const QString &delim = " " );
   //* add new GraphElem to given TOutArr
   Q_INVOKABLE int addOutArr( const QString &o_name );
   //* prepare inner data for plotting
   Q_INVOKABLE  int prepare();
   //* plot to file
   Q_INVOKABLE void plotToPng( const QString &fn );
   Q_INVOKABLE QString hintFileName() const;


   QSize renderTo( QImage &img, const ViewData *a_vd = nullptr, const ScaleData *scda = nullptr );
   bool fillViewData( ViewData *da );
   bool getPointAt( int ig, long ip, mglPoint *p ) const;
   long findNearest( const mglPoint &p, int ig ) const ;//* find nearest point index
   void viewForPoints( const mglPoint &p0, const mglPoint &p1, ViewData &vd ) const;
   QString getPlotLabel( int ig ) const;
   QString getPrintInfo( int ig ) const;
   mglPoint CalcXYZ( int mx, int my, int w, int h,
       const ViewData *a_vd = nullptr, const ScaleData *scda = nullptr );
 protected:
   virtual void do_reset() override;
   //* remove numbner of data points to plot
   long fillSqueeze( std::vector<uint8_t> &plp );
   void plotTo( mglGraph &gr, const ViewData *a_vd = nullptr, const ScaleData *scda = nullptr );
   void plot1( mglGraph &gr, const GraphElem *pl );
   void setupMglGraph( mglGraph &grs, const ViewData *a_vd, const ScaleData *scda, bool full = true );
   void addMetaData( QImage &img ) const;

   /** title of graph  */
   PRM_STRING( title, efNRC, "Title", "Plot title", "max=128\nncol=-1\ndef=fig. " );
   PRM_STRING( descr, efNRC, "description", "Plot description", "max=256\nncol=-1");
   // inners
   PRM_DOUBLE( v_min, efInner, "v_min", "Minimal plot value", "def=0.0" );
   PRM_DOUBLE( v_max, efInner, "v_max", "Maximal plot value", "def=1.0" );
   PRM_INT( c_nn, efInner, "nn", "Common number of data points", "def=0" );
   PRM_INT( c_nx, efInner, "nx", "Common number of data points in X direction", "def=0" );
   PRM_INT( c_ny, efInner, "ny", "Common number of data points in Y direction", "def=1" );

   ScaleData *scd; // not smart ptr, as under control of Qt

   bool prepared = false;
   bool was_2D = false;
   bool need_c_axis = false;
   long nn = 0, nx = 0, ny = 1;
   GraphElem* tli[LineRole::sz];  // type line ptrs
   std::vector<GraphElem*> pli;   // data line ptrs - only plottted lines

   // special elements, not in common tree, just to present missing axis/data
   // GraphElem *ge_zero = nullptr, *ge_fx = nullptr, *ge_fy = nullptr;
   shared_GE ge_zero, ge_fx, ge_fy;
   //* defining scale points
   mglPoint pr_min { 0, 0, 0 }, pr_max { 1, 1, 1 }, pr_dlt { 1, 1, 1 }; // real
   mglPoint pe_min { 0, 0, 0 }, pe_max { 1, 1, 1 }, pe_dlt { 1, 1, 1 }; // eff: last plot
   mglPoint ve_min { 0, 0, 0 }, ve_max { 1, 1, 1 }, ve_dlt { 1, 1, 1 }; // visual?

   const mglData *d_x = nullptr;
   const mglData *d_y = nullptr;
   // const mglData *d_z = nullptr;
   const mglData *d_c0 = nullptr;
   // mglGraph gr { 0, 800, 600 }; // 0 - def, 1 - OpenGL - dont work

   Q_CLASSINFO( "nameHintBase",  "plot_" );
   DCL_DEFAULT_STATIC;
};
typedef TGraph *PTGraph;



