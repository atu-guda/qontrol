/***************************************************************************
                          defs.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 *************************************************************************/

#ifndef _DEFS_H 
#define _DEFS_H

#include <limits>
#include <vector>
#include <config.h>

/* General constants */
#define MAX_NAMELEN   32
#define MAX_LABELLEN 128

/* Model constants */

#define MODEL_MX 64
#define MODEL_MY 64
#define MODEL_NVAR 256

/** max numbers of signal and parametric inputs */
#define OBJ_NINP 4
#define OBJ_NPRM 4

const int IMIN      { std::numeric_limits<int>::min() };
const int IMAX      { std::numeric_limits<int>::max() };
const unsigned UMIN { std::numeric_limits<unsigned int>::min() };
const unsigned UMAX { std::numeric_limits<unsigned int>::max() };
const double DMIN   { std::numeric_limits<double>::lowest() };
const double DMAX   { std::numeric_limits<double>::max() };

#define L8B QString::fromLocal8Bit

using dvector = std::vector<double>;
 
enum _DATA_TYPES {
   dtpEnd = 0, dtpUndef = 0, 
   dtpInt = 1, dtpLong = 1,
   dtpDouble = 2, dtpDbl = 2, dtpDou = 2,
   dtpString = 3, dtpStr = 3,
   dtpStringArr = 4,
   dtpIntArr = 5,
   dtpDoubleArr = 6,
   dtpObj = 10, //
   dtpFun = 11, // function with uniform args (TData?) TODO
   dtpFunPure = 12, // ordinary function, don't use in UI
   // dialog elements - not data
   dtpDial = 1000,   // dlg_x,y,w,h -- dialog geometry, listdata - header
   dtpButton = 1001, // subtp -- action type: dtpsOK, dtpsCancel, dtpsHelp...
   dtpLabel = 1002,  // listdata -- text of label (may be with some more?)
   dtpGroup = 1003   // listdata -- group caption
};

enum _DATA_STYPES {
   /* based on int */
   dtpsInt=0, dtpsSwitch=1, dtpsSw=1, dtpsList=2, dtpsRadio=3, 
   dtpsDate=4, dtpsTime=5, dtpsColor=7,
   /* based on string */
   dtpsStr=0, dtpsMline=1, dtpsBin=2, dtpsOName=3, dtpsEName=4, dtpsFName=5,
   /* based on dtpButton */
   dtpsOK=0, dtpsCancel=1, dtpsHelp=2, dtpsTest=3, dtpsBtn=4
};

enum _ELEM_FLAGS {
  efMustRead = 1, efNoRunChange = 2, efNRC = efNoRunChange,  /* MR, NRC */ 
  efNoDial = 4, efRODial = 8,                /* ND, ROD */
  efNoSave = 16, efRO = 32, efStatic = 64,   /* NS, RO, - */
  efInner = efNoDial | efNoSave | efRO,
  efOnlyFirst = 0x8000 /* not part, of flags, set by TMiso::fillLinks if req*/

};

// todo: func QString elemFlagsToString( int );

enum _STATE_VALUES {
  stateBad = 0, stateGood = 1, stateDone = 2, stateRun = 3,
  statePreRun = 4, statePostRun = 5 // not used
};

enum _LOOP_TYPES {
  loopInnerFirst = 1, loopParmFirst = 2, loopVeryFirst = 4,
  loopInnerLast = 16, loopParmLast = 32, loopVeryLast = 64
};

const char* getStateString( int stat_num );

constexpr char RE_NAME[] { "^[_a-zA-Z]+[_0-9A-Za-z]*$" };
// not good, allowing "a..b", buf fast
constexpr char RE_FULLNAME[]  { R"(^([_a-zA-Z]+[_0-9A-Za-z]*)\.([_a-zA-Z]+[_0-9A-Za-z.]*)$)" } ;

#endif // _DEFS_H

