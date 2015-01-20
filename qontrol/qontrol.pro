QT += core widgets xml script printsupport
TEMPLATE = app
TARGET = qontrol
CONFIG += c++11 release warn_on qt thread
QMAKE_CXXFLAGS += -std=gnu++11 -fgnu-keywords
#CONFIG += debug warn_on qt thread

RESOURCES = qontrol.qrc

INCLUDEPATH += .. ../elems

LIBS += -lgsl -lgslcblas
LIBS += -lmgl
LIBS += -lfftw3

SOURCES += \
  addelemdia.cpp \
  circbuf.cpp \
  contgraph.cpp \
  contout.cpp \
  datacont.cpp \
  dataset.cpp \
  elemfact.cpp \
  inputparams.cpp \
  main.cpp \
  miscfun.cpp \
  mo2settdlg.cpp \
  colorbtn.cpp \
  doubletable.cpp \
  graphdataview.cpp \
  labodoc.cpp \
  laboview.cpp \
  labowin.cpp \
  linkcompleter.cpp \
  mglview.cpp \
  outdataview.cpp \
  runview.cpp \
  statusmodel.cpp \
  structview.cpp \
  rootdata.cpp \
  scaledata.cpp \
  scheme.cpp \
  scriptfuncs.cpp \
  simul.cpp \
  simulview.cpp \
  tmiso.cpp \
  tmodel.cpp \
  datawidget.cpp \
  tex2label.cpp \
  tgraph.cpp \
  toutarr.cpp \
  rand_gen.cpp \
  ../elems/tadjgen.cpp \
  ../elems/tcorralalysis.cpp \
  ../elems/tcounter.cpp \
  ../elems/tcriterion.cpp \
  ../elems/tdelay.cpp \
  ../elems/textrlatch.cpp \
  ../elems/tfriction.cpp \
  ../elems/tfuncmisc.cpp \
  ../elems/tfuncpoly.cpp \
  ../elems/tfunctrans.cpp \
  ../elems/thyst.cpp \
  ../elems/tintegrator.cpp \
  ../elems/tlatch.cpp \
  ../elems/tlinear.cpp \
  ../elems/tlinp1q0.cpp \
  ../elems/tlogic.cpp \
  ../elems/tlorenz.cpp \
  ../elems/trossler.cpp \
  ../elems/tmultipl.cpp \
  ../elems/tpid.cpp \
  ../elems/trand.cpp \
  ../elems/trandtrigg.cpp \
  ../elems/trelaxgen.cpp \
  ../elems/tsource.cpp \
  ../elems/tsourcelin.cpp \
  ../elems/tswitch.cpp \
  ../elems/tthrum.cpp \
  ../elems/ttrigger.cpp \
  ../elems/tvibro.cpp 

HEADERS += \
  addelemdia.h \
  circbuf.h \
  contgraph.h \
  contout.h \
  datacont.h \
  dataset.h \
  defs.h \
  inputparams.h \
  miscfun.h \
  mo2settdlg.h \
  colorbtn.h \
  doubletable.h \
  graphdataview.h \
  labodoc.h \
  laboview.h \
  labowin.h \
  linkcompleter.h \
  mglview.h \
  outdataview.h \
  runview.h \
  statusmodel.h \
  structview.h \
  rootdata.h \
  scaledata.h \
  scheme.h \
  scriptfuncs.h \
  simul.h \
  simulview.h \
  tmiso.h \
  tmodel.h \
  datawidget.h \
  tgraph.h \
  toutarr.h \
  rand_gen.h \
  ../elems/tadjgen.h \
  ../elems/tcorralalysis.h \
  ../elems/tcounter.h \
  ../elems/tcriterion.h \
  ../elems/tdelay.h \
  ../elems/textrlatch.h \
  ../elems/tfriction.h \
  ../elems/tfuncmisc.h \
  ../elems/tfuncpoly.h \
  ../elems/tfunctrans.h \
  ../elems/thyst.h \
  ../elems/tintegrator.h \
  ../elems/tlatch.h \
  ../elems/tlinear.h \
  ../elems/tlinp1q0.h \
  ../elems/tlogic.h \
  ../elems/tlorenz.h \
  ../elems/trossler.h \
  ../elems/tmultipl.h \
  ../elems/tpid.h \
  ../elems/trand.h \
  ../elems/trandtrigg.h \
  ../elems/trelaxgen.h \
  ../elems/tsource.h \
  ../elems/tsourcelin.h \
  ../elems/tswitch.h \
  ../elems/tthrum.h \
  ../elems/ttrigger.h \
  ../elems/tvibro.h


