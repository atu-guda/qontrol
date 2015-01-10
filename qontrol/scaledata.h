/***************************************************************************
                          scaledata.h  -  description
                             -------------------
    begin                : 19 Jul 29 2012
    copyright            : GPL (C) 2012-2015 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _SCALEDATA_H
#define _SCALEDATA_H

#include <mgl2/mgl.h>

#include "dataset.h"


/** Information about scale: really struct */
class ScaleData : public TDataSet
{
  Q_OBJECT
  public:
   DCL_CTOR(ScaleData);
   virtual ~ScaleData() override;
   DCL_CREATE;
   DCL_STD_INF;

   mglPoint getMark() const { return mglPoint( markX, markY, markZ ); }
   mglPoint getBase() const { return mglPoint( baseX, baseY, baseZ ); }
   void setMark( const mglPoint &p ) { markX = p.x; markY = p.y; markZ = p.z; }
   void setBase( const mglPoint &p ) { baseX = p.x; baseY = p.y; baseZ = p.z; }

   PRM_INT(    w0,    efNRC, "width", "Initial plot width", "def=800\nmin=100\nmax=10000" );
   PRM_INT(    h0,    efNRC, "height", "Initial plot height", "def=600\nmin=100\nmax=10000\nsep=col" );
   PRM_DOUBLE( plotFactor,  efNRC, "Plot factor", "Plot scale relative to canvas", "def=1.4\nmin=0.5\nmax=10\nsep=block" );
   PRM_DOUBLE( fontSise,  efNRC, "Font size", "Base font size", "def=2.0\nmin=0.1\nmax=10" );
   PRM_DOUBLE( phi,  efNRC, "Phi", "Phi angle", "def=0.0" );
   PRM_DOUBLE( theta,  efNRC, "Theta", "Theta angle", "def=0.0" );
   PRM_DOUBLE( maxErr,  efNRC, "Max Err", "Maximum plot error", "def=0.6" );

   PRM_SWITCH( drawAxis,  efNRC, "Axes", "Draw axes", "def=1\nsep=col" );
   PRM_SWITCH( drawBox,   efNRC, "Box",  "Draw box",  "def=1" );
   PRM_SWITCH( drawColorbar,   efNRC, "Colorbar",  "Draw colorbar",  "def=0" );
   PRM_STRING( mainLabel, efNRC, "Main label", "Label for main axis (Y/Z)", "max=128" );

   PRM_SWITCH( useLight,  efNRC, "Light", "Use lighting", "def=0\nsep=col" );
   PRM_SWITCH( useAlpha,  efNRC, "Use alpha", "Use transparency", "def=0" );
   PRM_DOUBLE( alpha,     efNRC, "Alpha", "transparency value", "def=0.7\nmin=0.0\nmax=1.0" );
   PRM_INT( legend_pos,   efNRC, "Legend pos", "Legend position index", "def=3\nmin=0\nmax=4" );

   PRM_COLOR( bgcolor, efNRC, "BG color", "Background color", "sep=col\ndef=#FFFFFF" );
   PRM_COLOR( axis_color, efNRC, "Axis color", "Axis color", "def=#000000" );
   PRM_COLOR( grid_color, efNRC, "Grid color", "Grid color", "def=#CCCCCC" );
   PRM_STRING( extra, efNRC, "Extra", "Extra options to plot", "max=128" );
   PRM_STRING( colorScheme, efNRC, "Color scheme", "Color Scheme for 2D plot", "max=128\ndef=BbcyrR" );

   // X-scale
   PRM_SWITCH(  autoScX, efNRC,    "Autoscale X", "Automaticaly make X scale", "def=1\nsep=block" );
   PRM_DOUBLE( plotMinX, efNRC,    "Plot min(x)", "Used for plot minimum X value", "" );
   PRM_DOUBLE( plotMaxX, efNRC,    "Plot max(x)", "Used for plot maximum X value", "def=1" );
   PRM_DOUBLE(    gridX, efNRC,    "Grid X", "Number of Grid lines for X (- =step)", "def=0" );
   PRM_INT(       tickX, efNRC,    "Tick X", "Number of Ticks for X", "def=5" );
   // Y-scale
   PRM_SWITCH(  autoScY, efNRC,    "Autoscale Y", "Automaticaly make Y scale", "sep=col\ndef=1" );
   PRM_DOUBLE( plotMinY, efNRC,    "Plot min(y)", "Used for plot minimum Y value", "" );
   PRM_DOUBLE( plotMaxY, efNRC,    "Plot max(y)", "Used for plot maximum Y value", "def=1" );
   PRM_DOUBLE(    gridY, efNRC,    "Grid Y", "Number of Grid lines for Y (- =step)", "def=0" );
   PRM_INT(       tickY, efNRC,    "Tick Y", "Number of Ticks for Y", "def=5" );
   // Z-scale
   PRM_SWITCH(  autoScZ, efNRC,    "Autoscale Z", "Automaticaly make Z scale", "sep=col\ndef=1" );
   PRM_DOUBLE( plotMinZ, efNRC,    "Plot min(Z)", "Used for plot minimum Z value", "" );
   PRM_DOUBLE( plotMaxZ, efNRC,    "Plot max(Z)", "Used for plot maximum Z value", "def=1" );
   PRM_DOUBLE(    gridZ, efNRC,    "Grid Z", "Number of Grid lines for Z (- =step)", "def=0" );
   PRM_INT(       tickZ, efNRC,    "Tick Z", "Number of Ticks for Z", "def=5" );

   // Mark and base
   PRM_DOUBLE(    markX, efNRC,    "Mark X", "X coord of mark", "def=0\nsep=block" );
   PRM_DOUBLE(    markY, efNRC,    "Mark Y", "Y coord of mark", "def=0\nsep=col" );
   PRM_DOUBLE(    markZ, efNRC,    "Mark Z", "Z coord of mark", "def=0\nsep=col" );
   PRM_SWITCH( drawMark, efNRC,    "Draw Mark", "Draw mark point", "def=1\nsep=col" );
   PRM_DOUBLE(    baseX, efNRC,    "Base X", "X coord of base", "def=0\nsep=block" );
   PRM_DOUBLE(    baseY, efNRC,    "Base Y", "Y coord of base", "def=0\nsep=col" );
   PRM_DOUBLE(    baseZ, efNRC,    "Base Z", "Z coord of base", "def=0\nsep=col" );
   PRM_SWITCH( drawBase, efNRC,    "Draw Base", "Draw base lines", "def=1\nsep=col" );
  protected:
   DCL_DEFAULT_STATIC;
};

#endif  // _SCALEDATA_H