#ifndef RECOMTD_TIMINGIDTOOLS_MTDTRACKQUALITYMVA
#define RECOMTD_TIMINGIDTOOLS_MTDTRACKQUALITYMVA

#include "DataFormats/MTDReco/interface/MTDTimingInfo.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "CommonTools/MVAUtils/interface/TMVAEvaluator.h"

#define MTDTRACKQUALITYMVA_VARS(MTDBDTVAR) \
  MTDBDTVAR(Track_pt)                      \
  MTDBDTVAR(Track_eta)                     \
  MTDBDTVAR(Track_phi)                     \
  MTDBDTVAR(Track_dz)                      \
  MTDBDTVAR(Track_dxy)                     \
  MTDBDTVAR(Track_chi2)                    \
  MTDBDTVAR(Track_ndof)                    \
  MTDBDTVAR(Track_npixBarrelValidHits)     \
  MTDBDTVAR(Track_npixEndcapValidHits)     \
  MTDBDTVAR(Track_BTLchi2)                 \
  MTDBDTVAR(Track_BTLtime_chi2)            \
  MTDBDTVAR(Track_ETLchi2)                 \
  MTDBDTVAR(Track_ETLtime_chi2)            \
  MTDBDTVAR(Track_Tmtd)                    \
  MTDBDTVAR(Track_sigmaTmtd)               \
  MTDBDTVAR(Track_length)                  \
  MTDBDTVAR(Track_lHitPos)

#define MTDBDTVAR_ENUM(ENUM) ENUM,
#define MTDBDTVAR_STRING(STRING) #STRING,

class MTDTrackQualityMVA {
public:
  //---ctors---
  MTDTrackQualityMVA(std::string weights_file);

  enum class VarID { MTDTRACKQUALITYMVA_VARS(MTDBDTVAR_ENUM) };

  //---getters---
  // 4D
  float operator()(const reco::TrackRef& trk,
                   const reco::BeamSpot& beamspot,
                   const reco::MTDTimingInfo& timingInfo) const;

private:
  std::vector<std::string> vars_, spec_vars_;
  std::unique_ptr<TMVAEvaluator> mva_;
};

#endif
