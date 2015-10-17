/***************************************************************************
                          labodoc.h  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2015 by atu
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

  /** This method gets called when the user is about to close a frame window.
      * It checks, if more than one view is connected to the document
      * (then the frame can be closed), if pFrame is the last view and the
      * document is modified, the user gets asked if he wants to save the
      * document. Not const: can save document if need.*/
  bool canCloseFrame( LaboView* pFrame );
  /** returns if the document is modified or not. Use this to determine
      if your document needs saving by the user on closing.*/
  bool isModified() const;
  /** initializes the document generally */
  bool newDocument();
  /** loads the model document by filename  */
  bool openDocument( const QString &filename );
  /** saves the model document */
  bool saveDocument( bool forceNewName = false );
  /** sets the path to the file connected with the document */
  void setPathName( const QString &name );
  /** returns the pathname of the current document file*/
  const QString& pathName() const;
  /** create representation */
  QString toString() const;


  /** returns ptr to rootdata */
  TRootData *getRoot() const { return rootdata; }
  /** returns nonamed status */
  bool nonamed() const { return is_nonamed; }

 public slots:

 private:
   QString m_filename;
 protected:
   TRootData *rootdata = nullptr;
   bool is_nonamed = true;
};

#endif // LABODOC_H

// end of labodoc.h


