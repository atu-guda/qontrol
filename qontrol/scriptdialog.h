#pragma once
/***************************************************************************
     scriptdialog.h - dialog to edit and run scripts (header)
                             -------------------
    begin                : 2016.10.25
    copyright            : GPL (C) 2016-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/


#include <QtWidgets>

#include "tmodel.h"


class ScriptDialog : public QDialog {
 Q_OBJECT
  public:
   ScriptDialog( QString &_scr, TModel *_model, QWidget *_parent ); // _scr in not const: to allow save
  public Q_SLOTS:
   virtual void accept() override;
   virtual void run();
   virtual void clearScript();
   virtual void clearOutput();
   virtual void save();
   virtual void reload();
   virtual void revert();
  protected:
   void setupUi();
  //
   QString &scr0;   //* ref to original script
   QString scr;     //* current script
   QString scr00;   //* saved script
   QString out;     //* result: textual form
   QString errstr;  //* error text
   TModel *model;
 //
   QTextEdit *sced  = nullptr;  //* script editor, TODO: Qscintilla
   QTextEdit *outed = nullptr;  //* show output
   QTextEdit *erred = nullptr;  //* show errors
};


