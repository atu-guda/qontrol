/***************************************************************************
                          defs.h  -  common definitions and descriptions
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2015 by atu
    email                : atu@nmetau.edu.ua
 *************************************************************************/

#ifndef _DEFS_H
#define _DEFS_H

#include <limits>
#include <vector>

#include <QString>
#include <QStringList>

#define VERSION "0.0.2"

/* Name of package */
#define PACKAGE "qontrol"
#define ORG "atu.atunet"
#define COPYRIGHT "(c) 2000-2015 atu; under GPLv2+"

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
constexpr const unsigned UMIN { std::numeric_limits<unsigned int>::min() };
constexpr const unsigned UMAX { std::numeric_limits<unsigned int>::max() };
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
#define QSL QStringLiteral

#define SCRIPT_DIR "scripts"
#define LIB_DIR    "lib"

using dvector = std::vector<double>;


enum _ELEM_FLAGS {
  efMustRead = 1, // MR, really unused
  efNoRunChange = 2, efNRC = efNoRunChange,  //  NRC
  efNoDial = 4, efRODial = 8,                /* ND, ROD */
  efNoSave = 0x10, efRO = 0x20, efStatic = 0x40,   /* NS, RO, St */
  efImmutable = 0x80,  // Im
  efCalcParent = 0x100, // Ca
  efROAny = efRO | efRODial,
  efInner = efNoDial | efNoSave | efRO,
  efTmp = efNoSave | efRO,
  efOld = efNoRunChange | efNoDial | efNoSave
};
const constexpr int elm_known_flags = 9;

// todo: func QString elemFlagsToString( int );

enum _STATE_VALUES {
  stateBad = 0, stateGood = 1, stateDone = 2, stateRun = 3,
  statePreRun = 4, statePostRun = 5 // not used
};


const char* getStateString( int stat_num );

constexpr char RE_NAME[] { R"!(^[_a-zA-Z][_0-9A-Za-z]*$)!" };
constexpr char RE_NAME_IDX[] { R"!(^([_a-zA-Z][_0-9A-Za-z]*)(?:\[(\d+)\])?$)!" };
// not good, allowing "a..b", buf fast
constexpr char RE_FULLNAME[]
  { R"(^([_a-zA-Z]+[_0-9A-Za-z]*)\.([_a-zA-Z]+[_0-9A-Za-z.]*(?:\[(\d+)\])?)$)" } ;

// options passed to program
struct ProgOpts {
  bool batch = false;    // -b
  bool norun  = false;   // -N
  bool mod_scr = false;  // -M
  bool exit_st = false;  // -e scipt exit str to exit status
  int dbg = 0;           // -d - increase debug level -dN - set
  QString script;        // -x "JS code"
  QString fn_new;        // -c "new_file"
  QStringList s_files;   // -X file_with_script.js
  QString out_file;      // -o output_file.txt
  QString sim_name;      // -s sumulation name, def = sim0
  QStringList out_vars;  // -u T
  QStringList out_plots; // -g graph:file.png  or  -g graph
  QStringList out_outs;  // -p out[:file.txt]
  QStringList grdata_outs;  // -P graph[:file.txt]
  QStringList inc_dirs;  // -Idir : where to find scripts
  QStringList lib_dirs;  // -Ldir : where to find models, used as libs
  QStringList models;    // positional parameters
};

extern ProgOpts prog_opts; // defined in main.cpp
extern QString log_app;

#endif // _DEFS_H

