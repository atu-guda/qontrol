/***************************************************************************
                          resource.h  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RESOURCE_H
#define RESOURCE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


///////////////////////////////////////////////////////////////////
// resource.h  -- contains macros used for commands


///////////////////////////////////////////////////////////////////
// COMMAND VALUES FOR MENUBAR AND TOOLBAR ENTRIES


///////////////////////////////////////////////////////////////////
// File-menu entries
#define ID_FILE_NEW                 10020
#define ID_FILE_OPEN                10030

#define ID_FILE_SAVE                10050
#define ID_FILE_SAVE_AS             10060
#define ID_FILE_CLOSE               10070

#define ID_FILE_PRINT               10080

#define ID_FILE_QUIT                10100

///////////////////////////////////////////////////////////////////
// Edit-menu entries
#define ID_EDIT_UNDO                11010
#define ID_EDIT_REDO                11020
#define ID_EDIT_COPY                11030
#define ID_EDIT_CUT                 11040
#define ID_EDIT_PASTE               11050
#define ID_EDIT_SELECT_ALL          11060
////////////////////////////////////////////////
// atu edit commands
#define ID_EDIT_NEWELM             11100
#define ID_EDIT_DELELM             11110
#define ID_EDIT_EDITELM            11120
#define ID_EDIT_LINKELM            11130
#define ID_EDIT_LOCKELM            11140
#define ID_EDIT_EDITMODEL          11150
#define ID_EDIT_ORDELM             11160
#define ID_EDIT_MOVEELM            11170

#define ID_EDIT_NEWOUT              11200
#define ID_EDIT_DELOUT              11210
#define ID_EDIT_EDITOUT             11220
#define ID_EDIT_DUMPOUT             11230

#define ID_EDIT_NEWGRAPH            11300
#define ID_EDIT_DELGRAPH            11310
#define ID_EDIT_EDITGRAPH           11320
#define ID_EDIT_SHOWGRAPH           11330
#define ID_EDIT_DUMPGRAPH           11340
#define ID_VIEW_HELP                11350
#define ID_VIEW_PRINTSTRUCT         11400
#define ID_VIEW_PRINTPLOT           11410
#define ID_VIEW_GNUPLOT             11420

///////////////////////////////////////////////////////////////////
// View-menu entries                    
#define ID_VIEW_TOOLBAR             12010
#define ID_VIEW_STATUSBAR           12020
#define ID_VIEW_SHOWDATA            12110
#define ID_VIEW_SHOWVARS            12120

///////////////////////////////////////////////////////////////////
// Window-menu entries
#define ID_WINDOW_NEW_WINDOW        13010
#define ID_WINDOW_CASCADE	    13020
#define ID_WINDOW_TILE	            13030

///////////////////////////////////////////////////////////////////
// Help-menu entries
#define ID_HELP_ABOUT               1002

///////////////////////////////////////////////////////////////////
// General application values
#define ID_RUNRUN		    15000
#define ID_RUNPRM		    15010
#define ID_RUNPRM2		    15020

///////////////////////////////////////////////////////////////////
// atu misc values
#define ID_TEST                     30000 


#endif // RESOURCE_H
