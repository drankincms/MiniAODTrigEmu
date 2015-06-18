// -*- C++ -*-
//
// Package:    UserCode/MiniAODTrigEmu
// Class:      MiniAODTrigEmu
// 
/**\class MiniAODTrigEmu MiniAODTrigEmu.cc UserCode/MiniAODTrigEmu/plugins/MiniAODTrigEmu.cc

 Description: filter to allow trigger emulation (tightening of thresholds only) directly from miniAOD without RAW

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  dylan rankin
//         Created:  Thu, 18 Jun 2015 08:21:16 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"

//
// class declaration
//

class MiniAODTrigEmu : public edm::EDFilter {
   public:
      explicit MiniAODTrigEmu(const edm::ParameterSet&);
      ~MiniAODTrigEmu();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      
      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
      edm::EDGetTokenT<edm::TriggerResults> triggerBits_;
      edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> triggerObjects_;

      std::string origPath_;
      double newThresh_;
      int triggerType_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
MiniAODTrigEmu::MiniAODTrigEmu(const edm::ParameterSet& iConfig):
    triggerBits_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("bits"))),
    triggerObjects_(consumes<pat::TriggerObjectStandAloneCollection>(iConfig.getParameter<edm::InputTag>("objects"))),
    origPath_(iConfig.getParameter<std::string>("origpath")),//original path to use as a base
    newThresh_(iConfig.getParameter<double>("newthresh")),//new threshold to use on top of original path
    triggerType_(iConfig.getParameter<int>("triggertype"))//trigger type used
{
   //now do what ever initialization is needed

}


MiniAODTrigEmu::~MiniAODTrigEmu()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
MiniAODTrigEmu::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    edm::Handle<edm::TriggerResults> triggerBits;
    edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;

    iEvent.getByToken(triggerBits_, triggerBits);
    iEvent.getByToken(triggerObjects_, triggerObjects);

    const edm::TriggerNames &names = iEvent.triggerNames(*triggerBits);
    for (unsigned int i = 0, n = triggerBits->size(); i < n; ++i) {
        if (names.triggerName(i)==origPath_ && triggerBits->accept(i)) {
            //std::cout << "Found path pass... ";
            for (pat::TriggerObjectStandAlone obj : *triggerObjects) {
                obj.unpackPathNames(names);
                for (unsigned h = 0; h < obj.filterIds().size(); ++h) {
                    if (obj.filterIds()[h]==triggerType_ && obj.hasPathName( origPath_, true, true )) { //look at https://github.com/cms-sw/cmssw/blob/CMSSW_7_4_X/DataFormats/HLTReco/interface/TriggerTypeDefs.h for an explanation of trigger types
                        //std::cout << "Found correct type and path object... ";
                        if (obj.pt()>newThresh_) {
                            //std::cout << "Found passing object!" << std::endl;
                            return true;
                        }
                    }
                }
            }
        }
    }
    //std::cout << std::endl;

   return false;
}

// ------------ method called once each job just before starting event loop  ------------
void 
MiniAODTrigEmu::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
MiniAODTrigEmu::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
MiniAODTrigEmu::beginRun(edm::Run const&, edm::EventSetup const&)
{ 
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
MiniAODTrigEmu::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
MiniAODTrigEmu::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
MiniAODTrigEmu::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MiniAODTrigEmu::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(MiniAODTrigEmu);
