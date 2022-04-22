#pragma once
/***************************************************************************
     pastedialog.h - struct and dialog for paste and clone objects
                             -------------------
    begin                : 2016.1220
    copyright            : GPL (C) 2016-2022 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/


#include <QtWidgets>

struct PasteObjParams {
  QString old_name, new_name;
  bool iterateSrc = true;
  unsigned n = 1;
  int dn = 1, dx = 0, dy = 1;
};


class PasteDialog : public QDialog {
 Q_OBJECT
  public:
   PasteDialog( PasteObjParams &_pp, QWidget *_parent );
   static bool getPasteData( PasteObjParams &_pp, QWidget *aparent );
  public Q_SLOTS:
   virtual void accept() override;
  protected:
   void setupUi();
   PasteObjParams &pp;
   QLineEdit *ed_new_name = nullptr, *ed_n = nullptr, *ed_dn = nullptr,
             *ed_dx = nullptr, *ed_dy = nullptr;
   QCheckBox *cb_iter = nullptr;
};


