/***************************************************************************
                          defs.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2013 by atu
    email                : atu@nmetau.edu.ua
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

#define qP(x) qPrintable(x)
/** interface to qDebug with more info q: qPrintable to last argubent, x - format*/
#define DBG1(str)  qDebug("%s \n  in [ %s: %s %d ]", str, __FILE__, __PRETTY_FUNCTION__, __LINE__ )
#define DBG1q(str)  qDebug("%s \n  in [ %s: %s %d ]", qPrintable(str), __FILE__, __PRETTY_FUNCTION__, __LINE__ )
#define DBG2(str1,str2) qDebug("%s \"%s\"\n in [ %s: %s: %d ]", str1, str2, __FILE__, __PRETTY_FUNCTION__, __LINE__ )
// second string is QString
#define DBG2q(str1,str2) qDebug("%s \"%s\"\n in [ %s: %s: %d ]", str1, qPrintable(str2), __FILE__, __PRETTY_FUNCTION__, __LINE__ )
#define DBGx(...) { qDebug(__VA_ARGS__); qDebug(" in %s: %s: %d", __FILE__, __PRETTY_FUNCTION__, __LINE__ ); }

const int IMIN      { std::numeric_limits<int>::min() };
const int IMAX      { std::numeric_limits<int>::max() };
const unsigned UMIN { std::numeric_limits<unsigned int>::min() };
const unsigned UMAX { std::numeric_limits<unsigned int>::max() };
const double DMIN   { std::numeric_limits<double>::lowest() };
const double DMAX   { std::numeric_limits<double>::max() };

#define L8B QString::fromLocal8Bit
#define QSN QString::number

using dvector = std::vector<double>;
 

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

constexpr char RE_NAME[] { R"!(^[_a-zA-Z][_0-9A-Za-z]*$)!" };
constexpr char RE_NAME_IDX[] { R"!(^([_a-zA-Z][_0-9A-Za-z]*)(?:\[(\d+)\])?$)!" };
// not good, allowing "a..b", buf fast
constexpr char RE_FULLNAME[]  
  { R"(^([_a-zA-Z]+[_0-9A-Za-z]*)\.([_a-zA-Z]+[_0-9A-Za-z.]*(?:\[(\d+)\])?)$)" } ;

#endif // _DEFS_H

