/***************************************************************************
 prog_opts.h - description of program options
                             -------------------
    begin                : 2015.12.01
    copyright            : GPL (C) 2015-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/
#ifndef _PROG_OPTS_H
#define _PROG_OPTS_H

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
  QString list_obj;      // -l parent[:obj]
  QString sim_name;      // -s sumulation name, def = sim0
  QStringList out_vars;  // -u T
  QStringList set_vars;  // -S var=value
  QStringList out_plots; // -g graph:file.png  or  -g graph
  QStringList out_outs;  // -p out[:file.txt]
  QStringList grdata_outs;  // -P graph[:file.txt]
  QStringList inc_dirs;  // -Idir : where to find scripts
  QStringList lib_dirs;  // -Ldir : where to find models, used as libs
  QStringList models;    // positional parameters
};

extern ProgOpts prog_opts; // defined in main.cpp

#endif

