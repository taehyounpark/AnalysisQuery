#pragma once

#include <xAODJet/Jet.h>
#include <xAODJet/JetContainer.h>

#include <AsgTools/AnaToolHandle.h>
#include <JetCPInterfaces/ICPJetUncertaintiesTool.h>
#include <JetCalibTools/IJetCalibrationTool.h>
#include <JetInterface/IJetSelector.h>

#include <EventFlow/ObjectCalibration.h>

#include <queryosity.hpp>

struct JetCalibrationConfig {

  bool isMC = true;

  std::string jetCollection = "AntiKt4EMPFlow";

  std::string calibConfig   = "AntiKt4EMPFlow_MC23a_PreRecR22_Phase2_CalibConfig_ResPU_EtaJES_GSC_240306_InSitu.config";
  std::string calibSequence = "JetArea_Residual_EtaJES_GSC";
  std::string calibArea     = "00-04-83";

  std::string uncertConfig = "";
  std::string uncertMCType = "";

  bool sortByPt;
};

class JetCalibration
    : public ObjectCalibration<xAOD::JetContainer, JetCalibrationConfig> {

public:
  JetCalibration(JetCalibrationConfig const &cfg);
  virtual ~JetCalibration() = default;

  virtual void initialize(unsigned int, unsigned long long,
                     unsigned long long) override;

  virtual ConstDataVector<xAOD::JetContainer>
  evaluate(qty::column::observable<xAOD::JetContainer> jets) const override;

protected:
  asg::AnaToolHandle<IJetCalibrationTool> m_JetCalibrationTool_handle {"JetCalibrationTool/JetCalibrationTool"}; //!
  asg::AnaToolHandle<ICPJetUncertaintiesTool> m_JetUncertaintiesTool_handle; //!
  asg::AnaToolHandle<ICPJetUncertaintiesTool> m_pseudodataJERTool_handle;    //!
};
