#pragma once

#include "xdata.h"
#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include <queryosity.hpp>
#include <memory>

class xanalysis : public AthAnalysisAlgorithm {

public:
  xanalysis(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~xanalysis();

  virtual void analyze(qty::dataflow& df, qty::dataflow::input<xdata>& ds);
  // virtual void output();

  virtual StatusCode  initialize();     //once, before any input is loaded
  virtual StatusCode  beginInputFile(); //start of each input file, only metadata loaded
  // virtual StatusCode  firstExecute();   //once, after first eventdata is loaded (not per file)
  virtual StatusCode  execute();        //per event
  //virtual StatusCode  endInputFile();   //end of each input file
  //virtual StatusCode  metaDataStop();   //when outputMetaStore is populated by MetaDataTools
  virtual StatusCode  finalize();       //once, after all events processed
  
protected:
  qty::dataflow m_df;
  qty::dataflow::input<xdata> m_ds;

};