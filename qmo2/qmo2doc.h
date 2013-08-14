/***************************************************************************
                          qmo2doc.h  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2013 by atu
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

#ifndef QMO2DOC_H
#define QMO2DOC_H

#include <config.h>

// include files for QT
#include <QObject>
#include <QString>
#include <QList>

#include "dataset.h"
#include "rootdata.h"
#include "tmodel.h"

// forward declaration of the QMo2 classes
class QMo2View;

/**	QMo2Doc provides a document object for a document-view model.
  *
  * The QMo2Doc class provides a document object that can be used 
  * in conjunction with the classes
  * QMo2Win and QMo2View to create a document-view model for 
  * MDI (Multiple Document Interface)
  */
class QMo2Doc : public QObject
{
  Q_OBJECT
  friend class QMo2View;
 public:
  /** Constructor for the fileclass of the application */
  QMo2Doc();
  /** Destructor for the fileclass of the application */
  ~QMo2Doc();

  /** returns true, if the requested view is the last view of the document */
  bool isLastView() const;
  /** This method gets called when the user is about to close a frame window.
      * It checks, if more than one view is connected to the document 
      * (then the frame can be closed), if pFrame is the last view and the 
      * document is modified, the user gets asked if he wants to save the 
      * document. Not const: can save document if need.*/
  bool canCloseFrame( QMo2View* pFrame );
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

  /** sets the filename of the document */
  void setTitle( const QString &title );
  /** returns the title of the document */
  const QString& title() const;
  /** returns pointer to model */
  TModel *getModel(void) const;
  /** returns ptr to rootdata */
  TRootData *getRoot(void) const;
  /** fills rootdata reg infos */
  void fillRoot(void);
  /** returns nonamed status */
  bool nonamed(void) const { return is_nonamed; }
      
 public slots:
 	
 private:
   /** the modified flag of the current document */
   bool modified;
   QString m_title;
   QString m_filename;
  // atu:
 protected:
   TRootData *rootdata;
   TModel *model;      
   bool is_nonamed;
   bool loaded_as_old;
};

#endif // QMO2DOC_H

// end of qmo2doc.h


