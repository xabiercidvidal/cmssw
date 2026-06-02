#ifndef DataFormats_MTDReco_MTDTimingInfo_h
#define DataFormats_MTDReco_MTDTimingInfo_h

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

namespace reco {

  class MTDTimingInfo {
  public:
    MTDTimingInfo() = default;

    // --- setters ---
    void setT0(float v) { t0_ = v; }
    void setSigmaT0(float v) { sigmaT0_ = v; }
    void setBeta(float v) { beta_ = v; }
    void setP(float v) { p_ = v; }
    void setPathLength(float v) { pathLength_ = v; }
    void setTmtd(float v) { tmtd_ = v; }
    void setSigmaTmtd(float v) { sigmaTmtd_ = v; }
    void setTofPi(float v) { tofPi_ = v; }
    void setSigmaTofPi(float v) { sigmaTofPi_ = v; }
    void setTofK(float v) { tofK_ = v; }
    void setSigmaTofK(float v) { sigmaTofK_ = v; }
    void setTofP(float v) { tofP_ = v; }
    void setSigmaTofP(float v) { sigmaTofP_ = v; }
    void setBtlMatchChi2(float v) { btlMatchChi2_ = v; }
    void setBtlMatchTimeChi2(float v) { btlMatchTimeChi2_ = v; }
    void setEtlMatchChi2(float v) { etlMatchChi2_ = v; }
    void setEtlMatchTimeChi2(float v) { etlMatchTimeChi2_ = v; }
    void setOutermostHitPosition(float v) { outermostHitPosition_ = v; }
    void setNpixBarrel(int v) { npixBarrel_ = v; }
    void setNpixEndcap(int v) { npixEndcap_ = v; }
    void setMtdPos(GlobalPoint v) { mtdPos_ = v; }

    // --- getters ---
    float t0() const { return t0_; }
    float sigmaT0() const { return sigmaT0_; }
    float beta() const { return beta_; }
    float p() const { return p_; }
    float pathLength() const { return pathLength_; }
    float tmtd() const { return tmtd_; }
    float sigmaTmtd() const { return sigmaTmtd_; }
    float tofPi() const { return tofPi_; }
    float sigmaTofPi() const { return sigmaTofPi_; }
    float tofK() const { return tofK_; }
    float sigmaTofK() const { return sigmaTofK_; }
    float tofP() const { return tofP_; }
    float sigmaTofP() const { return sigmaTofP_; }
    float btlMatchChi2() const { return btlMatchChi2_; }
    float btlMatchTimeChi2() const { return btlMatchTimeChi2_; }
    float etlMatchChi2() const { return etlMatchChi2_; }
    float etlMatchTimeChi2() const { return etlMatchTimeChi2_; }
    float outermostHitPosition() const { return outermostHitPosition_; }
    int npixBarrel() const { return npixBarrel_; }
    int npixEndcap() const { return npixEndcap_; }
    GlobalPoint mtdPos() const { return mtdPos_; }

  private:
    float t0_ = 0.f;
    float sigmaT0_ = -1.f;
    float beta_ = 0.f;
    float p_ = 0.f;
    float pathLength_ = -1.f;
    float tmtd_ = 0.f;
    float sigmaTmtd_ = -1.f;
    float tofPi_ = 0.f;
    float sigmaTofPi_ = -1.f;
    float tofK_ = 0.f;
    float sigmaTofK_ = -1.f;
    float tofP_ = 0.f;
    float sigmaTofP_ = -1.f;
    float btlMatchChi2_ = -1.f;
    float btlMatchTimeChi2_ = -1.f;
    float etlMatchChi2_ = -1.f;
    float etlMatchTimeChi2_ = -1.f;
    float outermostHitPosition_ = 0.f;
    int npixBarrel_ = -1;
    int npixEndcap_ = -1;
    GlobalPoint mtdPos_{0.f, 0.f, 0.f};
  };

}  // namespace reco

#endif
