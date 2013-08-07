/***************************************************************************
                          addelem.h  -  description
                             -------------------
    begin                : Jul 30 2012
    copyright            : GPL (C) 2012-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _ADDELEMDIA_H
#define _ADDELEMDIA_H

#include <QtGui>
#include "dataset.h"

struct addElemInfo {
  QString type; // class type
  QString name; // element name
  int order;
};

class AddElemDialog : public QDialog {
 Q_OBJECT
  public:
   AddElemDialog( addElemInfo *a_aei, int a_props, TDataSet *a_pobj, QWidget *aparent );
  public slots:
   virtual void accept();
  protected:
   void setupUi();
   addElemInfo *aei;
   int props;
   TDataSet *pobj; // parent object - to ask allowed types. my be 0
   QLineEdit *ed_name;
   QLineEdit *ed_order;
   QListWidget *lw;
};

#endif 

