#pragma once
/***************************************************************************
     scriptdialog.h - dialog to edit and run scripts (header)
                             -------------------
    begin                : 2016.10.25
    copyright            : GPL (C) 2016-2022 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/


#include <QtWidgets>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerjavascript.h>

#include "tmodel.h"


class ScriptDialog : public QDialog {
 Q_OBJECT
  public:
   ScriptDialog( QString &_scr, TModel *_model, QWidget *_parent ); // _scr in not const: to allow save
  public Q_SLOTS:
   virtual void accept() override;
   virtual void run();
   virtual void runModelScript();
   virtual void initEngine();
   virtual void clearScript();
   virtual void clearOutput();
   virtual void save();
   virtual void reload();
   virtual void revert();
  protected:
   void setupUi();
   void runScr( const QString &s );
  //
   QString &scr0;   //* ref to original script
   QString scr;     //* current script
   QString scr00;   //* saved script
   QString out;     //* result: textual form
   QString errstr;  //* error text
   TModel *model;
 //
   QsciScintilla *sced  = nullptr;  //* script editor
   QTextEdit *outed = nullptr;  //* show output
   QTextEdit *erred = nullptr;  //* show errors
};


