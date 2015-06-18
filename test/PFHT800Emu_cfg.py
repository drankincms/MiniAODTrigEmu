import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
       '/store/mc/RunIISpring15DR74/RSGluonToTT_M-3000_TuneCUETP8M1_13TeV-pythia8/MINIAODSIM/Asympt25ns_MCRUN2_74_V9-v2/10000/24B62621-250E-E511-9E62-0025905A6088.root',
    )
)

process.hltPFHT800emu = cms.EDFilter("MiniAODTrigEmu",
    bits = cms.InputTag("TriggerResults","","HLT"),
    objects = cms.InputTag("selectedPatTrigger"),
    origpath = cms.string("HLT_PFHTForMC_v1"),# original path to use as a base
    newthresh = cms.double(800.),# new threshold to use
    triggertype = cms.int32(89),# look at https://github.com/cms-sw/cmssw/blob/CMSSW_7_4_X/DataFormats/HLTReco/interface/TriggerTypeDefs.h
)

process.HLT_PFHT800_v1 = cms.Path(process.hltPFHT800emu)
