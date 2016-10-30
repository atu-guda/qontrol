#pragma once
/***************************************************************************
                          defs.h  -  common definitions and descriptions
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2016 by atu
    email                : atu@nmetau.edu.ua
 *************************************************************************/


#include <limits>
#include <vector>

#include <QString>
#include <QStringList>

#define VERSION "0.0.4"

/* Name of package */
#define PACKAGE "qontrol"
#define ORG "atu.atunet"
#define COPYRIGHT "(c) 2000-2016 atu; under GPLv2+"

/* Model constants */

#define DOUBLE_PREC 16
#define MODEL_MX 64
#define MODEL_MY 64

#define qP(x) qPrintable(x)
#define WHE " in " << __FILE__ << ": " <<  __PRETTY_FUNCTION__ << ": " << __LINE__
#define NWHE " in " << getFullName() << " in " << __FILE__ << ": " <<  __PRETTY_FUNCTION__ << ": " << __LINE__

#define TCHILD(T) findChildren<T>( QString(), Qt::FindDirectChildrenOnly )

constexpr const int IMIN      { std::numeric_limits<int>::min() };
constexpr const int IMAX      { std::numeric_limits<int>::max() };
constexpr const long LMIN      { std::numeric_limits<long>::min() };
constexpr const long LMAX      { std::numeric_limits<long>::max() };
constexpr const unsigned UMIN { std::numeric_limits<unsigned int>::min() };
constexpr const unsigned UMAX { std::numeric_limits<unsigned int>::max() };
constexpr const unsigned long ULMIN { std::numeric_limits<unsigned long>::min() };
constexpr const unsigned long ULMAX { std::numeric_limits<unsigned long>::max() };
constexpr const double DMIN   { std::numeric_limits<double>::lowest() };
constexpr const double DMAX   { std::numeric_limits<double>::max() };
constexpr const double D_EPS  { 1.0e-6 }; // default error
constexpr const double D_AZERO  { 1.0e-100 }; // a near-zero value

constexpr const char *const DATE_FORMAT { "yyyy.MM.dd" };
constexpr const char *const TIME_FORMAT { "hh:mm:ss.zzz" };

constexpr const char *const model_file_suff { ".qol" };
constexpr const char *const model_files_sel { "Model *.qol files (*.qol);;All files(*)" };
constexpr const char *const data_file_suff  { ".dat" };
constexpr const char *const data_files_sel  { "Data files (*.txt *.dat *.csv);;All files (*)" };

#define L8B QString::fromLocal8Bit
#define QSN QString::number
#define QSNX(n) QString::number((n),DOUBLE_PREC)
#define QSND(n) QString::number((n),'g',DOUBLE_PREC)
#define QSNL(n,l) QString::number((n),'g',l)
#define QSL QStringLiteral

#define SCRIPT_DIR "scripts"
#define LIB_DIR    "lib"

using dvector = std::vector<double>;


enum _ELEM_FLAGS {
  efMustRead    =     1,   //* MR - really unused [0]
  efNRC         =     2,   //* NRC - disable change during run [1]
  efNoDial      =     4,   //* ND - not show in dialog [2]
  efRODial      =     8,   //* ROD - R/O in dialogs [3]
  efNoSave      =  0x10,   //* NS - no save this object in file [4]
  efRO          =  0x20,   //* RO - readonly element [5]
  efStatic      =  0x40,   //* St - static element - unknown for now [6]
  efImmutable   =  0x80,   //* Im - disable rename, delete [7]
  efPostParent  = 0x100,   //* PP - call parent post_set from self [8]
  efStructCh    = 0x200,   //* SS - changes hits common structure [9]
  efROAny = efRO | efRODial,
  efInner = efNoDial | efNoSave | efRO,
  efTmp = efNoSave | efRO,
  efOld = efNRC | efNoDial | efNoSave
};
const constexpr int elm_known_flags = 10;
// const char *elem_flag_names[elm_known_flags+1] in miscfunc.cpp for now
// QString flags2str( int fl ) there too

enum _STATE_VALUES {
  stateBad = 0, stateGood = 1, stateDone = 2, stateRun = 3
};


const char* getStateString( int stat_num );

constexpr char RE_NAME[] { R"!(^[_a-zA-Z][_0-9A-Za-z]*$)!" };
constexpr char RE_NAME_IDX[] { R"!(^([_a-zA-Z][_0-9A-Za-z]*)(?:\[(\d+)\])?$)!" };
// not good, allowing "a..b", buf fast
constexpr char RE_FULLNAME[]
  { R"(^([_a-zA-Z]+[_0-9A-Za-z]*)\.([_a-zA-Z]+[_0-9A-Za-z.]*(?:\[(\d+)\])?)$)" } ;

extern QString log_app;


