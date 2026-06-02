
#ifndef usercode_PrimaryVertexAnalyzer_VertexTimeAlgorithmFromTracksPID_h
#define usercode_PrimaryVertexAnalyzer_VertexTimeAlgorithmFromTracksPID_h

#include "VertexTimeAlgorithmBase.h"

#include "FWCore/Utilities/interface/EDGetToken.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/MTDReco/interface/MTDTimingInfo.h"

class VertexTimeAlgorithmFromTracksPID : public VertexTimeAlgorithmBase {
public:
  VertexTimeAlgorithmFromTracksPID(const edm::ParameterSet& conf, edm::ConsumesCollector& iC);
  ~VertexTimeAlgorithmFromTracksPID() override = default;

  static void fillPSetDescription(edm::ParameterSetDescription& iDesc);

  void setEvent(edm::Event& iEvent, edm::EventSetup const& iSetup) override;

  bool vertexTime(float& vtxTime, float& vtxTimeError, TransientVertex const& vtx) const override;

protected:
  struct TrackInfo {
    double trkWeight;
    double trkTimeErrorHyp[3];
    double trkTimeHyp[3];
  };

  edm::EDGetTokenT<edm::ValueMap<reco::MTDTimingInfo>> const trackMTDTimingInfoToken_;
  edm::EDGetTokenT<edm::ValueMap<float>> const trackMTDTimeQualityToken_;

  double const minTrackVtxWeight_;
  double const minTrackTimeQuality_;
  double const probPion_;
  double const probKaon_;
  double const probProton_;
  double const Tstart_;
  double const coolingFactor_;
  bool const useMVAVtxTime_;

  edm::ValueMap<reco::MTDTimingInfo> trackMTDTimingInfo_;
  edm::ValueMap<float> trackMTDTimeQualities_;
};

#endif
