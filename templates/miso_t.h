
#define CLASS_ID_ZZZZ 100X

class ZZZZ : public TMiso  {
 public:
   /** constructor */
   ZZZZ( PTDataSet aparent );
   /** empty destructor */
   virtual ~ZZZZ();
   /** creator */
   static PTDataSet create( PTDataSet apar );
   /** class id */
   virtual int getClassId(void) const ;
   /** class name - for check & human purpose */
   virtual const char* getClassName(void) const;
   /** return ptr to static class_info, must be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** return icon */
   virtual const char** getIcon(void) const;      
   /** reimplemented from TMiso to  */
   virtual int preRun( int run_tp, int an, int anx, int any, double adt );
   /** reimplemented from TMiso to  */
   virtual int postRun( int good );
   /** reimplemented from TMiso to  */
   virtual int startLoop( int acnx, int acny );
   /** reimplemented from TMiso to  */
   virtual int startLoop2( int acny );
   /** reimplemented from TMiso to  */
   virtual int endLoop(void);
   /** reimplemented from TMiso to  */
   virtual int endLoop2(void);
   /** main computation function */
   virtual double f( const double *u, double t );
 protected:
   /** type of */
   int type;
   /** coefficients  */
   double a[4], b;
   /** data descriptors -- with base class elements */ 
   static TDataInfo tzzzz_d_i[20];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
};


