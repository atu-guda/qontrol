TEMPLATE = lib


CONFIG += release qt warn_on staticlib thread 
#CONFIG += debug qt warn_on staticlib thread 

LIBS += -lgsl -lgslcblas -lm 

INCLUDEPATH += ../qmo2

SOURCES += \
  tadjgen.cpp \
  tcorralalysis.cpp \
  tcounter.cpp \
  tcriterion.cpp \
  tdelay.cpp \
  textrlatch.cpp \
  tfourier.cpp \
  tfriction.cpp \
  tfuncmisc.cpp \
  tfuncpoly.cpp \
  tfunctrans.cpp \
  tgraph.cpp \
  thyst.cpp \
  tinputany.cpp \
  tintegrator.cpp \
  tlatch.cpp \
  tlinear.cpp \
  tlinp1q0.cpp \
  tlogic.cpp \
  tlorenz.cpp \
  tmultipl.cpp \
  toutarr.cpp \
  toutputany.cpp \
  tpid.cpp \
  trand.cpp \
  trandtrigg.cpp \
  tsource.cpp \
  tsourcelin.cpp \
  tswitch.cpp \
  ttrigger.cpp \
  tvibro.cpp 

HEADERS += \
  tadjgen.h \
  tcorralalysis.h \
  tcounter.h \
  tcriterion.h \
  tdelay.h \
  textrlatch.h \
  tfourier.h \
  tfriction.h \
  tfuncmisc.h \
  tfuncpoly.h \
  tfunctrans.h \
  tgraph.h \
  thyst.h \
  tinputany.h \
  tintegrator.h \
  tlatch.h \
  tlinear.h \
  tlinp1q0.h \
  tlogic.h \
  tlorenz.h \
  tmultipl.h \
  toutarr.h \
  toutputany.h \
  tpid.h \
  trand.h \
  trandtrigg.h \
  tsource.h \
  tsourcelin.h \
  tswitch.h \
  ttrigger.h \
  tvibro.h
