/***************************************************************************
                          labodoc.h  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2014 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LABODOC_H
#define LABODOC_H

// include files for QT
#include <QObject>
#include <QString>
#include <QList>
#include <QScriptEngine>

#include "dataset.h"
#include "rootdata.h"
#include "tmodel.h"

// forward declaration of the Labo classes
class LaboView;

/**        LaboDoc provides a document object for a document-view model.
  *
  * The LaboDoc class provides a document object that can be used
  * in conjunction with the classes
  * LaboWin and LaboView to create a document-view model for
  * MDI (Multiple Document Interface)
  */
class LaboDoc : public QObject
{
  Q_OBJECT
  friend class LaboView;
 public:
  /** Constructor for the fileclass of the application */
  LaboDoc();
  /** Destructor for the fileclass of the application */
  ~LaboDoc();

  /** returns true, if the requested view is the last view of the document */
  bool isLastView() const;
  /** This method gets called when the user is about to close a frame window.
      * It checks, if more than one view is connected to the document
      * (then the frame can be closed), if pFrame is the last view and the
      * document is modified, the user gets asked if he wants to save the
      * document. Not const: can save document if need.*/
  bool canCloseFrame( LaboView* pFrame );
  /** sets the modified flag for the document after a modifying action
      on the view connected to the document. */
  void setModified( bool _m = true ){ modified=_m; }
  /** returns if the document is modified or not. Use this to determine
      if your document needs saving by the user on closing.*/
  bool isModified() const; // { return modified; };
  /** deletes the document's contents */
  void deleteContents();
  /** initializes the document generally */
  bool newDocument();
  /** loads the XML model document by filename  */
  bool openDocumentXML( const QString &filename );
  /** saves the XML model document under filename */
  bool saveDocumentXML( const QString &filename );
  /** sets the path to the file connected with the document */
  void setPathName( const QString &name );
  /** returns the pathname of the current document file*/
  const QString& pathName() const;
  /** create XML representation */
  QString makeXML() const;
  /** call engine and returns result, casted to QString */
  QString runScript( const QString& script );


  /** sets the filename of the document */
  void setTitle( const QString &title );
  /** returns the title of the document */
  const QString& title() const;
  /** returns pointer to model */
  TModel *getModel(void) const;
  /** returns ref to script engine: not const: engine can change model */
  QScriptEngine& getEngine() { return eng; }
  /** returns ptr to rootdata */
  TRootData *getRoot(void) const;
  /** fills rootdata reg infos */
  void fillRoot(void);
  /** returns nonamed status */
  bool nonamed(void) const { return is_nonamed; }

 public slots:

 private:
   void initEngine();
   bool migrateSumul();
   bool createEmptySyms();
   /** the modified flag of the current document */
   bool modified = false;
   QString m_title = "? unknown ";
   QString m_filename;
   QScriptEngine eng;
 protected:
   TRootData *rootdata = nullptr;
   TModel *model = nullptr;
   bool is_nonamed = true;
   bool loaded_as_old = true;
};

#endif // LABODOC_H

// end of labodoc.h


