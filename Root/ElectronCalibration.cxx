#include "AnaQuery/ElectronCalibration.h"

#include <iostream>

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

AnaQ::ElectronCalibration::ElectronCalibration(Json const &calibCfg, CP::SystematicVariation const &sysVar)
    : ObjectCalibration<xAOD::ElectronContainer>::ObjectCalibration(sysVar)
{
    m_esModel = calibCfg["esModel"].get<std::string>();
    m_decorrelationModel = calibCfg["decorrelationModel"].get<std::string>();
    m_useFastSim = calibCfg.value("useFastSim", false);
    m_sortByPt = calibCfg.value("sortByPt", true);
    m_applyIsolationCorrection = calibCfg.value("applyIsolationCorrection", false);
    m_randomRunNumber = calibCfg.value("randomRunNumber", -1);
}

void AnaQ::ElectronCalibration::initialize(unsigned int slot, unsigned long long, unsigned long long)
{
    // CP::EgammaCalibrationAndSmearingTool
    const auto p4CorrToolName = "EgammaCalibrationAndSmearingTool" + std::to_string(slot) + m_sysSet.name();
    m_p4CorrTool = std::make_unique<CP::EgammaCalibrationAndSmearingTool>(p4CorrToolName);
    m_p4CorrTool->msg().setLevel(MSG::ERROR); // DEBUG, VERBOSE, INFO
    m_p4CorrTool->setProperty("ESModel", m_esModel).ignore();
    m_p4CorrTool->setProperty("decorrelationModel", m_decorrelationModel).ignore();
    m_p4CorrTool->setProperty("useFastSim", (int)m_useFastSim).ignore();
    if (m_randomRunNumber >= 0)
        m_p4CorrTool->setProperty("randomRunNumber", m_randomRunNumber).ignore();
    m_p4CorrTool->initialize().ignore();
    m_p4CorrTool->applySystematicVariation(m_sysSet).ignore();

    // CP::IsolationCorrectionTool
    const auto isoCorrToolName = "IsolationCorrectionTool_" + m_sysSet.name() + "_slot" + std::to_string(slot);
    m_isoCorrTool = std::make_unique<CP::IsolationCorrectionTool>(isoCorrToolName);
    m_isoCorrTool->msg().setLevel(MSG::INFO); // DEBUG, VERBOSE, INFO
    m_isoCorrTool->setProperty("IsMC", true).ignore();
    m_isoCorrTool->initialize().ignore();
}

ConstDataVector<xAOD::ElectronContainer> AnaQ::ElectronCalibration::evaluate(
    qty::column::observable<xAOD::ElectronContainer> elCont) const
{
    ConstDataVector<xAOD::ElectronContainer> calibratedElectrons;

    // keep shallow copy for the event
    m_shallowCopy = ShallowCopy(xAOD::shallowCopyContainer(*elCont));

    // calibration each electron
    for (auto elecItr : *(m_shallowCopy.elements))
    {
        if (elecItr->caloCluster() && elecItr->trackParticle())
        {
            // std::cout << "electron pt correction (before) = " << elecItr->pt();
            if (m_p4CorrTool->applyCorrection(*elecItr) != CP::CorrectionCode::Ok)
            {
                throw std::runtime_error("Problem in "
                                         "CP::EgammaCalibrationAndSmearingTool::applyCorrection()");
            }
            // std::cout << ", (after) = " << elecItr->pt();
            // std::cout << std::endl;
            if (m_applyIsolationCorrection)
            {
                if (elecItr->pt() > 7e3 && m_isoCorrTool->CorrectLeakage(*elecItr) != CP::CorrectionCode::Ok)
                {
                    throw std::runtime_error("Problem in CP::IsolationCorrectionTool::CorrectLeakage()");
                }
            }
        }

        if (!xAOD::setOriginalObjectLink(*elCont, *m_shallowCopy.elements))
        {
            throw std::runtime_error("Failed to set original object links -- MET "
                                     "rebuilding cannot proceed.");
            // ANA_MSG_ERROR( "Failed to set original object links -- MET rebuilding
            // cannot proceed.");
        }
    }
    auto calibElCont = EventHelpers::makeConstDataVector(m_shallowCopy.elements.get());

    // sort by pT if the ordering might have changed pre- vs. post-calibration
    if (m_sortByPt)
    {
        std::sort(calibElCont.begin(), calibElCont.end(), EventHelpers::sortByPt);
    }

    return calibElCont;
}

void AnaQ::ElectronCalibration::finalize(unsigned int)
{
    this->m_shallowCopy = {};
}