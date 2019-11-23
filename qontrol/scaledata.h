#pragma once
/***************************************************************************
                          scaledata.h  -  description
                             -------------------
    begin                : 19 Jul 29 2012
    copyright            : GPL (C) 2012-2019 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/


#include <mgl2/mgl.h>

#include "dataset.h"


/** Information about scale: really struct */
class ScaleData : public TDataSet
{
  Q_OBJECT
  public:
   DCL_CTOR(ScaleData);
   // virtual ~ScaleData() override;
   DCL_CREATE;
   DCL_STD_INF;

   mglPoint getMark() const { return mglPoint( markX, markY, markZ ); }
   mglPoint getBase() const { return mglPoint( baseX, baseY, baseZ ); }
   void setMark( const mglPoint &p ) { markX = p.x; markY = p.y; markZ = p.z; }
   void setBase( const mglPoint &p ) { baseX = p.x; baseY = p.y; baseZ = p.z; }

   PRM_INT(    w0           , efNRC, QSL("width")        , QSL("Initial plot width")                   , QSL("def=800\nmin=100\nmax=10000") );
   PRM_INT(    h0           , efNRC, QSL("height")       , QSL("Initial plot height")                  , QSL("def=600\nmin=100\nmax=10000\nsep=col") );

   PRM_DOUBLE( phi          , efNRC, QSL("Phi")          , QSL("Phi angle")                            , QSL("def=0.0\nsep=block") );
   PRM_DOUBLE( theta        , efNRC, QSL("Theta")        , QSL("Theta angle")                          , QSL("def=0.0") );

   PRM_SWITCH( drawAxis     , efNRC, QSL("Axes")         , QSL("Draw axes")                            , QSL("def=1\nsep=col") );
   PRM_SWITCH( drawBox      , efNRC, QSL("Box")          , QSL("Draw box")                             , QSL("def=1") );

   PRM_STRING( mainLabel    , efNRC, QSL("Main label")   , QSL("Label for main axis (Y/Z)")            , QSL("max=128\nsep=col") );
   PRM_DOUBLE( mainScale    , efNRC, QSL("Main scale")   , QSL("Main label font scale")                , QSL("def=1.0\nmin=0.1\nmax=10") );

   PRM_INT(      legend_pos , efNRC, QSL("Legend pos")   , QSL("Legend position index 0-3,4,tXXYY")    , QSL("def=3\nmin=0\nsep=col") );
   PRM_DOUBLE( legend_scale , efNRC, QSL("Legend scale") , QSL("Legend font scale")                    , QSL("def=1.2\nmin=0.1\nmax=10") );

   // X-scale
   PRM_SWITCH(  autoScX     , efNRC, QSL("Autoscale X")  , QSL("Automaticaly make X scale")            , QSL("def=1\nsep=block") );
   PRM_SWITCH(  beauScX     , efNRC, QSL("Beautify X")   , QSL("Beautify X scale")                     , QSL("def=1") );
   PRM_DOUBLE( plotMinX     , efNRC, QSL("Plot min(x)")  , QSL("Used for plot minimum X value")        , QES );
   PRM_DOUBLE( plotMaxX     , efNRC, QSL("Plot max(x)")  , QSL("Used for plot maximum X value")        , QSL("def=1") );
   PRM_DOUBLE(    gridX     , efNRC, QSL("Grid X")       , QSL("Number of Grid lines for X (- =step)") , QSL("def=0") );
   PRM_INT(       tickX     , efNRC, QSL("Tick X")       , QSL("Number of Ticks for X")                , QSL("def=5") );
   // Y-scale
   PRM_SWITCH(  autoScY     , efNRC, QSL("Autoscale Y")  , QSL("Automaticaly make Y scale")            , QSL("sep=col\ndef=1") );
   PRM_SWITCH(  beauScY     , efNRC, QSL("Beautify Y")   , QSL("Beautify X scale")                     , QSL("def=1") );
   PRM_DOUBLE( plotMinY     , efNRC, QSL("Plot min(y)")  , QSL("Used for plot minimum Y value")        , QES );
   PRM_DOUBLE( plotMaxY     , efNRC, QSL("Plot max(y)")  , QSL("Used for plot maximum Y value")        , QSL("def=1") );
   PRM_DOUBLE(    gridY     , efNRC, QSL("Grid Y")       , QSL("Number of Grid lines for Y (- =step)") , QSL("def=0") );
   PRM_INT(       tickY     , efNRC, QSL("Tick Y")       , QSL("Number of Ticks for Y")                , QSL("def=5") );
   // Z-scale
   PRM_SWITCH(  autoScZ     , efNRC, QSL("Autoscale Z")  , QSL("Automaticaly make Z scale")            , QSL("sep=col\ndef=1") );
   PRM_SWITCH(  beauScZ     , efNRC, QSL("Beautify Z")   , QSL("Beautify Z scale")                     , QSL("def=1") );
   PRM_DOUBLE( plotMinZ     , efNRC, QSL("Plot min(Z)")  , QSL("Used for plot minimum Z value")        , QES );
   PRM_DOUBLE( plotMaxZ     , efNRC, QSL("Plot max(Z)")  , QSL("Used for plot maximum Z value")        , QSL("def=1") );
   PRM_DOUBLE(    gridZ     , efNRC, QSL("Grid Z")       , QSL("Number of Grid lines for Z (- =step)") , QSL("def=0") );
   PRM_INT(       tickZ     , efNRC, QSL("Tick Z")       , QSL("Number of Ticks for Z")                , QSL("def=5") );

   // Mark and base
   PRM_DOUBLE(    markX     , efNRC, QSL("Mark X")       , QSL("X coord of mark")                      , QSL("def=0\nsep=block") );
   PRM_DOUBLE(    markY     , efNRC, QSL("Mark Y")       , QSL("Y coord of mark")                      , QSL("def=0\nsep=col") );
   PRM_DOUBLE(    markZ     , efNRC, QSL("Mark Z")       , QSL("Z coord of mark")                      , QSL("def=0\nsep=col") );
   PRM_SWITCH( drawMark     , efNRC, QSL("Draw Mark")    , QSL("Draw mark point")                      , QSL("def=1\nsep=col") );
   PRM_DOUBLE(    baseX     , efNRC, QSL("Base X")       , QSL("X coord of base")                      , QSL("def=0\nsep=block") );
   PRM_DOUBLE(    baseY     , efNRC, QSL("Base Y")       , QSL("Y coord of base")                      , QSL("def=0\nsep=col") );
   PRM_DOUBLE(    baseZ     , efNRC, QSL("Base Z")       , QSL("Z coord of base")                      , QSL("def=0\nsep=col") );
   PRM_SWITCH( drawBase     , efNRC, QSL("Draw Base")    , QSL("Draw base lines")                      , QSL("sep=col") );

  // ------------------------ tab

   PRM_DOUBLE( plotFactor   , efNRC, QSL("Plot factor")  , QSL("Plot scale relative to canvas")        , QSL("def=1.4\nmin=0.5\nmax=10\nsep=tab\ntabname=More") );
   PRM_DOUBLE( fontSise     , efNRC, QSL("Font size")    , QSL("Base font size")                       , QSL("def=2.5\nmin=0.1\nmax=10") );
   PRM_INT(    tuneTick     , efNRC, QSL("Tune ticks")   , QSL("bit 0 = for large, bit 1 - small")     , QSL("def=3") );
   PRM_DOUBLE( multPos      , efNRC, QSL("Mult. pos")    , QSL("Position of the scale multipliers")    , QSL("def=1.05\nmin=0.1\nmax=2") );

   PRM_COLOR( bgcolor       , efNRC, QSL("BG color")     , QSL("Background color")                     , QSL("sep=col\ndef=#FFFFFFFF") );
   PRM_COLOR( axis_color    , efNRC, QSL("Axis color")   , QSL("Axis color")                           , QSL("def=#FF000000") );
   PRM_COLOR( grid_color    , efNRC, QSL("Grid color")   , QSL("Grid color")                           , QSL("def=#FFCCCCCC") );

   PRM_SWITCH( drawColorbar , efNRC, QSL("Colorbar")     , QSL("Draw colorbar")                        , QSL("def=0\nsep=col") );
   PRM_SWITCH(     useLight , efNRC, QSL("Light")        , QSL("Use lighting")                         , QSL("def=0") );
   PRM_SWITCH(     useAlpha , efNRC, QSL("Use alpha")    , QSL("Use transparency")                     , QSL("def=0") );
   PRM_DOUBLE(        alpha , efNRC, QSL("Alpha")        , QSL("transparency value")                   , QSL("def=0.7\nmin=0.0\nmax=1.0") );

   PRM_STRING( extra        , efNRC, QSL("Extra")        , QSL("Extra options to plot")                , QSL("max=128\nsep=col") );
   PRM_STRING( colorScheme  , efNRC, QSL("Color scheme") , QSL("Color Scheme for 2D plot")             , QSL("max=128\ndef=BbcyrR") );
   PRM_STRING( axisType     , efNRC, QSL("Axis type")    , QSL("Axis properties w/o xy/xyz")           , QSL("max=64\ndef=U3AKDTVISO") );
   PRM_DOUBLE( maxErr       , efNRC, QSL("Max Err")      , QSL("Maximum plot error")                   , QSL("def=0.6") );

   PRM_STRING( mglScript, 0, QSL("MGL script"), QSL("Script to be run before plot"),
       QSL("props=STRING,MLINE\nncol=-1\nsep=block"));
  protected:
   DCL_DEFAULT_STATIC;
};

