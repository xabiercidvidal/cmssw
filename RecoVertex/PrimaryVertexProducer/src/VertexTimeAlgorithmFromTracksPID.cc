#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ValidatedPluginMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "vdt/vdtMath.h"

#include "RecoVertex/PrimaryVertexProducer/interface/VertexTimeAlgorithmFromTracksPID.h"

#ifdef PVTX_DEBUG
#define LOG edm::LogPrint("VertexTimeAlgorithmFromTracksPID")
#else
#define LOG LogDebug("VertexTimeAlgorithmFromTracksPID")
#endif

VertexTimeAlgorithmFromTracksPID::VertexTimeAlgorithmFromTracksPID(edm::ParameterSet const& iConfig,
                                                                   edm::ConsumesCollector& iCC)
    : VertexTimeAlgorithmBase(iConfig, iCC),
      trackMTDTimingInfoToken_(
          iCC.consumes(iConfig.getParameter<edm::InputTag>("trackMTDTimingInfoVMapTag"))),
      trackMTDTimeQualityToken_(iCC.consumes(iConfig.getParameter<edm::InputTag>("trackMTDTimeQualityVMapTag"))),
      minTrackVtxWeight_(iConfig.getParameter<double>("minTrackVtxWeight")),
      minTrackTimeQuality_(iConfig.getParameter<double>("minTrackTimeQuality")),
      probPion_(iConfig.getParameter<double>("probPion")),
      probKaon_(iConfig.getParameter<double>("probKaon")),
      probProton_(iConfig.getParameter<double>("probProton")),
      Tstart_(iConfig.getParameter<double>("Tstart")),
      coolingFactor_(iConfig.getParameter<double>("coolingFactor")),
      useMVAVtxTime_(iConfig.getParameter<bool>("useMVAVtxTime")) {}

void VertexTimeAlgorithmFromTracksPID::fillPSetDescription(edm::ParameterSetDescription& iDesc) {
  VertexTimeAlgorithmBase::fillPSetDescription(iDesc);

  iDesc.add<edm::InputTag>("trackMTDTimingInfoVMapTag", edm::InputTag("trackExtenderWithMTD:mtdTimingInfo"))
      ->setComment("Input ValueMap of MTDTimingInfo from TrackExtenderWithMTD");
  iDesc.add<edm::InputTag>("trackMTDTimeQualityVMapTag", edm::InputTag("mtdTrackQualityMVA:mtdQualMVA"))
      ->setComment("Input ValueMap for track MVA quality value");

  iDesc.add<double>("minTrackVtxWeight", 0.5)->setComment("Minimum track weight");
  iDesc.add<double>("minTrackTimeQuality", 0.8)->setComment("Minimum MVA Quality selection on tracks");

  iDesc.add<double>("probPion", 0.7)->setComment("A priori probability pions");
  iDesc.add<double>("probKaon", 0.2)->setComment("A priori probability kaons");
  iDesc.add<double>("probProton", 0.1)->setComment("A priori probability protons");

  iDesc.add<double>("Tstart", 256.)->setComment("DA initial temperature T");
  iDesc.add<double>("coolingFactor", 0.5)->setComment("DA cooling factor");
  iDesc.add<bool>("useMVAVtxTime", true)->setComment("Use MVA quality selection for vertex time calculation");
}

void VertexTimeAlgorithmFromTracksPID::setEvent(edm::Event& iEvent, edm::EventSetup const&) {
  // additional collections required for vertex-time calculation
  trackMTDTimingInfo_ = iEvent.get(trackMTDTimingInfoToken_);
  trackMTDTimeQualities_ = iEvent.get(trackMTDTimeQualityToken_);
}

bool VertexTimeAlgorithmFromTracksPID::vertexTime(float& vtxTime,
                                                  float& vtxTimeError,
                                                  const TransientVertex& vtx) const {
  if (vtx.originalTracks().empty()) {
    return false;
  }

  auto const vtxTime_init = vtxTime;
  auto const vtxTimeError_init = vtxTimeError;
  const int max_iterations = 100;

  double tsum = 0;
  double wsum = 0;
  double w2sum = 0;

  double const a[3] = {probPion_, probKaon_, probProton_};

  std::vector<TrackInfo> v_trackInfo;
  v_trackInfo.reserve(vtx.originalTracks().size());

  // initial guess
  for (const auto& trk : vtx.originalTracks()) {
    auto const trkWeight = vtx.trackWeight(trk);
    if (trkWeight > minTrackVtxWeight_) {
      auto const trkTimeQuality = trackMTDTimeQualities_[trk.trackBaseRef()];
      if (!useMVAVtxTime_ || (useMVAVtxTime_ && trkTimeQuality >= minTrackTimeQuality_)) {
        const auto& tinfo = trackMTDTimingInfo_[trk.trackBaseRef()];
        auto const trkTime = tinfo.tmtd();
        auto const trkTimeError = tinfo.sigmaTmtd();

        v_trackInfo.emplace_back();
        auto& trkInfo = v_trackInfo.back();

        trkInfo.trkWeight = trkWeight;
        trkInfo.trkTimeErrorHyp[0] =
            std::sqrt(trkTimeError * trkTimeError + tinfo.sigmaTofPi() * tinfo.sigmaTofPi());
        trkInfo.trkTimeErrorHyp[1] =
            std::sqrt(trkTimeError * trkTimeError + tinfo.sigmaTofK() * tinfo.sigmaTofK());
        trkInfo.trkTimeErrorHyp[2] =
            std::sqrt(trkTimeError * trkTimeError + tinfo.sigmaTofP() * tinfo.sigmaTofP());

        trkInfo.trkTimeHyp[0] = trkTime - tinfo.tofPi();
        trkInfo.trkTimeHyp[1] = trkTime - tinfo.tofK();
        trkInfo.trkTimeHyp[2] = trkTime - tinfo.tofP();

        double const wgt[3] = {trkWeight / (trkInfo.trkTimeErrorHyp[0] * trkInfo.trkTimeErrorHyp[0]),
                               trkWeight / (trkInfo.trkTimeErrorHyp[1] * trkInfo.trkTimeErrorHyp[1]),
                               trkWeight / (trkInfo.trkTimeErrorHyp[2] * trkInfo.trkTimeErrorHyp[2])};

        for (uint j = 0; j < 3; ++j) {
          wsum += wgt[j] * a[j];
          tsum += wgt[j] * a[j] * trkInfo.trkTimeHyp[j];
        }

        LOG << "vertexTimeFromTracks:     track"
            << " pt=" << trk.track().pt() << " eta=" << trk.track().eta() << " phi=" << trk.track().phi()
            << " vtxWeight=" << trkWeight << " time=" << trkTime << " timeError=" << trkTimeError
            << " timeQuality=" << trkTimeQuality << " timeHyp[pion]=" << trkInfo.trkTimeHyp[0]
            << " +/- " << trkInfo.trkTimeErrorHyp[0] << " timeHyp[kaon]=" << trkInfo.trkTimeHyp[1]
            << " +/- " << trkInfo.trkTimeErrorHyp[1] << " timeHyp[proton]=" << trkInfo.trkTimeHyp[2]
            << " +/- " << trkInfo.trkTimeErrorHyp[2];
      }
    }
  }
  if (wsum > 0) {
    auto t0 = tsum / wsum;
    auto beta = 1. / Tstart_;
    int nit = 0;
    while ((nit++) < max_iterations) {
      tsum = 0;
      wsum = 0;
      w2sum = 0;

      for (auto const& trkInfo : v_trackInfo) {
        double dt[3] = {trkInfo.trkTimeErrorHyp[0], trkInfo.trkTimeErrorHyp[1], trkInfo.trkTimeErrorHyp[2]};
        double e[3] = {0, 0, 0};
        const double cut_off = 4.5;
        double Z = vdt::fast_exp(
            -beta * cut_off);  // outlier rejection term Z_0 = exp(-beta * cut_off) = exp(-beta * 0.5 * 3 * 3)

        for (unsigned int j = 0; j < 3; j++) {
          auto const tpull = (trkInfo.trkTimeHyp[j] - t0) / dt[j];
          e[j] = vdt::fast_exp(-0.5 * beta * tpull * tpull);
          Z += a[j] * e[j];
        }

        double wsum_trk = 0, wsum_sigma_trk = 0;
        for (uint j = 0; j < 3; j++) {
          double wt = a[j] * e[j] / Z;
          double w = wt * trkInfo.trkWeight / (dt[j] * dt[j]);
          wsum_trk += w;
          wsum_sigma_trk += w * dt[j];
          tsum += w * trkInfo.trkTimeHyp[j];
        }

        wsum += wsum_trk;
        w2sum += wsum_sigma_trk * wsum_sigma_trk;
      }

      if (wsum < 1e-10) {
        LOG << "vertexTimeFromTracks:   failed while iterating";
        return false;
      }

      vtxTime = tsum / wsum;

      LOG << "vertexTimeFromTracks:   iteration=" << nit << ", T= " << 1 / beta << ", t=" << vtxTime
          << ", t-t0=" << vtxTime - t0;

      if ((std::abs(vtxTime - t0) < 1e-4 / std::sqrt(beta)) and beta >= 1.) {
        vtxTimeError = std::sqrt(w2sum) / wsum;

        LOG << "vertexTimeFromTracks:   tfit = " << vtxTime << " +/- " << vtxTimeError << " trec = " << vtx.time()
            << ", iteration=" << nit;

        return true;
      }

      if ((std::abs(vtxTime - t0) < 1e-3) and beta < 1.) {
        beta = std::min(1., beta / coolingFactor_);
      }

      t0 = vtxTime;
    }

    LOG << "vertexTimeFromTracks: failed to converge";
  } else {
    LOG << "vertexTimeFromTracks: has no track timing info";
  }

  vtxTime = vtxTime_init;
  vtxTimeError = vtxTimeError_init;

  return false;
}
