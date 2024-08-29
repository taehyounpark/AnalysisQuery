#pragma once

#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicsUtil.h"

#include <queryosity.hpp>

namespace EventFlow 
{

template <typename Act, typename Cfg> class Action : public Act {

public:
  Action(Cfg const &cfg);

  Cfg const *cfg() const;

protected:
  Cfg m_cfg;
};

template <typename Def, typename Cfg>
class Column : public Action<qty::column::definition<Def>, Cfg> {

public:
  Column(Cfg const &cfg);
  Column(Cfg const &cfg, CP::SystematicVariation const &var);

protected:
  CP::SystematicSet m_sysSet = {};
};

template <typename Qry, typename Cfg>
using Query = Action<qty::query::definition<Qry>, Cfg>;

}

template <typename Act, typename Cfg>
EventFlow::Action<Act, Cfg>::Action(Cfg const &cfg) : m_cfg(cfg) {}

template <typename Act, typename Cfg>
Cfg const *EventFlow::Action<Act, Cfg>::cfg() const {
  return &m_cfg;
}

template <typename Def, typename Cfg>
EventFlow::Column<Def, Cfg>::Column(Cfg const &cfg)
    : Action<qty::column::definition<Def>, Cfg>(cfg) {}

template <typename Def, typename Cfg>
EventFlow::Column<Def, Cfg>::Column(Cfg const &cfg,
                               CP::SystematicVariation const &var)
    : Action<qty::column::definition<Def>, Cfg>(cfg), m_sysSet({var}) {}