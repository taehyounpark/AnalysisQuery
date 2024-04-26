#include "AnaQuery/ElectronEfficiencyCorrection.h"

AnaQ::ElectronEfficiencyCorrection::ElectronEfficiencyCorrection(Json const &sfConfig) : m_sysSet({})
{
    m_toolName = sfConfig.value("toolName", "ElectronEfficiencyCorrectionTool");
    m_simulationFlavour = sfConfig.value("simulationFlavour", 1);
    m_idWorkingPoint = sfConfig.value("idWorkingPoint", "");
    m_isoWorkingPoint = sfConfig.value("isoWorkingPoint", "");
    m_recoWorkingPoint = sfConfig.value("recoWorkingPoint", "");
    m_trigWorkingPoint = sfConfig.value("trigWorkingPoint", "");
    m_correlationModel = sfConfig.value("correlationModel", "TOTAL");
    m_correctionFileNameList = sfConfig.value("correctionFileNameList", std::vector<std::string>());
    for (auto &correctionFileName : m_correctionFileNameList)
    {
        correctionFileName = PathResolverFindCalibFile(correctionFileName);
    }
    if (!m_idWorkingPoint.empty())
    {
        m_toolName += ("_pid_" + m_idWorkingPoint);
    }
    if (!m_isoWorkingPoint.empty())
    {
        m_toolName += ("_iso_" + m_isoWorkingPoint);
    }
    if (!m_recoWorkingPoint.empty())
    {
        m_toolName += ("_reco_" + m_trigWorkingPoint);
    }
    if (!m_trigWorkingPoint.empty())
    {
        m_toolName += ("_trig_" + m_trigWorkingPoint);
    }
}

AnaQ::ElectronEfficiencyCorrection::ElectronEfficiencyCorrection(Json const &sfConfig,
                                                                 CP::SystematicVariation const &sysVar)
    : ElectronEfficiencyCorrection(sfConfig)
{
    m_sysSet = std::vector<CP::SystematicVariation>{sysVar};
    m_toolName += ("_" + m_sysSet.name());
}

void AnaQ::ElectronEfficiencyCorrection::initialize(unsigned int slot, unsigned long long, unsigned long long)
{
    if (!m_elEffCorrTool)
    {
        m_elEffCorrTool =
            std::make_unique<AsgElectronEfficiencyCorrectionTool>(m_toolName + "_slot" + std::to_string(slot));
        m_elEffCorrTool->setProperty("ForceDataType", m_simulationFlavour).ignore();
        m_elEffCorrTool->setProperty("CorrelationModel", m_correlationModel).ignore();
        // m_elEffCorrTool->setProperty("IdKey", m_idWorkingPoint).ignore();
        // m_elEffCorrTool->setProperty("IsoKey", m_isoWorkingPoint).ignore();
        // m_elEffCorrTool->setProperty("RecoKey", m_recoWorkingPoint).ignore();
        // m_elEffCorrTool->setProperty("TriggerKey", m_trigWorkingPoint).ignore();
        m_elEffCorrTool->setProperty("CorrectionFileNameList", m_correctionFileNameList).ignore();
    }
    m_elEffCorrTool->initialize().ignore();
}

ROOT::RVec<double> AnaQ::ElectronEfficiencyCorrection::evaluate(
    AnaQ::Observable<ConstDataVector<xAOD::ElectronContainer>> electrons) const
{
    if (m_elEffCorrTool->applySystematicVariation(m_sysSet) != EL::StatusCode::SUCCESS)
    {
        throw std::runtime_error("Failed to apply systematic variation '" + m_sysSet.name() +
                                 "' on electron efficiency correction tool '" + m_toolName + "'");
    }

    ROOT::RVec<double> effSFs;
    effSFs.reserve(electrons->size());
    for (auto elItr : *(electrons))
    {
        double effSF = -1.0;
        auto status = m_elEffCorrTool->getEfficiencyScaleFactor(*elItr, effSF);
        if (status == CP::CorrectionCode::Error)
        {
            // ANA_MSG_ERROR( "Problem in PID getEfficiencyScaleFactor Tool");
            // return EL::StatusCode::FAILURE;
            throw std::runtime_error("Problem in electron efficiency correction tool '" + m_toolName + "'");
        }
        else if (status == CP::CorrectionCode::OutOfValidityRange)
        {
            // ANA_MSG_DEBUG( "Electron of of PID efficiency validity range");
        }
        effSFs.push_back(effSF);
    }
    return effSFs;
}

void AnaQ::ElectronEfficiencyCorrection::finalize(unsigned int)
{
    m_elEffCorrTool.reset();
}