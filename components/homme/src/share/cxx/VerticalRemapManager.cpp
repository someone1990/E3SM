/********************************************************************************
 * HOMMEXX 1.0: Copyright of Sandia Corporation
 * This software is released under the BSD license
 * See the file 'COPYRIGHT' in the HOMMEXX/src/share/cxx directory
 *******************************************************************************/

#include "VerticalRemapManager.hpp"
#include "SimulationParams.hpp"
#include "Context.hpp"
#include "Elements.hpp"
#include "Tracers.hpp"
#include "HybridVCoord.hpp"
#include "HommexxEnums.hpp"
#include "RemapFunctor.hpp"
#include "PpmRemap.hpp"

namespace Homme {

struct VerticalRemapManager::Impl {
  Impl(const SimulationParams &params, const Elements &e, const Tracers &t,
       const HybridVCoord &h) {
    if (params.remap_alg == RemapAlg::PPM_FIXED_PARABOLA) {
      if (params.rsplit != 0) {
        remapper = std::make_shared<Remap::RemapFunctor<
            true, Remap::Ppm::PpmVertRemap, Remap::Ppm::PpmFixedParabola> >(
            params.qsize, e.m_state, e.m_derived.m_eta_dot_dpdn, t.qdp, h);
      } else {
        remapper = std::make_shared<Remap::RemapFunctor<
            false, Remap::Ppm::PpmVertRemap, Remap::Ppm::PpmFixedParabola> >(
            params.qsize, e.m_state, e.m_derived.m_eta_dot_dpdn, t.qdp, h);
      }
    } else if (params.remap_alg == RemapAlg::PPM_FIXED_MEANS) {
      if (params.rsplit != 0) {
        remapper = std::make_shared<Remap::RemapFunctor<
            true, Remap::Ppm::PpmVertRemap, Remap::Ppm::PpmFixedMeans> >(
            params.qsize, e.m_state, e.m_derived.m_eta_dot_dpdn, t.qdp, h);
      } else {
        remapper = std::make_shared<Remap::RemapFunctor<
            false, Remap::Ppm::PpmVertRemap, Remap::Ppm::PpmFixedMeans> >(
            params.qsize, e.m_state, e.m_derived.m_eta_dot_dpdn, t.qdp, h);
      }
    } else if (params.remap_alg == RemapAlg::PPM_MIRRORED) {
      if (params.rsplit != 0) {
        remapper = std::make_shared<Remap::RemapFunctor<
            true, Remap::Ppm::PpmVertRemap, Remap::Ppm::PpmMirrored> >(
            params.qsize, e.m_state, e.m_derived.m_eta_dot_dpdn, t.qdp, h);
      } else {
        remapper = std::make_shared<Remap::RemapFunctor<
            false, Remap::Ppm::PpmVertRemap, Remap::Ppm::PpmMirrored> >(
            params.qsize, e.m_state, e.m_derived.m_eta_dot_dpdn, t.qdp, h);
      }
    } else {
      Errors::runtime_abort(
          "Error in VerticalRemapManager: unknown remap algorithm.\n",
          Errors::err_unknown_option);
    }
  }

  std::shared_ptr<Remap::Remapper> remapper;
};

VerticalRemapManager::VerticalRemapManager() {
  const auto &h = Context::singleton().get<HybridVCoord>();
  const auto &p = Context::singleton().get<SimulationParams>();
  const auto &e = Context::singleton().get<Elements>();
  const auto &t = Context::singleton().get<Tracers>();
  assert(p.params_set);
  p_.reset(new Impl(p, e, t, h));
}

void VerticalRemapManager::run_remap(int np1, int np1_qdp, double dt) const {
  assert(p_);
  assert(p_->remapper);
  p_->remapper->run_remap(np1, np1_qdp, dt);
}
}
