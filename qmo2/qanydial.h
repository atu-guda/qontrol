/***************************************************************************
                          qanydial.h  -  description
                             -------------------
    begin                : Mon Jun 26 2000
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

#ifndef QANYDIALOG_H
#define QANYDIALOG_H

#include <QDialog>
// #include <QComboBox>
class QComboBox;
#include "dataset.h"

/** Inserts textual items to QComboBox from string s
  elements separated by \n; mn -- max number of items
*/
int fillComboBox( QComboBox *qb, const char *s, int mn );

/** This class can create dialog window with elements defined
in array of TDataInfo. First element must describe size of dialog.
There are 2 types of elements:
  a) data-linked -- connected whith real datas (int(switch,combo), dbl, str)
  b) decoration - labels, groupboxes, buttons ...

  *@author atu
*/

class QAnyDialog : public QDialog  {
 Q_OBJECT 
 public: 
  /** default constructor -- 
      TDataSet *ads -- pointer to data containing element */
  explicit QAnyDialog( TDataSet *ads , QWidget *parent=0, const char *name=0 );
  ~QAnyDialog();
  /** Creates all elements described in *ds */
  void initDialog(void);
  /** sets data to dialog elements, executes dialog, 
      if ok -- transfers data back */
  int exec_trans(void);
  void showSimpleHelp(void);
 public slots:
  void showHelp(void);
 protected: 
  TDataSet *ds;
  QWidget **elms;
};

#endif

// end of qanydial.h

