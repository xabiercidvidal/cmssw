#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

#include "DataFormats/MTDReco/interface/MTDTimingInfo.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "RecoMTD/TimingIDTools/interface/MTDTrackQualityMVA.h"

using namespace std;
using namespace edm;

class MTDTrackQualityMVAProducer : public edm::stream::EDProducer<> {
public:
  MTDTrackQualityMVAProducer(const ParameterSet& pset);

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  template <class H, class T>
  void fillValueMap(edm::Event& iEvent,
                    const edm::Handle<H>& handle,
                    const std::vector<T>& vec,
                    const std::string& name) const;

  void produce(edm::Event& ev, const edm::EventSetup& es) final;

private:
  static constexpr char mvaName[] = "mtdQualMVA";

  edm::EDGetTokenT<reco::TrackCollection> tracksToken_;
  edm::EDGetTokenT<edm::ValueMap<reco::MTDTimingInfo>> mtdTimingInfoToken_;
  edm::EDGetTokenT<reco::BeamSpot> RecBeamSpotToken_;

  MTDTrackQualityMVA mva_;
};

MTDTrackQualityMVAProducer::MTDTrackQualityMVAProducer(const ParameterSet& iConfig)
    : tracksToken_(consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("tracksSrc"))),
      mtdTimingInfoToken_(
          consumes<edm::ValueMap<reco::MTDTimingInfo>>(iConfig.getParameter<edm::InputTag>("mtdTimingInfoSrc"))),
      RecBeamSpotToken_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("offlineBS"))),
      mva_(iConfig.getParameter<edm::FileInPath>("qualityBDT_weights_file").fullPath()) {
  produces<edm::ValueMap<float>>(mvaName);
}

// Configuration descriptions
void MTDTrackQualityMVAProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("tracksSrc", edm::InputTag("generalTracks"))->setComment("Input tracks collection");
  desc.add<edm::InputTag>("mtdTimingInfoSrc", edm::InputTag("trackExtenderWithMTD:mtdTimingInfo"))
      ->setComment("Input ValueMap of MTDTimingInfo from TrackExtenderWithMTD");
  desc.add<edm::InputTag>("offlineBS", edm::InputTag("offlineBeamSpot"));
  desc.add<edm::FileInPath>("qualityBDT_weights_file",
                            edm::FileInPath("RecoMTD/TimingIDTools/data/BDT_nvars_17_d7.xml"))
      ->setComment("Track MTD quality BDT weights");
  descriptions.add("mtdTrackQualityMVAProducer", desc);
}

template <class H, class T>
void MTDTrackQualityMVAProducer::fillValueMap(edm::Event& iEvent,
                                              const edm::Handle<H>& handle,
                                              const std::vector<T>& vec,
                                              const std::string& name) const {
  auto out = std::make_unique<edm::ValueMap<T>>();
  typename edm::ValueMap<T>::Filler filler(*out);
  filler.insert(handle, vec.begin(), vec.end());
  filler.fill();
  iEvent.put(std::move(out), name);
}

void MTDTrackQualityMVAProducer::produce(edm::Event& ev, const edm::EventSetup& es) {
  edm::Handle<reco::TrackCollection> tracksH;
  ev.getByToken(tracksToken_, tracksH);
  const auto& tracks = *tracksH;

  reco::BeamSpot beamSpot;
  edm::Handle<reco::BeamSpot> BeamSpotH;
  ev.getByToken(RecBeamSpotToken_, BeamSpotH);
  beamSpot = *BeamSpotH;

  const auto& mtdTimingInfo = ev.get(mtdTimingInfoToken_);

  std::vector<float> mvaOutRaw;

  //Loop over tracks collection
  for (unsigned int itrack = 0; itrack < tracks.size(); ++itrack) {
    const reco::TrackRef trackref(tracksH, itrack);
    if (mtdTimingInfo[trackref].pathLength() == -1.)
      mvaOutRaw.push_back(-1.);
    else {
      mvaOutRaw.push_back(mva_(trackref, beamSpot, mtdTimingInfo[trackref]));
    }
  }
  fillValueMap(ev, tracksH, mvaOutRaw, mvaName);
}

//define this as a plug-in
#include <FWCore/Framework/interface/MakerMacros.h>
DEFINE_FWK_MODULE(MTDTrackQualityMVAProducer);
