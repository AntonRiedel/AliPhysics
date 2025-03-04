//Class for <pt>-v2 correlations

#include "AliAnalysisTaskMeanPtV2Corr.h"
#include "AliEventCuts.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisFilter.h"
#include "AliAnalysisUtils.h"
#include "AliVEvent.h"
#include "AliAODEvent.h"
#include "AliVTrack.h"
#include "AliVParticle.h"
#include "AliAODTrack.h"
#include "AliVVertex.h"
#include "AliAODVertex.h"
#include "AliCentrality.h"
#include "AliExternalTrackParam.h"
#include "AliMultSelection.h"
#include "AliInputEventHandler.h"
#include "TList.h"
#include "TProfile.h"
#include "AliEventCuts.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "AliStack.h"
#include "TClonesArray.h"
#include "TProfile.h"
#include "AliGFWWeights.h"
#include "AliGFWFlowContainer.h"
#include "AliGFW.h"
#include "TClonesArray.h"
#include "AliGFWCuts.h"
#include "AliAODMCParticle.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TNamed.h"
#include "AliPID.h"
#include "AliPIDResponse.h"
#include "AliPIDCombined.h"

ClassImp(AliAnalysisTaskMeanPtV2Corr);

AliAnalysisTaskMeanPtV2Corr::AliAnalysisTaskMeanPtV2Corr():
  AliAnalysisTaskSE(),
  fStageSwitch(0),
  fContSubfix(0),
  fCentEst(0),
  fExtendV0MAcceptance(kTRUE),
  fIsMC(kFALSE),
  fBypassTriggerAndEvetCuts(kFALSE),
  fMCEvent(0),
  fUseRecoNchForMC(kTRUE),
  fRndm(0),
  fNBootstrapProfiles(10),
  fPtAxis(0),
  fMultiAxis(0),
  fV0MMultiAxis(0),
  fPtBins(0),
  fNPtBins(0),
  fMultiBins(0),
  fNMultiBins(0),
  fUseNch(kFALSE),
  fUseWeightsOne(kFALSE),
  fEta(0.8),
  fEtaNch(0.8),
  fEtaV2Sep(0.4),
  fPIDResponse(0),
  fBayesPID(0),
  fMPTList(0),
  fMPTListMC(0),
  fmPT(0),
  fMptClosure(0),
  fMultiDist(0),
  fMultiVsV0MCorr(0),
  fNchVsMulti(0),
  fNchInBins(0),
  fptVarList(0),
  fCkCont(0),
  fptvar(0),
  fCovList(0),
  fV2dPtList(0),
  fCovariance(0),
  fmptSet(kFALSE),
  fTriggerType(AliVEvent::kMB+AliVEvent::kINT7),
  fWeightList(0),
  fWeights(0),
  fNUAList(0),
  fNUAHist(0),
  fRunNo(0),
  fGFWSelection(0),
  fGFWNtotSelection(0),
  fFC(0),
  fGFW(0),
  fEfficiencyList(0),
  fEfficiency(0),
  fEfficiencies(0),
  fPseudoEfficiency(2.),
  fV0MMulti(0),
  fV2dPtMulti(0),
  fDisablePID(kTRUE),
  fConsistencyFlag(3),
  fRequireReloadOnRunChange(kFALSE)
{
};
AliAnalysisTaskMeanPtV2Corr::AliAnalysisTaskMeanPtV2Corr(const char *name, Bool_t IsMC, TString stageSwitch, TString ContSubfix):
  AliAnalysisTaskSE(name),
  fStageSwitch(0),
  fContSubfix(0),
  fCentEst(0),
  fExtendV0MAcceptance(kTRUE),
  fIsMC(IsMC),
  fBypassTriggerAndEvetCuts(kFALSE),
  fMCEvent(0),
  fUseRecoNchForMC(kTRUE),
  fNBootstrapProfiles(10),
  fRndm(0),
  fPtAxis(0),
  fMultiAxis(0),
  fV0MMultiAxis(0),
  fPtBins(0),
  fNPtBins(0),
  fMultiBins(0),
  fNMultiBins(0),
  fUseNch(kFALSE),
  fUseWeightsOne(kFALSE),
  fEta(0.8),
  fEtaNch(0.8),
  fEtaV2Sep(0.4),
  fPIDResponse(0),
  fBayesPID(0),
  fMPTList(0),
  fMPTListMC(0),
  fmPT(0),
  fMptClosure(0),
  fMultiDist(0),
  fMultiVsV0MCorr(0),
  fNchVsMulti(0),
  fNchInBins(0),
  fptVarList(0),
  fCkCont(0),
  fptvar(0),
  fCovList(0),
  fV2dPtList(0),
  fCovariance(0),
  fmptSet(kFALSE),
  fTriggerType(AliVEvent::kMB+AliVEvent::kINT7),
  fWeightList(0),
  fWeights(0),
  fNUAList(0),
  fNUAHist(0),
  fRunNo(0),
  fGFWSelection(0),
  fGFWNtotSelection(0),
  fFC(0),
  fGFW(0),
  fEfficiencyList(0),
  fEfficiency(0),
  fEfficiencies(0),
  fPseudoEfficiency(2.),
  fV0MMulti(0),
  fV2dPtMulti(0),
  fDisablePID(kTRUE),
  fConsistencyFlag(3),
  fRequireReloadOnRunChange(kFALSE)
{
  fStageSwitch = GetStageSwitch(stageSwitch);
  SetContSubfix(ContSubfix);
  fCentEst = new TString("V0M");
  if(!fStageSwitch) AliFatal("Stage switch is 0, not sure what should be done!\n");
  if(fStageSwitch==1)
    DefineOutput(1,TList::Class());
  if(fStageSwitch==2) {
    if(!fIsMC) DefineInput(1,TList::Class());
    DefineOutput(1,TList::Class());
    DefineOutput(2,TH1D::Class());
  };
  if(fStageSwitch==3) {
    DefineInput(1,TList::Class()); //Mean Pt, should be rerun with Bayes PID
    if(!fIsMC) { //Efficiency and NUA only important for data
      DefineInput(2,TList::Class()); //NUE weights; ultimately, should be combined with NUA, but don't want to rerun now
      DefineInput(3,TList::Class()); //NUA weights from other analysis; quickfix
    };
    DefineOutput(1,TList::Class());
    DefineOutput(2,AliGFWFlowContainer::Class());
    DefineOutput(3,TList::Class());
    DefineOutput(4,TList::Class());
  }
  if(fStageSwitch==7) {
    DefineOutput(1,TList::Class());
  }
  if(fStageSwitch==8) { //MC-clusore for Mpt
    DefineInput(1,TList::Class());
    DefineOutput(1,TList::Class());
    DefineOutput(2,TList::Class());
  }
  if(fStageSwitch==9) {
    if(!fIsMC) { //Efficiency and NUA only important for data
      DefineInput(1,TList::Class()); //NUE weights; ultimately, should be combined with NUA, but don't want to rerun now
      DefineInput(2,TList::Class()); //NUA weights from other analysis; quickfix
    };
    DefineOutput(1,TList::Class());
    DefineOutput(2,AliGFWFlowContainer::Class());
    DefineOutput(3,TList::Class());
  };
  SetNchCorrelationCut(1,0,kFALSE);
};
AliAnalysisTaskMeanPtV2Corr::~AliAnalysisTaskMeanPtV2Corr() {
  SetNchCorrelationCut(1,0,kFALSE);
};
void AliAnalysisTaskMeanPtV2Corr::UserCreateOutputObjects(){
  printf("Stage switch is %i\n\n\n",fStageSwitch);
  if(!fGFWSelection) SetSystFlag(0);
  fGFWSelection->PrintSetup();
  if(fGFWSelection->GetSystFlagIndex() == 13) SetCentralityEstimator("CL0");
  else if(fGFWSelection->GetSystFlagIndex() == 14) SetCentralityEstimator("CL1");
  OpenFile(1);
  const Int_t temp_NV0MBinsDefault=fExtendV0MAcceptance?11:10;
  Double_t temp_V0MBinsDefault[12] = {0,5,10,20,30,40,50,60,70,80,90,101}; //Last bin to include V0M beyond anchor point
  if(!fV0MMultiAxis) SetV0MBins(temp_NV0MBinsDefault,temp_V0MBinsDefault);
  Double_t *l_V0MBinsDefault=GetBinsFromAxis(fV0MMultiAxis);
  Int_t l_NV0MBinsDefault=fV0MMultiAxis->GetNbins();
  if(l_V0MBinsDefault[l_NV0MBinsDefault]>90) fExtendV0MAcceptance = kTRUE; //If V0M is beyond 90, then we need to extend the V0M acceptance!
  if(!fMultiAxis) SetMultiBins(l_NV0MBinsDefault,l_V0MBinsDefault);
  fMultiBins = GetBinsFromAxis(fMultiAxis);
  fNMultiBins = fMultiAxis->GetNbins();
  if(!fV2dPtMulti) {
    Double_t temp_bn[] = {0,1e6};
    SetV2dPtMultiBins(1,temp_bn);
  };
  const Int_t l_NPtBinsDefault = 25;
  Double_t l_PtBinsDefault[l_NPtBinsDefault+1] = {0.20, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95,
                     1.00, 1.10, 1.20, 1.30, 1.40, 1.50, 1.60, 1.70, 1.80, 1.90,
                     2.00, 2.20, 2.40, 2.60, 2.80, 3.00};
  if(!fPtAxis) SetPtBins(l_NPtBinsDefault,l_PtBinsDefault);
  fPtBins = GetBinsFromAxis(fPtAxis);
  fNPtBins = fPtAxis->GetNbins();
  TString spNames[] = {"ch","pi","ka","pr"};
  if(fStageSwitch==1) {
    fRequireReloadOnRunChange = kFALSE;
    fWeightList = new TList();
    fWeightList->SetOwner(kTRUE);
      TString wNames[] = {"ch","pi","ka","pr"};
      fWeights = new AliGFWWeights*[4];
      for(Int_t i=0; i<4;i++) {
        fWeights[i] = new AliGFWWeights();
        fWeights[i]->SetPtBins(fNPtBins,fPtBins);
        // fWeights[i]->SetPtBins(NbinsPtForV2,binsPtForV2);
        fWeights[i]->SetName(Form("weight_%s",wNames[i].Data()));
        fWeights[i]->Init(!fIsMC,fIsMC);
        fWeightList->Add(fWeights[i]);
      }
      PostData(1,fWeightList);
  };
  if(fStageSwitch==2) {
    fRndm = new TRandom(0);
    fRequireReloadOnRunChange=kFALSE;
    if(!fIsMC) {
      fEfficiencyList = (TList*)GetInputData(1);
      fEfficiencies = new TH1D*[l_NV0MBinsDefault];
      for(Int_t i=0;i<l_NV0MBinsDefault;i++) {
        fEfficiencies[i] = (TH1D*)fEfficiencyList->FindObject(Form("EffRescaled_Cent%i%s",i,fGFWSelection->GetSystPF()));
        if(fEfficiencies[i] && fPseudoEfficiency<1) fEfficiencies[i]->Scale(fPseudoEfficiency);
        if(!fEfficiencies[i]) {
          if(!i) AliFatal("Could not fetch efficiency!\n");
          printf("Could not find efficiency for V0M bin no. %i! Cloning the previous efficiency instead...\n",i);
          fEfficiencies[i] = (TH1D*)fEfficiencies[i-1]->Clone(Form("EffRescaled_Cent%i%s",i,fGFWSelection->GetSystPF()));
        };
      }
    };
    fMPTList = new TList();
    fMPTList->SetOwner(kTRUE);
    fmPT = new TProfile*[4];
    for(Int_t i=0;i<4;i++) {
      fmPT[i] = new TProfile(Form("MeanPt_%s%s",spNames[i].Data(),fGFWSelection->GetSystPF()),Form("MeanPt_%s",spNames[i].Data()),fNMultiBins,fMultiBins);
      fMPTList->Add(fmPT[i]);
    }
    fMultiDist = new TH1D("MultiDistribution","Multiplicity distribution; #it{N}_{ch}; N(events)",fNMultiBins,fMultiBins);
    fV0MMulti = new TH1D("V0M_Multi","V0M_Multi",l_NV0MBinsDefault,l_V0MBinsDefault);
    fMPTList->Add(fMultiDist);
    fMPTList->Add(fV0MMulti);
    fMultiVsV0MCorr = new TH2D*[2];
    fMultiVsV0MCorr[0] = new TH2D("MultVsV0M_BeforeConsistency","MultVsV0M_BeforeConsistency",103,0,103,fNMultiBins,fMultiBins[0],fMultiBins[fNMultiBins]);
    fMultiVsV0MCorr[1] = new TH2D("MultVsV0M_AfterConsistency","MultVsV0M_AfterConsistency",103,0,103,fNMultiBins,fMultiBins[0],fMultiBins[fNMultiBins]);
    fMPTList->Add(fMultiVsV0MCorr[0]);
    fMPTList->Add(fMultiVsV0MCorr[1]);
    if(fIsMC) {
      fNchTrueVsReco = new TH2D("NchTrueVsReco",";Nch (MC-true); Nch (MC-reco)",fNMultiBins,fMultiBins,fNMultiBins,fMultiBins);
      fMPTList->Add(fNchTrueVsReco);
    }
    PostData(1,fMPTList);
  };
  if(fStageSwitch==3) {
    fRndm = new TRandom(0);
    fRequireReloadOnRunChange = kFALSE;
    fMPTList = (TList*)GetInputData(1);
    if(!fMPTList) AliFatal("Could not fetch input mean pT list!\n");
    fmPT = new TProfile*[4];
    for(Int_t i=0;i<4;i++) {
      fmPT[i] = (TProfile*)fMPTList->FindObject(Form("MeanPt_%s%s",spNames[i].Data(),fGFWSelection->GetSystPF()));
      if(!fmPT[i]) AliFatal("Could not fetch mean pt!\n");
    }
    if(!fIsMC) { //Efficiencies and NUA are only for the data
      fEfficiencyList = (TList*)GetInputData(2);
      fEfficiencies = new TH1D*[l_NV0MBinsDefault];
      for(Int_t i=0;i<l_NV0MBinsDefault;i++) {
        fEfficiencies[i] = (TH1D*)fEfficiencyList->FindObject(Form("EffRescaled_Cent%i%s",i,fGFWSelection->GetSystPF()));
        if(fEfficiencies[i] && fPseudoEfficiency<1) fEfficiencies[i]->Scale(fPseudoEfficiency);
        if(!fEfficiencies[i]) {
          if(!i) AliFatal("Could not fetch efficiency!\n");
          printf("Could not find efficiency for V0M bin no. %i! Cloning the previous efficiency instead...\n",i);
          fEfficiencies[i] = (TH1D*)fEfficiencies[i-1]->Clone(Form("EffRescaled_Cent%i%s",i,fGFWSelection->GetSystPF()));
        };
      }
      fWeightList = (TList*)GetInputData(3);
      fWeights = new AliGFWWeights*[1];
    };
    // if(!LoadMyWeights(0)) return; //Loading run-avg NUA weights
    fptVarList = new TList();
    fptVarList->SetOwner(kTRUE);
    fptvar = new AliProfileBS*[4];
    for(Int_t i=0;i<4;i++) {
      fptVarList->Add(new AliProfileBS(Form("varpt_%s",spNames[i].Data()),Form("varpt_%s",spNames[i].Data()),fNMultiBins,fMultiBins));
      fptvar[i] = (AliProfileBS*)fptVarList->At(i);
      if(fNBootstrapProfiles) fptvar[i]->InitializeSubsamples(fNBootstrapProfiles);
    }
    fMultiDist = new TH1D("MultiDistribution","Multiplicity distribution; #it{N}_{ch}; N(events)",fNMultiBins,fMultiBins);
    fV0MMulti = new TH1D("V0M_Multi","V0M_Multi",l_NV0MBinsDefault,l_V0MBinsDefault);
    fptVarList->Add(fMultiDist);
    fptVarList->Add(fV0MMulti);
    fCkCont = new AliCkContainer("ckContainer","ckCont",fNMultiBins,fMultiBins);
    if(fNBootstrapProfiles) fCkCont->InitializeSubsamples(fNBootstrapProfiles);
    fptVarList->Add(fCkCont);
    fMultiVsV0MCorr = new TH2D*[2];
    fMultiVsV0MCorr[0] = new TH2D("MultVsV0M_BeforeConsistency","MultVsV0M_BeforeConsistency",103,0,103,fNMultiBins,fMultiBins[0],fMultiBins[fNMultiBins]);
    fMultiVsV0MCorr[1] = new TH2D("MultVsV0M_AfterConsistency","MultVsV0M_AfterConsistency",103,0,103,fNMultiBins,fMultiBins[0],fMultiBins[fNMultiBins]);
    fMptClosure = new TProfile(Form("MeanPtClosure_%s%s",spNames[0].Data(),fGFWSelection->GetSystPF()),Form("MeanPtClosure_%s",spNames[0].Data()),fNMultiBins,fMultiBins);
    fptVarList->Add(fMultiVsV0MCorr[0]);
    fptVarList->Add(fMultiVsV0MCorr[1]);
    fptVarList->Add(fMptClosure);
    if(fIsMC) {
      fNchTrueVsReco = new TH2D("NchTrueVsReco",";Nch (MC-true); Nch (MC-reco)",fNMultiBins,fMultiBins,fNMultiBins,fMultiBins);
      fptVarList->Add(fNchTrueVsReco);
    }

    PostData(1,fptVarList);
    //Setting up the FlowContainer
    TObjArray *oba = new TObjArray();
    oba->Add(new TNamed("ChGap22","ChGap22")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("ChGap24","ChGap24")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("ChFull22","ChFull22")); //no-gap case
    oba->Add(new TNamed("ChFull24","ChFull24")); //no-gap case
//adding v3n
    oba->Add(new TNamed("ChGap32","ChGap32")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("ChGap34","ChGap34")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("ChFull32","ChFull32")); //no-gap case
    oba->Add(new TNamed("ChFull34","ChFull34")); //no-gap case

    oba->Add(new TNamed("ChGap42","ChGap42")); //gap case

    oba->Add(new TNamed("LM22","LM22")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("MR22","MR22")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("LR22","LR22")); //for gap (|eta|>0.4) case

    oba->Add(new TNamed("LLMR24","LLMR24")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("LMMR24","LMMR24")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("LMRR24","LMRR24")); //for gap (|eta|>0.4) case

    oba->Add(new TNamed("ChSC234","ChSC234")); //for SC{2,3}
    oba->Add(new TNamed("ChSC244","ChSC244")); //for SC{2,4}

    //Following is for PID. Let's remove it for now to save some memory
/*    oba->Add(new TNamed("ChPos22","ChPos22"));
    oba->Add(new TNamed("ChPos24","ChPos24"));
    oba->Add(new TNamed("PiPos22","PiPos22"));
    oba->Add(new TNamed("PiPos24","PiPos24"));
    oba->Add(new TNamed("KaPos22","KaPos22"));
    oba->Add(new TNamed("KaPos24","KaPos24"));
    oba->Add(new TNamed("PrPos22","PrPos22"));
    oba->Add(new TNamed("PrPos24","PrPos24"));
    oba->Add(new TNamed("ChNeg22","ChNeg22"));
    oba->Add(new TNamed("ChNeg24","ChNeg24"));
    oba->Add(new TNamed("PiNeg22","PiNeg22"));
    oba->Add(new TNamed("PiNeg24","PiNeg24"));
    oba->Add(new TNamed("KaNeg22","KaNeg22"));
    oba->Add(new TNamed("KaNeg24","KaNeg24"));
    oba->Add(new TNamed("PrNeg22","PrNeg22"));
    oba->Add(new TNamed("PrNeg24","PrNeg24"));*/
    fFC = new AliGFWFlowContainer();
    TString fcname("FlowContainer");
    if(!fContSubfix->IsNull()) fcname.Append(fContSubfix->Data());
    // fcname.Append(fGFWSelection->GetSystPF());
    fFC->SetName(fcname.Data());
    fFC->Initialize(oba,fNMultiBins,fMultiBins,fNBootstrapProfiles);
    delete oba;
    PostData(2,fFC);
    //Initializing GFW
    /* //for v2-only

    Int_t pows[] = {3,0,2,0,3};
    Int_t powsFull[] = {5,0,4,0,3};*/
    // Int_t powsPOI[] = {3,0,2,0,3};
    //for v2{4} and v3{4}:
    Int_t pows[] = {3,0,2,2,3,3,3}; //5th harm. sum = 3, b/c {-2 -3}
    Int_t powsFull[] = {5,0,4,4,3,3,3};
    fGFW = new AliGFW();
    fGFW->AddRegion("refN",7,pows,-0.8,-fEtaV2Sep,1,1);
    fGFW->AddRegion("refP",7,pows,fEtaV2Sep,0.8,1,1);
    if(fEtaV2Sep>=0)
      fGFW->AddRegion("subMid",7,pows,-fEtaV2Sep,fEtaV2Sep,1,1);
    fGFW->AddRegion("mid",7,powsFull,-0.8,0.8,1,2);
    //No need to do full-blown PID, limit only with charged flow
    /*
    fGFW->AddRegion("refN",5,pows,-0.8,-0.4,1,1);
    fGFW->AddRegion("refP",5,pows,0.4,0.8,1,1);
    fGFW->AddRegion("chN",3,powsPOI,-0.8,-0.4,1,2);
    fGFW->AddRegion("chP",3,powsPOI,0.4,0.8,1,2);
    fGFW->AddRegion("piN",3,powsPOI,-0.8,-0.4,1,4);
    fGFW->AddRegion("piP",3,powsPOI,0.4,0.8,1,4);
    fGFW->AddRegion("kaN",3,powsPOI,-0.8,-0.4,1,8);
    fGFW->AddRegion("kaP",3,powsPOI,0.4,0.8,1,8);
    fGFW->AddRegion("prN",3,powsPOI,-0.8,-0.4,1,16);
    fGFW->AddRegion("prP",3,powsPOI,0.4,0.8,1,16);
    fGFW->AddRegion("OLchN",5,pows,-0.8,-0.4,1,32);
    fGFW->AddRegion("OLchP",5,pows,0.4,0.8,1,32);
    fGFW->AddRegion("OLpiN",5,pows,-0.8,-0.4,1,64);
    fGFW->AddRegion("OLpiP",5,pows,0.4,0.8,1,64);
    fGFW->AddRegion("OLkaN",5,pows,-0.8,-0.4,1,128);
    fGFW->AddRegion("OLkaP",5,pows,0.4,0.8,1,128);
    fGFW->AddRegion("OLprN",5,pows,-0.8,-0.4,1,256);
    fGFW->AddRegion("OLprP",5,pows,0.4,0.8,1,256);*/
    CreateCorrConfigs();
    //Covariance
    fCovList = new TList();
    fCovList->SetOwner(kTRUE);
    fCovariance = new AliProfileBS*[9];
    for(Int_t i=0;i<4;i++) {
      fCovList->Add(new AliProfileBS(Form("cov_%s",spNames[i].Data()),Form("cov_%s",spNames[i].Data()),fNMultiBins,fMultiBins));
      fCovariance[i] = (AliProfileBS*)fCovList->At(i);
      if(fNBootstrapProfiles) fCovariance[i]->InitializeSubsamples(fNBootstrapProfiles);
    };
    for(Int_t i=0;i<4;i++) {
      fCovList->Add(new AliProfileBS(Form("cov_v3_%s",spNames[i].Data()),Form("cov_v3_%s",spNames[i].Data()),fNMultiBins,fMultiBins));
      fCovariance[4+i] = (AliProfileBS*)fCovList->At(i+4);
      if(fNBootstrapProfiles) fCovariance[4+i]->InitializeSubsamples(fNBootstrapProfiles);
    };
    fCovList->Add(new AliProfileBS(Form("cov_v23_%s",spNames[0].Data()),Form("cov_v23_%s",spNames[0].Data()),fNMultiBins,fMultiBins));
    fCovariance[8] = (AliProfileBS*)fCovList->At(8);
    if(fNBootstrapProfiles) fCovariance[8]->InitializeSubsamples(fNBootstrapProfiles);

    PostData(3,fCovList);
    fV2dPtList = new TList();
    // fV2dPtList->SetName(Form("MPtV2_%i",fSystFlag));
    fV2dPtList->SetOwner(kTRUE);
    fV2dPtList->Add(fV2dPtMulti);
    // delete oba;
    oba = new TObjArray();
    oba->Add(new TNamed("ChGap22","ChGap22"));
    oba->Add(new TNamed("ChGap32","ChGap32"));
    for(Int_t j=0;j<fV2dPtMulti->GetNbinsX();j++) {
      AliGFWFlowContainer *fPV = new AliGFWFlowContainer();
      fPV->SetName(Form("v2dpt_%i",j));
      Double_t mptbins[21];
      for(Int_t i=0;i<21;i++) mptbins[i] = (i - 10.)/100;
      fPV->Initialize(oba,20,mptbins);
      fV2dPtList->Add(fPV);
    };
    delete oba;
    PostData(4,fV2dPtList);
  }
  if(fStageSwitch==7) {
    fRequireReloadOnRunChange = kFALSE;
    fEfficiencyList = new TList();
    fEfficiencyList->SetOwner(kTRUE);
    Int_t nSpecies=7;
    fEfficiency = new TH2D*[nSpecies*3];
    TString lNames[] = {"ch","pi","ka","pr","sigma","xi","om"};
    for(Int_t i=0;i<nSpecies;i++) {
      lNames[i].Prepend("Spectra_");
      fEfficiency[i] = new TH2D(lNames[i].Data(),lNames[i].Data(),fNPtBins,fPtBins,l_NV0MBinsDefault,l_V0MBinsDefault);
      lNames[i].Append("_Gen");
      fEfficiency[nSpecies+i] = new TH2D(lNames[i].Data(),lNames[i].Data(),fNPtBins,fPtBins,l_NV0MBinsDefault,l_V0MBinsDefault);
      lNames[i].Append("_Sec");
      fEfficiency[2*nSpecies+i] = new TH2D(lNames[i].Data(),lNames[i].Data(),fNPtBins,fPtBins,l_NV0MBinsDefault,l_V0MBinsDefault);
      fEfficiencyList->Add(fEfficiency[i]);
      fEfficiencyList->Add(fEfficiency[i+nSpecies]);
      fEfficiencyList->Add(fEfficiency[i+(2*nSpecies)]);
    }
    fV0MMulti = new TH1D("V0M_Multi","V0M_Multi",l_NV0MBinsDefault,l_V0MBinsDefault);
    fEfficiencyList->Add(fV0MMulti);
    fNchTrueVsReco = new TH2D("NchTrueVsReco",";Nch (MC-true); Nch (MC-reco)",fNMultiBins,fMultiBins,fNMultiBins,fMultiBins);
    fEfficiencyList->Add(fNchTrueVsReco);
    PostData(1,fEfficiencyList);
  }
  if(fStageSwitch==8) {
    fRndm = new TRandom(0);
    fRequireReloadOnRunChange=kFALSE;
    fEfficiencyList = (TList*)GetInputData(1);
    fEfficiencies = new TH1D*[l_NV0MBinsDefault];
    for(Int_t i=0;i<l_NV0MBinsDefault;i++) {
      fEfficiencies[i] = (TH1D*)fEfficiencyList->FindObject(Form("EffRescaled_Cent%i%s",i,fGFWSelection->GetSystPF()));
      if(fEfficiencies[i] && fPseudoEfficiency<1) fEfficiencies[i]->Scale(fPseudoEfficiency);
      if(!fEfficiencies[i]) {
        if(!i) AliFatal("Could not fetch efficiency!\n");
        printf("Could not find efficiency for V0M bin no. %i! Cloning the previous efficiency instead...\n",i);
        fEfficiencies[i] = (TH1D*)fEfficiencies[i-1]->Clone(Form("EffRescaled_Cent%i%s",i,fGFWSelection->GetSystPF()));
      };
    }
    fMPTList = new TList();
    fMPTList->SetOwner(kTRUE);
    fMPTListMC = new TList();
    fMPTListMC->SetOwner(kTRUE);
    fmPT = new TProfile*[8];
    for(Int_t i=0;i<4;i++) {
      fmPT[i] = new TProfile(Form("MeanPt_%s%s",spNames[i].Data(),fGFWSelection->GetSystPF()),Form("MeanPt_%s",spNames[i].Data()),fNMultiBins,fMultiBins);
      fMPTList->Add(fmPT[i]);
      fmPT[4+i] = new TProfile(Form("MeanPt_%s%s_Gen",spNames[i].Data(),fGFWSelection->GetSystPF()),Form("MeanPt_%s_Gen",spNames[i].Data()),fNMultiBins,fMultiBins);
      fMPTListMC->Add(fmPT[4+i]);

    }
    fMultiDist = new TH1D("MultiDistribution","Multiplicity distribution; #it{N}_{ch}; N(events)",fNMultiBins,fMultiBins);
    fV0MMulti = new TH1D("V0M_Multi","V0M_Multi",l_NV0MBinsDefault,l_V0MBinsDefault);
    fMPTList->Add(fMultiDist);
    fMPTList->Add(fV0MMulti);
    fMultiVsV0MCorr = new TH2D*[2];
    fMultiVsV0MCorr[0] = new TH2D("MultVsV0M_BeforeConsistency","MultVsV0M_BeforeConsistency",103,0,103,fNMultiBins,fMultiBins[0],fMultiBins[fNMultiBins]);
    fMultiVsV0MCorr[1] = new TH2D("MultVsV0M_AfterConsistency","MultVsV0M_AfterConsistency",103,0,103,fNMultiBins,fMultiBins[0],fMultiBins[fNMultiBins]);
    fMPTList->Add(fMultiVsV0MCorr[0]);
    fMPTList->Add(fMultiVsV0MCorr[1]);
    fNchTrueVsReco = new TH2D("NchTrueVsReco",";Nch (MC-true); Nch (MC-reco)",fNMultiBins,fMultiBins,fNMultiBins,fMultiBins);
    fMPTListMC->Add(fNchTrueVsReco);
    PostData(1,fMPTList);
    PostData(2,fMPTListMC);
  }
  if(fStageSwitch==9) {
    fRndm = new TRandom(0);
    fRequireReloadOnRunChange = kFALSE;
    if(!fIsMC) { //Efficiencies and NUA are only for the data
      fEfficiencyList = (TList*)GetInputData(1);
      fEfficiencies = new TH1D*[l_NV0MBinsDefault];
      for(Int_t i=0;i<l_NV0MBinsDefault;i++) {
        fEfficiencies[i] = (TH1D*)fEfficiencyList->FindObject(Form("EffRescaled_Cent%i%s",i,fGFWSelection->GetSystPF()));
        if(fEfficiencies[i] && fPseudoEfficiency<1) fEfficiencies[i]->Scale(fPseudoEfficiency);
        if(!fEfficiencies[i]) {
          if(!i) AliFatal("Could not fetch efficiency!\n");
          printf("Could not find efficiency for V0M bin no. %i! Cloning the previous efficiency instead...\n",i);
          fEfficiencies[i] = (TH1D*)fEfficiencies[i-1]->Clone(Form("EffRescaled_Cent%i%s",i,fGFWSelection->GetSystPF()));
        };
      }
      fWeightList = (TList*)GetInputData(2);
      fWeights = new AliGFWWeights*[1];
    };
    // if(!LoadMyWeights(0)) return; //Loading run-avg NUA weights
    fptVarList = new TList();
    fptVarList->SetOwner(kTRUE);
    for(Int_t i=0;i<1;i++) {
      fCkCont = new AliCkContainer(Form("ckcont_%s",spNames[i].Data()),Form("ckcont_%s",spNames[i].Data()),fNMultiBins,fMultiBins);
      fptVarList->Add(fCkCont);
      if(fNBootstrapProfiles) fCkCont->InitializeSubsamples(fNBootstrapProfiles);
    }
    fMultiDist = new TH1D("MultiDistribution","Multiplicity distribution; #it{N}_{ch}; N(events)",fNMultiBins,fMultiBins);
    fV0MMulti = new TH1D("V0M_Multi","V0M_Multi",l_NV0MBinsDefault,l_V0MBinsDefault);
    fptVarList->Add(fMultiDist);
    fptVarList->Add(fV0MMulti);
    fMultiVsV0MCorr = new TH2D*[2];
    fMultiVsV0MCorr[0] = new TH2D("MultVsV0M_BeforeConsistency","MultVsV0M_BeforeConsistency",103,0,103,fNMultiBins,fMultiBins[0],fMultiBins[fNMultiBins]);
    fMultiVsV0MCorr[1] = new TH2D("MultVsV0M_AfterConsistency","MultVsV0M_AfterConsistency",103,0,103,fNMultiBins,fMultiBins[0],fMultiBins[fNMultiBins]);
    fMptClosure = new TProfile(Form("MeanPtClosure_%s%s",spNames[0].Data(),fGFWSelection->GetSystPF()),Form("MeanPtClosure_%s",spNames[0].Data()),fNMultiBins,fMultiBins);
    fptVarList->Add(fMultiVsV0MCorr[0]);
    fptVarList->Add(fMultiVsV0MCorr[1]);
    fptVarList->Add(fMptClosure);
    if(fIsMC) {
      fNchTrueVsReco = new TH2D("NchTrueVsReco",";Nch (MC-true); Nch (MC-reco)",fNMultiBins,fMultiBins,fNMultiBins,fMultiBins);
      fptVarList->Add(fNchTrueVsReco);
    }

    PostData(1,fptVarList);
    //Setting up the FlowContainer
    TObjArray *oba = new TObjArray();
    oba->Add(new TNamed("ChGap22","ChGap22")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("ChGap24","ChGap24")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("ChFull22","ChFull22")); //no-gap case
    oba->Add(new TNamed("ChFull24","ChFull24")); //no-gap case
//adding v3n
    oba->Add(new TNamed("ChGap32","ChGap32")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("ChGap34","ChGap34")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("ChFull32","ChFull32")); //no-gap case
    oba->Add(new TNamed("ChFull34","ChFull34")); //no-gap case

    oba->Add(new TNamed("ChGap42","ChGap42")); //gap case

    oba->Add(new TNamed("LM22","LM22")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("MR22","MR22")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("LR22","LR22")); //for gap (|eta|>0.4) case

    oba->Add(new TNamed("LLMR24","LLMR24")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("LMMR24","LMMR24")); //for gap (|eta|>0.4) case
    oba->Add(new TNamed("LMRR24","LMRR24")); //for gap (|eta|>0.4) case

    oba->Add(new TNamed("ChSC234","ChSC234")); //for SC{2,3}
    oba->Add(new TNamed("ChSC244","ChSC244")); //for SC{2,3}

    fFC = new AliGFWFlowContainer();
    TString fcname("FlowContainer");
    if(!fContSubfix->IsNull()) fcname.Append(fContSubfix->Data());
    fFC->SetName(fcname.Data());
    fFC->Initialize(oba,fNMultiBins,fMultiBins,fNBootstrapProfiles);
    delete oba;
    PostData(2,fFC);
    Int_t pows[] = {3,0,2,2,3,3,3}; //5th harm. sum = 3, b/c {-2 -3}
    Int_t powsFull[] = {5,0,4,4,3,3,3};
    fGFW = new AliGFW();
    fGFW->AddRegion("refN",7,pows,-0.8,-fEtaV2Sep,1,1);
    fGFW->AddRegion("refP",7,pows,fEtaV2Sep,0.8,1,1);
    if(fEtaV2Sep>=0)
      fGFW->AddRegion("subMid",7,pows,-fEtaV2Sep,fEtaV2Sep,1,1);
    fGFW->AddRegion("mid",7,powsFull,-0.8,0.8,1,2);
    CreateCorrConfigs();
    //Covariance
    fCovList = new TList();
    fCovList->SetOwner(kTRUE);
    fCovariance = new AliProfileBS*[6];
    for(Int_t i=0;i<1;i++) {
      fCovList->Add(new AliProfileBS(Form("covmpt_%s",spNames[i].Data()),Form("covmpt_%s",spNames[i].Data()),fNMultiBins,fMultiBins));
      fCovariance[i] = (AliProfileBS*)fCovList->At(i);
      fCovList->Add(new AliProfileBS(Form("covnopt_%s",spNames[i].Data()),Form("covnopt_%s",spNames[i].Data()),fNMultiBins,fMultiBins));
      fCovariance[i+1] = (AliProfileBS*)fCovList->At(i+1);
      fCovList->Add(new AliProfileBS(Form("covmpt_v3_%s",spNames[i].Data()),Form("covmpt_v3_%s",spNames[i].Data()),fNMultiBins,fMultiBins));
      fCovariance[i+2] = (AliProfileBS*)fCovList->At(i+2);
      fCovList->Add(new AliProfileBS(Form("covnopt_v3_%s",spNames[i].Data()),Form("covnopt_v3_%s",spNames[i].Data()),fNMultiBins,fMultiBins));
      fCovariance[i+3] = (AliProfileBS*)fCovList->At(i+3);
      fCovList->Add(new AliProfileBS(Form("covmpt_v23_%s",spNames[0].Data()),Form("covmpt_v23_%s",spNames[0].Data()),fNMultiBins,fMultiBins));
      fCovariance[i+4] = (AliProfileBS*)fCovList->At(4);
      fCovList->Add(new AliProfileBS(Form("covnopt_v23_%s",spNames[0].Data()),Form("covnopt_v23_%s",spNames[0].Data()),fNMultiBins,fMultiBins));
      fCovariance[i+5] = (AliProfileBS*)fCovList->At(5);

    };
    if(fNBootstrapProfiles) for(Int_t i=0;i<6;i++) fCovariance[i]->InitializeSubsamples(fNBootstrapProfiles);
    PostData(3,fCovList);
  }


  fEventCuts.OverrideAutomaticTriggerSelection(fTriggerType,true);
  if(fExtendV0MAcceptance) {
    fEventCuts.OverrideCentralityFramework(1);
    fEventCuts.SetCentralityEstimators("V0M","CL0");
    fEventCuts.SetCentralityRange(0.f,101.f);
  }
  fGFWNtotSelection = new AliGFWCuts();
  fGFWNtotSelection->SetupCuts(0);
  fGFWNtotSelection->SetEta(fEtaNch);
  AliAnalysisManager *man=AliAnalysisManager::GetAnalysisManager();
  AliInputEventHandler* inputHandler = (AliInputEventHandler*) (man->GetInputEventHandler());
  fPIDResponse = inputHandler->GetPIDResponse();
  fBayesPID = new AliPIDCombined();
  fBayesPID->SetDefaultTPCPriors();
  fBayesPID->SetSelectedSpecies(AliPID::kSPECIES);
  fBayesPID->SetDetectorMask(AliPIDResponse::kDetTPC+AliPIDResponse::kDetTOF);
  LoadWeightAndMPT();
};
void AliAnalysisTaskMeanPtV2Corr::UserExec(Option_t*) {
  AliAODEvent *fAOD = dynamic_cast<AliAODEvent*>(InputEvent());
  if(!fAOD) return;
  if(fIsMC) {
    fMCEvent = dynamic_cast<AliMCEvent *>(MCEvent());
    if (!fMCEvent) return;
  }
  AliMultSelection *lMultSel = (AliMultSelection*)fInputEvent->FindListObject("MultSelection");
  Double_t l_Cent = lMultSel->GetMultiplicityPercentile(fCentEst->Data());
  if(!fBypassTriggerAndEvetCuts)
    if(!CheckTrigger(l_Cent)) return;
  Double_t vtxXYZ[] = {0.,0.,0.};
  if(!AcceptAOD(fAOD, vtxXYZ)) return;
  Double_t vz = fAOD->GetPrimaryVertex()->GetZ();
  if(!fGFWSelection->AcceptVertex(fAOD)) return;
  if(fStageSwitch==1)
    fIsMC?FillWeightsMC(fAOD, vz,l_Cent,vtxXYZ):FillWeights(fAOD, vz,l_Cent,vtxXYZ);
  if(fStageSwitch==2)
    fIsMC?FillMeanPtMC(fAOD,vz,l_Cent,vtxXYZ):FillMeanPt(fAOD, vz, l_Cent,vtxXYZ);
  if(fStageSwitch==3)
    FillCK(fAOD,vz,l_Cent,vtxXYZ);
  if(fStageSwitch==7)
    ProduceEfficiencies(fAOD,vz,l_Cent,vtxXYZ);
  if(fStageSwitch==8)
    MCClosure_MeanPt(fAOD,vz,l_Cent,vtxXYZ);
  if(fStageSwitch==9)
    CovSkipMpt(fAOD,vz,l_Cent,vtxXYZ);
};
void AliAnalysisTaskMeanPtV2Corr::NotifyRun() {
  AliAODEvent *fAOD = dynamic_cast<AliAODEvent*>(InputEvent());
  //Reinitialize AliEventCuts (done automatically on check):
  Bool_t dummy = fEventCuts.AcceptEvent(fAOD);
  //Then override PU cut if required:
  if(fGFWSelection->GetSystFlagIndex()==15)
    fEventCuts.fESDvsTPConlyLinearCut[0] = 1500.;
}
void AliAnalysisTaskMeanPtV2Corr::Terminate(Option_t*) {
};
Bool_t AliAnalysisTaskMeanPtV2Corr::CheckTrigger(Double_t lCent) {
  UInt_t fSelMask = ((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected();
  if(!(fTriggerType&fSelMask)) return kFALSE;
  if((fSelMask&fTriggerType&AliVEvent::kCentral) && lCent>10) return kFALSE;
  if((fSelMask&fTriggerType&AliVEvent::kSemiCentral) && (lCent<30 || lCent>50)) return kFALSE;
  return kTRUE;
};
Bool_t AliAnalysisTaskMeanPtV2Corr::AcceptAOD(AliAODEvent *inEv, Double_t *lvtxXYZ) {
  if(!fBypassTriggerAndEvetCuts) if(!fEventCuts.AcceptEvent(inEv)) return 0;
  const AliAODVertex* vtx = dynamic_cast<const AliAODVertex*>(inEv->GetPrimaryVertex());
  if(!vtx || vtx->GetNContributors() < 1)
    return kFALSE;
  const AliAODVertex* vtxSPD = dynamic_cast<const AliAODVertex*>(inEv->GetPrimaryVertexSPD());
  Double_t dMaxResol = 0.25; // suggested from DPG
  Double_t cov[6] = {0};
  vtxSPD->GetCovarianceMatrix(cov);
  Double_t zRes = TMath::Sqrt(cov[5]);
  if ( vtxSPD->IsFromVertexerZ() && (zRes > dMaxResol)) return kFALSE;
  const Double_t aodVtxZ = vtx->GetZ();
  if(TMath::Abs(aodVtxZ) > 10)
    return kFALSE;
  vtx->GetXYZ(lvtxXYZ);
  return kTRUE;
};
Bool_t AliAnalysisTaskMeanPtV2Corr::AcceptAODTrack(AliAODTrack *mtr, Double_t *ltrackXYZ, const Double_t &ptMin, const Double_t &ptMax, Double_t *vtxp) {
  if(mtr->Pt()<ptMin) return kFALSE;
  if(mtr->Pt()>ptMax) return kFALSE;
  if(ltrackXYZ && vtxp) {
    mtr->GetXYZ(ltrackXYZ);
    ltrackXYZ[0] = ltrackXYZ[0]-vtxp[0];
    ltrackXYZ[1] = ltrackXYZ[1]-vtxp[1];
    ltrackXYZ[2] = ltrackXYZ[2]-vtxp[2];
  } else return kFALSE; //DCA cut is a must for now
  return fGFWSelection->AcceptTrack(mtr,ltrackXYZ,0,kFALSE);
};
Bool_t AliAnalysisTaskMeanPtV2Corr::AcceptAODTrack(AliAODTrack *mtr, Double_t *ltrackXYZ, const Double_t &ptMin, const Double_t &ptMax, Double_t *vtxp, Int_t &nTot) {
  if(mtr->Pt()<ptMin) return kFALSE;
  if(mtr->Pt()>ptMax) return kFALSE;
  if(ltrackXYZ && vtxp) {
    mtr->GetXYZ(ltrackXYZ);
    ltrackXYZ[0] = ltrackXYZ[0]-vtxp[0];
    ltrackXYZ[1] = ltrackXYZ[1]-vtxp[1];
    ltrackXYZ[2] = ltrackXYZ[2]-vtxp[2];
  } else return kFALSE; //DCA cut is a must for now
  if(fGFWNtotSelection->AcceptTrack(mtr,ltrackXYZ,0,kFALSE)) nTot++;
  return fGFWSelection->AcceptTrack(mtr,ltrackXYZ,0,kFALSE);
};
Bool_t AliAnalysisTaskMeanPtV2Corr::AcceptParticle(AliVParticle *mpa) {
  if(!mpa->IsPhysicalPrimary()) return kFALSE;
  if(mpa->Charge()==0) return kFALSE;
  if(TMath::Abs(mpa->Eta())>fEta) return kFALSE;
  // if(mpa->Pt()<0.5) return kFALSE;
  // if(mpa->Pt()>2) return kFALSE;
  return kTRUE;
};
Int_t AliAnalysisTaskMeanPtV2Corr::GetStageSwitch(TString instr) {
  if(instr.Contains("weights")) return 1;
  if(instr.Contains("meanpt")) return 2;
  if(instr.Contains("full")) return 3;
  if(instr.Contains("Efficiency")) return 7;
  if(instr.Contains("MC_MptClosure")) return 8;
  if(instr.Contains("CovSkipMpt")) return 9;
  return 0;
}
void AliAnalysisTaskMeanPtV2Corr::FillWeightsMC(AliAODEvent *fAOD, const Double_t &vz, const Double_t &l_Cent, Double_t *vtxp) {
  //MC generated
  AliAODTrack *lTrack;
  // AliVParticle *lPart;
  Double_t trackXYZ[3];
  Double_t dummyDouble[] = {0.,0.};
  Double_t ptMin = fPtBins[0];
  Double_t ptMax = fPtBins[fNPtBins];
  TClonesArray *tca = (TClonesArray*)fInputEvent->FindListObject("mcparticles");
  Int_t nPrim = tca->GetEntries();
  AliVParticle *lPart;
  Int_t partNotFetched=0;
  for (Int_t ipart = 0; ipart < nPrim; ipart++) {
    lPart = (AliAODMCParticle*)tca->At(ipart);
    if (!lPart) { continue; };
    /* get particlePDG */
    Int_t pdgcode = TMath::Abs(lPart->PdgCode());
    if (!lPart->IsPhysicalPrimary()) continue;
    if (lPart->Charge()==0.) continue;
    if (TMath::Abs(lPart->Eta()) > fEta) continue;
    Double_t pt = lPart->Pt();
    if (pt<ptMin || pt>ptMax) continue;
    fWeights[0]->Fill(lPart->Phi(),lPart->Eta(),vz,lPart->Pt(),l_Cent,2);
    if(fDisablePID) continue;
    if(pdgcode==211) fWeights[1]->Fill(lPart->Phi(),lPart->Eta(),vz,lPart->Pt(),l_Cent,2);
    if(pdgcode==321) fWeights[2]->Fill(lPart->Phi(),lPart->Eta(),vz,lPart->Pt(),l_Cent,2);
    if(pdgcode==2212) fWeights[3]->Fill(lPart->Phi(),lPart->Eta(),vz,lPart->Pt(),l_Cent,2);
  };

  //MC reconstructed
  for(Int_t lTr=0;lTr<fAOD->GetNumberOfTracks();lTr++) {
    lTrack = (AliAODTrack*)fAOD->GetTrack(lTr);
    if(!AcceptAODTrack(lTrack,trackXYZ,ptMin,ptMax,vtxp)) continue;
    lPart = (AliAODMCParticle*)tca->At(TMath::Abs(lTrack->GetLabel()));
    if(!lPart) continue;
    if(!lPart->IsPhysicalPrimary()) continue;
    if(TMath::Abs(lTrack->Eta())>fEta) continue;
    if(!fGFWSelection->AcceptTrack(lTrack,dummyDouble)) continue;
    fWeights[0]->Fill(lPart->Phi(),lPart->Eta(),vz,lPart->Pt(),l_Cent,1);
    if(fDisablePID) continue;
    Int_t PIDIndex = GetBayesPIDIndex(lTrack)+1;
    if(PIDIndex) fWeights[PIDIndex]->Fill(lPart->Phi(),lPart->Eta(),vz,lPart->Pt(),l_Cent,1);
  };
  PostData(1,fWeightList);
}
void AliAnalysisTaskMeanPtV2Corr::FillWeights(AliAODEvent *fAOD, const Double_t &vz, const Double_t &l_Cent, Double_t *vtxp) {
  AliAODTrack *lTrack;
  Double_t trackXYZ[3];
  Double_t ptMin = fPtBins[0];
  Double_t ptMax = fPtBins[fNPtBins];
  for(Int_t lTr=0;lTr<fAOD->GetNumberOfTracks();lTr++) {
    lTrack = (AliAODTrack*)fAOD->GetTrack(lTr);
    if(!lTrack) continue;
    Double_t trackXYZ[] = {0.,0.,0.};
    if(!AcceptAODTrack(lTrack,trackXYZ,ptMin,ptMax,vtxp)) continue;
    Double_t leta = lTrack->Eta();
    Double_t lpt = lTrack->Pt();
    ((AliGFWWeights*)fWeightList->At(0))->Fill(lTrack->Phi(),lTrack->Eta(),vz,lTrack->Pt(),l_Cent,0);
    if(fDisablePID) continue;
    Int_t PIDIndex = GetBayesPIDIndex(lTrack)+1;
    if(PIDIndex) ((AliGFWWeights*)fWeightList->At(PIDIndex))->Fill(lTrack->Phi(),lTrack->Eta(),vz,lTrack->Pt(),l_Cent,0);

  };
  PostData(1,fWeightList);
}

void AliAnalysisTaskMeanPtV2Corr::FillMeanPtCounter(Double_t pt, Double_t &l_sum, Double_t &l_count, AliGFWWeights *inWeight) {
  Double_t w = inWeight?inWeight->GetIntegratedEfficiency(pt):1;
  if(w==0) return;
  l_sum+=pt/w;
  l_count+=1./w;
}
void AliAnalysisTaskMeanPtV2Corr::FillMeanPtCounterWW(const Double_t &pt, Double_t &l_sum, Double_t &l_count, const Double_t &w) {
  if(w==0) return;
  l_sum+=pt/w;
  l_count+=1./w;
}
Int_t AliAnalysisTaskMeanPtV2Corr::GetNtotTracks(AliAODEvent* lAOD, const Double_t &ptmin, const Double_t &ptmax, Double_t *vtxp) {
  Double_t ltrackXYZ[3];
  AliAODTrack *lTrack;
  Int_t nTotNoTracks=0;
  for(Int_t lTr=0;lTr<lAOD->GetNumberOfTracks();lTr++) {
    lTrack = (AliAODTrack*)lAOD->GetTrack(lTr);
    if(!lTrack) continue;
    if(!AcceptAODTrack(lTrack,ltrackXYZ,ptmin,ptmax,vtxp,nTotNoTracks)) continue;
  };
  return nTotNoTracks;
}
void AliAnalysisTaskMeanPtV2Corr::FillMeanPt(AliAODEvent *fAOD, const Double_t &vz, const Double_t &l_Cent, Double_t *vtxp) {
  // LoadWeightAndMPT(fAOD);
  AliAODTrack *lTrack;
  Double_t l_ptsum[]={0,0,0,0};
  Double_t l_ptCount[]={0,0,0,0};
  Double_t trackXYZ[3];
  Int_t iCent = fV0MMulti->FindBin(l_Cent);
  Int_t lPosCount=0, lNegCount=0;
  if(!iCent || iCent>fV0MMulti->GetNbinsX()) return;
  Double_t ptMin = fPtBins[0];
  Double_t ptMax = fPtBins[fNPtBins];
  Int_t nTotNoTracks=GetNtotTracks(fAOD,ptMin,ptMax,vtxp);
  Bool_t usingPseudoEff = (fPseudoEfficiency<1);
  for(Int_t lTr=0;lTr<fAOD->GetNumberOfTracks();lTr++) {
    if(usingPseudoEff) if(fRndm->Uniform()>fPseudoEfficiency) continue;
    lTrack = (AliAODTrack*)fAOD->GetTrack(lTr);
    if(!lTrack) continue;
    Double_t trackXYZ[] = {0.,0.,0.};
    if(!AcceptAODTrack(lTrack,trackXYZ,ptMin,ptMax,vtxp)) continue;
    Double_t leta = lTrack->Eta();
    // if(TMath::Abs(leta)<fEtaNch) nTotNoTracks+=1; //Nch calculated in EtaNch region
    if(leta<-fEtaV2Sep) lNegCount++; else if(leta>fEtaV2Sep) lPosCount++;
    if(TMath::Abs(leta)>fEta) continue; //<pt> calculated in fEta region
    // if(TMath::Abs(lTrack->Eta())<0.8 && lTrack->Pt()>0.2 && lTrack->Pt()<3)  nTotNoTracks++;
    Double_t lpt = lTrack->Pt();
    Double_t l_weight = fEfficiencies[iCent-1]->GetBinContent(fEfficiencies[iCent-1]->FindBin(lpt));
    FillMeanPtCounterWW(lpt,l_ptsum[0],l_ptCount[0],l_weight);
    if(fDisablePID) continue;
    Int_t PIDIndex = GetBayesPIDIndex(lTrack)+1;
    if(PIDIndex) FillMeanPtCounter(lpt,l_ptsum[PIDIndex],l_ptCount[PIDIndex],fWeights[PIDIndex]);
  };
  if(l_ptCount[0]==0) return;
  fMultiVsV0MCorr[0]->Fill(l_Cent,nTotNoTracks);
  if(fConsistencyFlag&1) if(!lPosCount || !lNegCount) return; // only events where v2{2, gap} could be calculated
  if(fConsistencyFlag&2) if(nTotNoTracks<4) return; //only events where v2{4} can be calculated (assuming same region as nch)
  if(fConsistencyFlag&4) if(lPosCount<2 || lNegCount<2) return; //Only events where v2{4, gap} can be calculated
  fMultiVsV0MCorr[1]->Fill(l_Cent,nTotNoTracks);
  Double_t lMulti  = fUseNch?(1.0*nTotNoTracks):l_Cent; //Whatever the multiplicity is
  for(Int_t i=0;i<4;i++) {
    if(!l_ptCount[i]) continue;
    Double_t fillWeight = fUseWeightsOne?1:l_ptCount[i];
    fmPT[i]->Fill(lMulti,l_ptsum[i]/l_ptCount[i],fillWeight);
  }
  fMultiDist->Fill(lMulti);
  fV0MMulti->Fill(l_Cent);
  PostData(1,fMPTList);
};
void AliAnalysisTaskMeanPtV2Corr::FillMeanPtMC(AliAODEvent *fAOD, const Double_t &vz, const Double_t &l_Cent, Double_t *vtxp) {
  Double_t l_ptsum[]={0,0,0,0};
  Double_t l_ptCount[]={0,0,0,0};
  Double_t trackXYZ[3];
  Int_t nTotNoTracksMC=0;
  Int_t nTotNoTracksReco=0;
  Int_t iCent = fV0MMulti->FindBin(l_Cent);
  if(!iCent || iCent>fV0MMulti->GetNbinsX()) return;
  if(fUseRecoNchForMC) nTotNoTracksReco = GetNtotTracks(fAOD,0.2,3.,vtxp);
  Int_t lPosCount=0, lNegCount=0;
  TClonesArray *tca = (TClonesArray*)fInputEvent->FindListObject("mcparticles");
  Int_t nPrim = tca->GetEntries();
  AliAODMCParticle *lPart;
  for(Int_t ipart = 0; ipart < nPrim; ipart++) {
    lPart = (AliAODMCParticle*)tca->At(ipart);
    if (!lPart->IsPhysicalPrimary()) continue;
    if (lPart->Charge()==0.) continue;
    //Hardcoded cuts to inhereted from AcceptAODTrack
    Double_t leta = lPart->Eta();
    if (TMath::Abs(leta) > 0.8) continue;
    Double_t pt = lPart->Pt();
    if (pt<0.2 || pt>3.) continue;
    if(TMath::Abs(leta)<fEtaNch) nTotNoTracksMC++; //Nch calculated in EtaNch region
    if(leta<-fEtaV2Sep) lNegCount++; else if(leta>fEtaV2Sep) lPosCount++;
    if(TMath::Abs(leta)>fEta) continue; //<pt> calculated in fEta region
    Double_t lpt = lPart->Pt();
    FillMeanPtCounterWW(lpt,l_ptsum[0],l_ptCount[0],1); //MC truth, so weight = 1
  };
  Int_t nTotNoTracks = fUseRecoNchForMC?nTotNoTracksReco:nTotNoTracksMC;
  if(l_ptCount[0]==0) return;
  if(fConsistencyFlag&1) if(!lPosCount || !lNegCount) return; // only events where v2{2, gap} could be calculated
  if(fConsistencyFlag&2) if(nTotNoTracks<4) return; //only events where v2{4} can be calculated (assuming same region as nch)
  if(fConsistencyFlag&4) if(lPosCount<2 || lNegCount<2) return; //Only events where v2{4, gap} can be calculated
  Double_t lMulti  = fUseNch?(1.0*nTotNoTracks):l_Cent; //Whatever the multiplicity is
  for(Int_t i=0;i<1;i++) { //No PID = index is only 1
    if(!l_ptCount[i]) continue;
    Double_t fillWeight = fUseWeightsOne?1:l_ptCount[i];
    fmPT[i]->Fill(lMulti,l_ptsum[i]/l_ptCount[i],fillWeight);
  }
  fMultiDist->Fill(lMulti);
  fV0MMulti->Fill(l_Cent);
  if(fUseRecoNchForMC) fNchTrueVsReco->Fill(nTotNoTracksMC,nTotNoTracksReco);
  PostData(1,fMPTList);
};
void AliAnalysisTaskMeanPtV2Corr::MCClosure_MeanPt(AliAODEvent *fAOD, const Double_t &vz, const Double_t &l_Cent, Double_t *vtxp) {
  // LoadWeightAndMPT(fAOD);
  AliAODTrack *lTrack;
  Double_t l_ptsum[]={0,0,0,0};
  Double_t l_ptCount[]={0,0,0,0};
  Double_t trackXYZ[3];
  Int_t iCent = fV0MMulti->FindBin(l_Cent);
  Int_t lPosCount=0, lNegCount=0;
  if(!iCent || iCent>fV0MMulti->GetNbinsX()) return;
  Double_t ptMin = fPtBins[0];
  Double_t ptMax = fPtBins[fNPtBins];
  Int_t nTotNoTracks=GetNtotTracks(fAOD,ptMin,ptMax,vtxp);
  TClonesArray *tca = (TClonesArray*)fInputEvent->FindListObject("mcparticles");
  AliAODMCParticle *lPart;
  Bool_t usingPseudoEff = (fPseudoEfficiency<1);
  Bool_t ProceedWithoutFilling=kFALSE;
  for(Int_t lTr=0;lTr<fAOD->GetNumberOfTracks();lTr++) {
    if(usingPseudoEff) if(fRndm->Uniform()>fPseudoEfficiency) continue;
    lTrack = (AliAODTrack*)fAOD->GetTrack(lTr);
    if(!lTrack) continue;
    Double_t trackXYZ[] = {0.,0.,0.};
    if(!AcceptAODTrack(lTrack,trackXYZ,ptMin,ptMax,vtxp)) continue;
    Int_t fLabel = lTrack->GetLabel();
    Int_t index = TMath::Abs(fLabel);
    if (index < 0) continue;
    lPart = (AliAODMCParticle*)tca->At(index);
    if(!lPart->IsPhysicalPrimary()) continue;
    Double_t leta = lTrack->Eta();
    if(leta<-fEtaV2Sep) lNegCount++; else if(leta>fEtaV2Sep) lPosCount++;
    if(TMath::Abs(leta)>fEta) continue; //<pt> calculated in fEta region
    Double_t lpt = lTrack->Pt(); //Efficiency done on reco tracks, so here also should use reco tracks
    Double_t l_weight = fEfficiencies[iCent-1]->GetBinContent(fEfficiencies[iCent-1]->FindBin(lpt));
    FillMeanPtCounterWW(lpt,l_ptsum[0],l_ptCount[0],l_weight);
    if(fDisablePID) continue;
    Int_t PIDIndex = GetBayesPIDIndex(lTrack)+1;
    if(PIDIndex) FillMeanPtCounter(lpt,l_ptsum[PIDIndex],l_ptCount[PIDIndex],fWeights[PIDIndex]);
  };
  if(l_ptCount[0]==0) ProceedWithoutFilling=kTRUE;
  fMultiVsV0MCorr[0]->Fill(l_Cent,nTotNoTracks);
  if(fConsistencyFlag&1) if(!lPosCount || !lNegCount) ProceedWithoutFilling=kTRUE; // only events where v2{2, gap} could be calculated
  if(fConsistencyFlag&2) if(nTotNoTracks<4) ProceedWithoutFilling=kTRUE; //only events where v2{4} can be calculated (assuming same region as nch)
  if(fConsistencyFlag&4) if(lPosCount<2 || lNegCount<2) ProceedWithoutFilling=kTRUE; //Only events where v2{4, gap} can be calculated
  if(!ProceedWithoutFilling) {
    fMultiVsV0MCorr[1]->Fill(l_Cent,nTotNoTracks);
    Double_t lMulti  = fUseNch?(1.0*nTotNoTracks):l_Cent; //Whatever the multiplicity is
    for(Int_t i=0;i<4;i++) {
      if(!l_ptCount[i]) continue;
      Double_t fillWeight = fUseWeightsOne?1:l_ptCount[i];
      fmPT[i]->Fill(lMulti,l_ptsum[i]/l_ptCount[i],fillWeight);
    }
    fMultiDist->Fill(lMulti);
    fV0MMulti->Fill(l_Cent);
    PostData(1,fMPTList);
  };
  //Reset all the variables:
  for(Int_t i=0;i<4;i++) {l_ptsum[i]=0;  l_ptCount[i]=0; };
  lPosCount=0, lNegCount=0;
  Int_t nTotNoTracksMC=0;
  Int_t nTotNoTracksReco=nTotNoTracks;
  Int_t nPrim = tca->GetEntries();
  for(Int_t ipart = 0; ipart < nPrim; ipart++) {
    lPart = (AliAODMCParticle*)tca->At(ipart);
    if (!lPart->IsPhysicalPrimary()) continue;
    if (lPart->Charge()==0.) continue;
    Int_t pdgcode = TMath::Abs(lPart->GetPdgCode());
    Int_t pidInd = GetPIDIndex(pdgcode);
    if(pidInd==4 || pidInd==5 || pidInd==6) continue; //reject Sigma, Xi, and omega
    //Hardcoded cuts to inhereted from AcceptAODTrack
    Double_t leta = lPart->Eta();
    if (TMath::Abs(leta) > 0.8) continue;
    Double_t pt = lPart->Pt();
    if (pt<0.2 || pt>3.) continue;
    if(TMath::Abs(leta)<fEtaNch) nTotNoTracksMC++; //Nch calculated in EtaNch region
    if(leta<-fEtaV2Sep) lNegCount++; else if(leta>fEtaV2Sep) lPosCount++;
    if(TMath::Abs(leta)>fEta) continue; //<pt> calculated in fEta region
    Double_t lpt = lPart->Pt();
    FillMeanPtCounterWW(lpt,l_ptsum[0],l_ptCount[0],1); //MC truth, so weight = 1
  };
  nTotNoTracks = fUseRecoNchForMC?nTotNoTracksReco:nTotNoTracksMC;
  ProceedWithoutFilling=kFALSE;
  if(l_ptCount[0]==0) ProceedWithoutFilling=kTRUE;
  if(fConsistencyFlag&1) if(!lPosCount || !lNegCount) ProceedWithoutFilling=kTRUE; // only events where v2{2, gap} could be calculated
  if(fConsistencyFlag&2) if(nTotNoTracks<4) ProceedWithoutFilling=kTRUE; //only events where v2{4} can be calculated (assuming same region as nch)
  if(fConsistencyFlag&4) if(lPosCount<2 || lNegCount<2) ProceedWithoutFilling=kTRUE; //Only events where v2{4, gap} can be calculated
  if(!ProceedWithoutFilling) {
    Double_t lMulti  = fUseNch?(1.0*nTotNoTracks):l_Cent; //Whatever the multiplicity is
    for(Int_t i=0;i<1;i++) { //No PID = index is only 1
      if(!l_ptCount[i]) continue;
      Double_t fillWeight = fUseWeightsOne?1:l_ptCount[i];
      fmPT[4+i]->Fill(lMulti,l_ptsum[i]/l_ptCount[i],fillWeight);
    }
    fMultiDist->Fill(lMulti);
    fV0MMulti->Fill(l_Cent);
    if(fUseRecoNchForMC) fNchTrueVsReco->Fill(nTotNoTracksMC,nTotNoTracksReco);
    PostData(2,fMPTListMC);
  };
};

void AliAnalysisTaskMeanPtV2Corr::FillWPCounter(Double_t inArr[5], Double_t w, Double_t p) {
  inArr[0] += w;       // = w1p0
  inArr[1] += w*p;     // = w1p1
  inArr[2] += w*w*p*p; // = w2p2
  inArr[3] += w*w*p;   // = w2p1
  inArr[4] += w*w;     // = w2p0
}
void AliAnalysisTaskMeanPtV2Corr::CalculateMptValues(Double_t outArr[4], Double_t inArr[5]) {
  //Input:
  //inArr[0] = w1p0
  //inArr[1] = w1p1
  //inArr[2] = w2p2
  //inArr[3] = w2p1
  //inArr[4] = w2p0
  //outArr[0] = <pT> (avg. over all events), has to be preset when calling the function
  //Output:
  //outArr[1] = variance
  //outAtt[2] = norm
  //outArr[3] = [pT] in this event (M(pt))
  //Assuming outArr[0] is preset to meanPt; outArr[1] = variance; outArr[2] = norm; outArr[3] = mpt in this event
  outArr[1] = TMath::Power(inArr[1] - outArr[0]*inArr[0], 2) //(w1p1 - l_meanPt*w1p0) * (w1p1 - l_meanPt*w1p0)
              - inArr[2] + 2*outArr[0]*inArr[3] - outArr[0]*outArr[0]*inArr[4]; //- w2p2 + 2*l_meanPt*w2p1 - l_meanPt*l_meanPt*w2p0;
  outArr[2] = inArr[0]*inArr[0] - inArr[4];
  outArr[3] = inArr[1]/inArr[0];
}

void AliAnalysisTaskMeanPtV2Corr::FillCK(AliAODEvent *fAOD, const Double_t &vz, const Double_t &l_Cent, Double_t *vtxp) {
  AliAODTrack *lTrack;
  Double_t wp[4][5] = {{0,0,0,0,0}, {0,0,0,0,0},
                       {0,0,0,0,0}, {0,0,0,0,0}}; //Initial values, [species][w*p]
  Double_t outVals[4][4] = {{0,0,0,0}, {0,0,0,0},
                            {0,0,0,0}, {0,0,0,0}};
  Double_t trackXYZ[3];
  fGFW->Clear();
  Double_t ptmins[] = {0.2,0.2,0.3,0.5};
  Double_t ptmaxs[] = {10.,10.,6.0,6.0};
  Int_t iCent = fV0MMulti->FindBin(l_Cent);
  if(!iCent || iCent>fV0MMulti->GetNbinsX()) return;
  iCent--;
  Int_t lPosCount=0, lNegCount=0, lMidCount=0;
  Double_t ptMin = fPtBins[0];
  Double_t ptMax = fPtBins[fNPtBins];
  Int_t nTotNoTracks=0;
  if(fIsMC) {
    Int_t nTotNoTracksMC=0;
    Int_t nTotNoTracksReco=0;
    if(fUseRecoNchForMC) nTotNoTracksReco = GetNtotTracks(fAOD,ptMin,ptMax,vtxp);
    TClonesArray *tca = (TClonesArray*)fInputEvent->FindListObject("mcparticles");
    Int_t nPrim = tca->GetEntries();
    AliAODMCParticle *lPart;
    for(Int_t ipart = 0; ipart < nPrim; ipart++) {
      lPart = (AliAODMCParticle*)tca->At(ipart);
      if (!lPart->IsPhysicalPrimary()) continue;
      if (lPart->Charge()==0.) continue;
      //Hardcoded cuts to inhereted from AcceptAODTrack
      Double_t leta = lPart->Eta();
      if (TMath::Abs(leta) > 0.8) continue;
      Double_t pt = lPart->Pt();
      if (pt<0.2 || pt>3.) continue;
      if(leta<-fEtaV2Sep) lNegCount++;
      if(leta>fEtaV2Sep) lPosCount++;
      if(TMath::Abs(leta)<fEtaNch) nTotNoTracksMC++; //Nch calculated in EtaNch region
      Double_t lpt = lPart->Pt();
      if(TMath::Abs(leta)<fEta)  { //for mean pt, only consider -0.4-0.4 region
        FillWPCounter(wp[0],1,lpt); //weight = 1, naturally
      }  //Actually, no need for if() statememnt now since GFW knows about eta's, so I can fill it all the time
      fGFW->Fill(leta,1,lPart->Phi(),1,3); //filling both gap (bit mask 1) and full (bit mas 2). Since this is MC, weight is 1.
      // FillMeanPtCounterWW(lpt,l_ptsum[0],l_ptCount[0],1); //MC truth, so weight = 1
    };
    nTotNoTracks = fUseRecoNchForMC?nTotNoTracksReco:nTotNoTracksMC;
    if(fUseRecoNchForMC) fNchTrueVsReco->Fill(nTotNoTracksMC,nTotNoTracksReco);
  } else {
    if(!LoadMyWeights(fAOD->GetRunNumber())) return; //Only load wieghts for data
    Bool_t usingPseudoEff = (fPseudoEfficiency<1);
    nTotNoTracks=GetNtotTracks(fAOD,ptMin,ptMax,vtxp);
    for(Int_t lTr=0;lTr<fAOD->GetNumberOfTracks();lTr++) {
      if(usingPseudoEff) if(fRndm->Uniform()>fPseudoEfficiency) continue;
      lTrack = (AliAODTrack*)fAOD->GetTrack(lTr);
      if(!lTrack) continue;
      Double_t leta = lTrack->Eta();
      Double_t trackXYZ[] = {0.,0.,0.};
      if(!AcceptAODTrack(lTrack,trackXYZ,ptMin,ptMax,vtxp)) continue;
      // if(TMath::Abs(leta)<fEtaNch) nTotNoTracks+=1;
      if(leta<-fEtaV2Sep) lNegCount++;
      if(leta>fEtaV2Sep) lPosCount++;
      if(fEtaV2Sep>0 && TMath::Abs(leta)<fEtaV2Sep) lMidCount++;
      Double_t p1 = lTrack->Pt();
      Double_t weff = fEfficiencies[iCent]->GetBinContent(fEfficiencies[iCent]->FindBin(p1));
      if(weff==0) continue;
      Double_t wacc = fWeights[0]->GetNUA(lTrack->Phi(),lTrack->Eta(),vz);
      weff = 1./weff;
      if(TMath::Abs(lTrack->Eta())<fEta)  { //for mean pt, only consider -0.4-0.4 region
        FillWPCounter(wp[0],weff,p1);
      }  //Actually, no need for if() statememnt now since GFW knows about eta's, so I can fill it all the time
      fGFW->Fill(lTrack->Eta(),1,lTrack->Phi(),wacc*weff,3); //filling both gap (bit mask 1) and full (bit mas 2)
    };
  };
  if(wp[0][0]==0) return; //if no single charged particles, then surely no PID either, no sense to continue
  fMultiVsV0MCorr[0]->Fill(l_Cent,nTotNoTracks);
  //here in principle one could use the GFW output to check if the values are calculated, but this is more efficient
  if(fConsistencyFlag&1) if(!lPosCount || !lNegCount) return; // only events where v2{2, gap} could be calculated
  if(fConsistencyFlag&2) if(nTotNoTracks<4) return; //only events where v2{4} can be calculated (assuming same region as nch)
  if(fConsistencyFlag&4) if(lPosCount<2 || lNegCount<2) return; //Only events where v2{4, gap} can be calculated
  if(fConsistencyFlag&8) if(lMidCount<2) return; //If less than 2 particles in mid, reject. Relevant, if calculating v24{3-sub}
  fMultiVsV0MCorr[1]->Fill(l_Cent,nTotNoTracks);
/*
for(Int_t i=0;i<1;i++) { //No PID = index is only 1
  if(!l_ptCount[i]) continue;
  Double_t fillWeight = fUseWeightsOne?1:l_ptCount[i];
  fmPT[i]->Fill(lMulti,l_ptsum[i]/l_ptCount[i],fillWeight);
}
*/
  //Filling pT variance
  Double_t l_Multi = fUseNch?(1.0*nTotNoTracks):l_Cent;
  //A check in case l_Multi is completely off the charts (in MC, sometimes it ends up being... -Xe-310???)
  if(fUseNch && l_Multi<1) return;
  Double_t l_Random = fRndm->Rndm();
  for(Int_t i=0;i<1;i++) {
    if(!wp[i][0]) continue;
    outVals[i][0] = fmPT[i]->GetBinContent(fmPT[i]->FindBin(l_Multi));
    CalculateMptValues(outVals[i],wp[i]);
    Double_t ptvarw = fUseWeightsOne?1:outVals[i][2];
    if(outVals[i][2]!=0)
      fptvar[i]->FillProfile(l_Multi,outVals[i][1]/outVals[i][2],ptvarw,l_Random);
  };
  fV0MMulti->Fill(l_Cent);
  fMultiDist->Fill(l_Multi);
  fCkCont->FillObs(wp[0],l_Multi,l_Random);
  if(wp[0][0]!=0)
    fMptClosure->Fill(l_Multi,wp[0][1]/wp[0][0],wp[0][0]);
  PostData(1,fptVarList);
  //Filling FCs
  for(Int_t l_ind=0; l_ind<corrconfigs.size(); l_ind++) {
    Bool_t filled = FillFCs(corrconfigs.at(l_ind),l_Multi,l_Random);
  };
  PostData(2,fFC);
  for(Int_t i=0;i<1;i++) {
    FillCovariance(fCovariance[i],corrconfigs.at(i*4),l_Multi,outVals[i][3]-outVals[i][0],wp[i][0],l_Random);
    FillCovariance(fCovariance[i+4],corrconfigs.at((i+1)*4),l_Multi,outVals[i][3]-outVals[i][0],wp[i][0],l_Random);
    FillCovariance(fCovariance[8],corrconfigs.at(14),l_Multi,outVals[i][3]-outVals[i][0],wp[i][0],l_Random);
    //following is not necessary since we don't have any POIs
  };
  PostData(3,fCovList);
  if(outVals[0][0]==0) return;
  Int_t indx =   fV2dPtMulti->FindBin(l_Multi);
  //To avoid filling out of boundaries -- aparently, important for MC
  if(indx<1 || indx>fV2dPtMulti->GetNbinsX()) return;
  fV2dPtMulti->Fill(l_Multi);
  // printf("Will use dpt v2 profile index %i (out of %i-1), multiplicity is %f\n",indx,fV2dPtList->GetEntries(),l_Multi);
  Fillv2dPtFCs(corrconfigs.at(0),outVals[0][3]/outVals[0][0]-1,0,indx);
  Fillv2dPtFCs(corrconfigs.at(4),outVals[0][3]/outVals[0][0]-1,0,indx);
  PostData(4,fV2dPtList);
}
void AliAnalysisTaskMeanPtV2Corr::ProduceEfficiencies(AliAODEvent *fAOD, const Double_t &vz, const Double_t &l_Cent, Double_t *vtxp) {
  AliAODTrack *lTrack;
  Double_t l_ptsum[]={0,0,0,0};
  Double_t l_ptCount[]={0,0,0,0};
  Double_t trackXYZ[3];
  Double_t ptMin = fPtBins[0];
  Double_t ptMax = fPtBins[fNPtBins];
  Float_t dcaxy, dcaz;
  fV0MMulti->Fill(l_Cent);
  TClonesArray *tca = (TClonesArray*)fInputEvent->FindListObject("mcparticles");
  Int_t nPrim = tca->GetEntries();
  AliAODMCParticle *lPart;
  Int_t partNotFetched=0;
  Int_t lNchGen=0;
  Int_t lNchRec=0;
  Int_t nSpecies=7;
  for (Int_t ipart = 0; ipart < nPrim; ipart++) {
    lPart = (AliAODMCParticle*)tca->At(ipart);
    if (!lPart) { partNotFetched++; continue; };
    /* get particlePDG */
    Int_t pdgcode = TMath::Abs(lPart->GetPdgCode());
    if (!lPart->IsPhysicalPrimary()) continue;
    if (lPart->Charge()==0.) continue;
    Double_t pt = lPart->Pt();
    Double_t lEta = TMath::Abs(lPart->Eta());
    if (pt<0.15 || pt>50.) continue;
    if(pt>0.2 && pt<3 && lEta<fEtaNch) lNchGen++;
    if (lEta > fEta) continue;
    fEfficiency[nSpecies]->Fill(pt,l_Cent);
    Int_t pidind = GetPIDIndex(pdgcode);
    if(pidind) fEfficiency[nSpecies+pidind]->Fill(pt,l_Cent);
  };
  for(Int_t lTr=0;lTr<fAOD->GetNumberOfTracks();lTr++) {
    lTrack = (AliAODTrack*)fAOD->GetTrack(lTr);
    if(!lTrack) continue;
    if(!AcceptAODTrack(lTrack,trackXYZ,ptMin,ptMax,vtxp)) continue;
    Double_t lpt = lTrack->Pt();
    if(lpt>0.2 && lpt<3 && TMath::Abs(lTrack->Eta())<fEtaNch) lNchRec++;
    Int_t fLabel = lTrack->GetLabel();
    Int_t index = TMath::Abs(fLabel);
    if (index < 0) continue;
    lPart = (AliAODMCParticle*)tca->At(index);//fMCEvent->Particle(index);
    if(!lPart) continue;
    if (TMath::Abs(lPart->Eta()) > fEta) continue;
    Int_t pdgcode = lPart->GetPdgCode();
    Int_t pidind = GetPIDIndex(pdgcode);
    if(lPart->IsPhysicalPrimary()) {
        fEfficiency[0]->Fill(lPart->Pt(),l_Cent);
        if(pidind)
          fEfficiency[pidind]->Fill(lPart->Pt(),l_Cent);
    }
    if(lPart->IsSecondaryFromWeakDecay() || lPart->IsSecondaryFromMaterial()) {
        fEfficiency[2*nSpecies]->Fill(lPart->Pt(),l_Cent);
        if(pidind)
          fEfficiency[pidind+(2*nSpecies)]->Fill(lPart->Pt(),l_Cent);
    };
  };
  if(fUseCorrCuts) if(!CheckNchCorrelation(lNchGen,lNchRec)) return;
  fNchTrueVsReco->Fill(lNchGen,lNchRec);
  PostData(1,fEfficiencyList);
}
void AliAnalysisTaskMeanPtV2Corr::CovSkipMpt(AliAODEvent *fAOD, const Double_t &vz, const Double_t &l_Cent, Double_t *vtxp) {
  AliAODTrack *lTrack;
  Double_t wp[5] = {0,0,0,0,0}; //Initial values, [species][w*p]
  Double_t trackXYZ[3];
  fGFW->Clear();
  Int_t iCent = fV0MMulti->FindBin(l_Cent);
  if(!iCent || iCent>fV0MMulti->GetNbinsX()) return;
  iCent--;
  Int_t lPosCount=0, lNegCount=0, lMidCount=0;
  Double_t ptMin = fPtBins[0];
  Double_t ptMax = fPtBins[fNPtBins];
  Int_t nTotNoTracks=0;
  if(fIsMC) {
    Int_t nTotNoTracksMC=0;
    Int_t nTotNoTracksReco=0;
    if(fUseRecoNchForMC) nTotNoTracksReco = GetNtotTracks(fAOD,ptMin,ptMax,vtxp);
    TClonesArray *tca = (TClonesArray*)fInputEvent->FindListObject("mcparticles");
    Int_t nPrim = tca->GetEntries();
    AliAODMCParticle *lPart;
    for(Int_t ipart = 0; ipart < nPrim; ipart++) {
      lPart = (AliAODMCParticle*)tca->At(ipart);
      if (!lPart->IsPhysicalPrimary()) continue;
      if (lPart->Charge()==0.) continue;
      //Hardcoded cuts to inhereted from AcceptAODTrack
      Double_t leta = lPart->Eta();
      if (TMath::Abs(leta) > 0.8) continue;
      Double_t pt = lPart->Pt();
      if (pt<0.2 || pt>3.) continue;
      if(leta<-fEtaV2Sep) lNegCount++;
      if(leta>fEtaV2Sep) lPosCount++;
      if(TMath::Abs(leta)<fEtaNch) nTotNoTracksMC++; //Nch calculated in EtaNch region
      Double_t lpt = lPart->Pt();
      if(TMath::Abs(leta)<fEta)  { //for mean pt, only consider -0.4-0.4 region
        FillWPCounter(wp,1,lpt); //weight = 1, naturally
      }  //Actually, no need for if() statememnt now since GFW knows about eta's, so I can fill it all the time
      fGFW->Fill(leta,1,lPart->Phi(),1,3); //filling both gap (bit mask 1) and full (bit mas 2). Since this is MC, weight is 1.
      // FillMeanPtCounterWW(lpt,l_ptsum[0],l_ptCount[0],1); //MC truth, so weight = 1
    };
    nTotNoTracks = fUseRecoNchForMC?nTotNoTracksReco:nTotNoTracksMC;
    if(fUseRecoNchForMC) fNchTrueVsReco->Fill(nTotNoTracksMC,nTotNoTracksReco);
  } else {
    if(!LoadMyWeights(fAOD->GetRunNumber())) return; //Only load wieghts for data
    Bool_t usingPseudoEff = (fPseudoEfficiency<1);
    nTotNoTracks=GetNtotTracks(fAOD,ptMin,ptMax,vtxp);
    for(Int_t lTr=0;lTr<fAOD->GetNumberOfTracks();lTr++) {
      if(usingPseudoEff) if(fRndm->Uniform()>fPseudoEfficiency) continue;
      lTrack = (AliAODTrack*)fAOD->GetTrack(lTr);
      if(!lTrack) continue;
      Double_t leta = lTrack->Eta();
      Double_t trackXYZ[] = {0.,0.,0.};
      if(!AcceptAODTrack(lTrack,trackXYZ,ptMin,ptMax,vtxp)) continue;
      // if(TMath::Abs(leta)<fEtaNch) nTotNoTracks+=1;
      if(leta<-fEtaV2Sep) lNegCount++;
      if(leta>fEtaV2Sep) lPosCount++;
      if(fEtaV2Sep>0 && TMath::Abs(leta)<fEtaV2Sep) lMidCount++;
      Double_t p1 = lTrack->Pt();
      Double_t weff = fEfficiencies[iCent]->GetBinContent(fEfficiencies[iCent]->FindBin(p1));
      if(weff==0) continue;
      Double_t wacc = fWeights[0]->GetNUA(lTrack->Phi(),lTrack->Eta(),vz);
      weff = 1./weff;
      if(TMath::Abs(lTrack->Eta())<fEta)  { //for mean pt, only consider -0.4-0.4 region
        FillWPCounter(wp,weff,p1);
      }  //Actually, no need for if() statememnt now since GFW knows about eta's, so I can fill it all the time
      fGFW->Fill(lTrack->Eta(),1,lTrack->Phi(),wacc*weff,3); //filling both gap (bit mask 1) and full (bit mas 2)
    };
  };
  if(wp[0]==0) return; //if no single charged particles, then surely no PID either, no sense to continue
  fMultiVsV0MCorr[0]->Fill(l_Cent,nTotNoTracks);
  //here in principle one could use the GFW output to check if the values are calculated, but this is more efficient
  if(fConsistencyFlag&1) if(!lPosCount || !lNegCount) return; // only events where v2{2, gap} could be calculated
  if(fConsistencyFlag&2) if(nTotNoTracks<4) return; //only events where v2{4} can be calculated (assuming same region as nch)
  if(fConsistencyFlag&4) if(lPosCount<2 || lNegCount<2) return; //Only events where v2{4, gap} can be calculated
  if(fConsistencyFlag&8) if(lMidCount<2) return; //If less than 2 particles in mid, reject. Relevant, if calculating v24{3-sub}
  fMultiVsV0MCorr[1]->Fill(l_Cent,nTotNoTracks);
  //Filling pT variance
  Double_t l_Multi = fUseNch?(1.0*nTotNoTracks):l_Cent;
  //A check in case l_Multi is completely off the charts (in MC, sometimes it ends up being... -Xe-310???)
  if(fUseNch && l_Multi<1) return;
  Double_t l_Random = fRndm->Rndm();
  fCkCont->FillObs(wp,l_Multi,l_Random);
  fV0MMulti->Fill(l_Cent);
  fMultiDist->Fill(l_Multi);
  PostData(1,fptVarList);
  //Filling FCs
  for(Int_t l_ind=0; l_ind<corrconfigs.size(); l_ind++) {
    Bool_t filled = FillFCs(corrconfigs.at(l_ind),l_Multi,l_Random);
  };
  PostData(2,fFC);
  Double_t mptev = wp[1]/wp[0];
  for(Int_t i=0;i<1;i++) {
    FillCovariance(fCovariance[0],corrconfigs.at(i*4),l_Multi,mptev,wp[0],l_Random);
    FillCovariance(fCovariance[1],corrconfigs.at(i*4),l_Multi,1,wp[0],l_Random);
    FillCovariance(fCovariance[2],corrconfigs.at((i+1)*4),l_Multi,mptev,wp[0],l_Random);
    FillCovariance(fCovariance[3],corrconfigs.at((i+1)*4),l_Multi,1,wp[0],l_Random);
    FillCovariance(fCovariance[4],corrconfigs.at(14),l_Multi,mptev,wp[0],l_Random);
    FillCovariance(fCovariance[5],corrconfigs.at(14),l_Multi,1,wp[0],l_Random);
    //following is not necessary since we don't have any POIs
  };
  PostData(3,fCovList);
  // if(mptev) return;
  // Int_t indx =   fV2dPtMulti->FindBin(l_Multi);
  // //To avoid filling out of boundaries -- aparently, important for MC
  // if(indx<1 || indx>fV2dPtMulti->GetNbinsX()) return;
  // fV2dPtMulti->Fill(l_Multi);
  // printf("Will use dpt v2 profile index %i (out of %i-1), multiplicity is %f\n",indx,fV2dPtList->GetEntries(),l_Multi);
  // Fillv2dPtFCs(corrconfigs.at(0),mptev,0,indx);
  // Fillv2dPtFCs(corrconfigs.at(4),mptev,0,indx);
  // PostData(4,fV2dPtList);
}

Bool_t AliAnalysisTaskMeanPtV2Corr::FillFCs(const AliGFW::CorrConfig &corconf, const Double_t &cent, const Double_t &rndmn, const Bool_t debug) {
  Double_t dnx, val;
  dnx = fGFW->Calculate(corconf,0,kTRUE).Re();
  if(debug) printf("FillFCs: dnx = %f\n",dnx);
  if(dnx==0) return kFALSE;
  if(!corconf.pTDif) {
    val = fGFW->Calculate(corconf,0,kFALSE).Re()/dnx;
    if(debug) printf("FillFCs: val = %f\n",val);
    if(TMath::Abs(val)<1)
      fFC->FillProfile(corconf.Head.Data(),cent,val,fUseWeightsOne?1:dnx,rndmn);
    return kTRUE;
  };
  return kTRUE;
};
Bool_t AliAnalysisTaskMeanPtV2Corr::Fillv2dPtFCs(const AliGFW::CorrConfig &corconf, const Double_t &dpt, const Double_t &rndmn, const Int_t index) {
  if(!index || index>fV2dPtList->GetEntries()) return kFALSE;
  Double_t dnx, val;
  dnx = fGFW->Calculate(corconf,0,kTRUE).Re();
  if(dnx==0) return kFALSE;
  if(!corconf.pTDif) {
    val = fGFW->Calculate(corconf,0,kFALSE).Re()/dnx;
    if(TMath::Abs(val)<1)
      ((AliGFWFlowContainer*)fV2dPtList->At(index))->FillProfile(corconf.Head.Data(),dpt,val,fUseWeightsOne?1:dnx,rndmn);
    return kTRUE;
  };
  return kTRUE;
};

Bool_t AliAnalysisTaskMeanPtV2Corr::FillCovariance(AliProfileBS *target, const AliGFW::CorrConfig &corconf, const Double_t &cent, const Double_t &d_mpt, const Double_t &dw_mpt, const Double_t &l_rndm) {
  Double_t dnx, val;
  dnx = fGFW->Calculate(corconf,0,kTRUE).Re();
  if(dnx==0) return kFALSE;
  if(!corconf.pTDif) {
    val = fGFW->Calculate(corconf,0,kFALSE).Re()/dnx;
    if(TMath::Abs(val)<1)
      target->FillProfile(cent,val*d_mpt,fUseWeightsOne?1:dnx*dw_mpt,l_rndm);
    return kTRUE;
  };
  return kTRUE;
};
void AliAnalysisTaskMeanPtV2Corr::CreateCorrConfigs() {

  corrconfigs.push_back(GetConf("ChGap22","refP {2} refN {-2}", kFALSE));
  corrconfigs.push_back(GetConf("ChGap24","refP {2 2} refN {-2 -2}", kFALSE));
  corrconfigs.push_back(GetConf("ChFull22","mid {2 -2}", kFALSE));
  corrconfigs.push_back(GetConf("ChFull24","mid {2 2 -2 -2}", kFALSE));
//v3:
  corrconfigs.push_back(GetConf("ChGap32","refP {3} refN {-3}", kFALSE));
  corrconfigs.push_back(GetConf("ChGap34","refP {3 3} refN {-3 -3}", kFALSE));
  corrconfigs.push_back(GetConf("ChFull32","mid {3 -3}", kFALSE));
  corrconfigs.push_back(GetConf("ChFull34","mid {3 3 -3 -3}", kFALSE));

  corrconfigs.push_back(GetConf("ChGap42","refP {4} refN {-4}", kFALSE));
//v24 3-sub
  if(fEtaV2Sep<0) return; //if eta < 0, then pos & neg are w/o SE and thus doesn't make sense to calculate v24
  corrconfigs.push_back(GetConf("LM22","refP {2} subMid {-2}", kFALSE));
  corrconfigs.push_back(GetConf("MR22","subMid {2} refN {-2}", kFALSE));
  corrconfigs.push_back(GetConf("LR22","refP {2} refN {-2}", kFALSE));
  corrconfigs.push_back(GetConf("LLMR24","refP {2 2} subMid {-2} refN {-2}", kFALSE));
  corrconfigs.push_back(GetConf("LMMR24","refP {2} subMid {-2 -2} refN {2}", kFALSE));
  corrconfigs.push_back(GetConf("LMRR24","refP {2} subMid {2} refN {-2 -2}", kFALSE));

  corrconfigs.push_back(GetConf("ChSC234","refP {2 3} refN {-2 -3}", kFALSE));
  corrconfigs.push_back(GetConf("ChSC244","refP {2 4} refN {-2 -4}", kFALSE));


  return;

  //ditch the last code for now, since we don't need PID
  corrconfigs.push_back(GetConf("ChPos22","chP {2} refN {-2}", kFALSE));
  corrconfigs.push_back(GetConf("ChNeg22","chN {2} refP {-2}", kFALSE));
  corrconfigs.push_back(GetConf("ChPos24","chP refP | OLchP {2 2} refN {-2 -2}", kFALSE));
  corrconfigs.push_back(GetConf("ChNeg24","chN refN | OLchN {2 2} refP {-2 -2}", kFALSE));
  return;
//pi
  corrconfigs.push_back(GetConf("PiPos22","piP {2} refN {-2}", kFALSE));
  corrconfigs.push_back(GetConf("PiNeg22","piN {2} refP {-2}", kFALSE));
  corrconfigs.push_back(GetConf("PiPos24","piP refP | OLpiP {2 2} refN {-2 -2}", kFALSE));
  corrconfigs.push_back(GetConf("PiNeg24","piN refN | OLpiN {2 2} refP {-2 -2}", kFALSE));
//ka
  corrconfigs.push_back(GetConf("KaPos22","kaP {2} refN {-2}", kFALSE));
  corrconfigs.push_back(GetConf("KaNeg22","kaN {2} refP {-2}", kFALSE));
  corrconfigs.push_back(GetConf("KaPos24","kaP refP | OLkaP {2 2} refN {-2 -2}", kFALSE));
  corrconfigs.push_back(GetConf("KaNeg24","kaN refN | OLkaN {2 2} refP {-2 -2}", kFALSE));
//pr
  corrconfigs.push_back(GetConf("PrPos22","prP {2} refN {-2}", kFALSE));
  corrconfigs.push_back(GetConf("PrNeg22","prN {2} refP {-2}", kFALSE));
  corrconfigs.push_back(GetConf("PrPos24","prP refP | OLprP {2 2} refN {-2 -2}", kFALSE));
  corrconfigs.push_back(GetConf("PrNeg24","prN refN | OLprN {2 2} refP {-2 -2}", kFALSE));


};
void AliAnalysisTaskMeanPtV2Corr::GetSingleWeightFromList(AliGFWWeights **inWeights, TString pf) {
  (*inWeights) = (AliGFWWeights*)fWeightList->FindObject(Form("weight_%s",pf.Data()));
  if(!(*inWeights)) AliFatal(Form("Could not find weight %s in weight list\n", pf.Data()));
  if(!(*inWeights)->CalculateIntegratedEff()) AliFatal("Could not calculate integrated efficiency!\n");
  (*inWeights)->CreateNUA();
};
void AliAnalysisTaskMeanPtV2Corr::LoadWeightAndMPT() {//AliAODEvent *inEv) {
  if(!fRequireReloadOnRunChange) return;
  if(!fWeightList) AliFatal("Weight list not set!\n");

  // Int_t l_RunNo = inEv->GetRunNumber();
  TString spNames[] = {"ch","pi","ka","pr"};
  fWeights = new AliGFWWeights*[4];
  for(Int_t i=0;i<4;i++) GetSingleWeightFromList(&fWeights[i],spNames[i]);
  if(fStageSwitch==3) { //if on switch 3 (full), then also check if need to preload dif. weight
    if(fmPT) delete [] fmPT;
    fmPT = new TProfile*[4];
    for(Int_t i=0;i<4;i++) {
      fmPT[i] = (TProfile*)fMPTList->FindObject(Form("MeanPt_%s",spNames[i].Data()));
      if(!fmPT[i]) AliFatal(Form("Could not find mean pT for %s in the list\n",spNames[i].Data()));
    };
  }
}
Bool_t AliAnalysisTaskMeanPtV2Corr::WithinSigma(Double_t SigmaCut, AliAODTrack *inTrack, AliPID::EParticleType partType) {
  if(!fPIDResponse) return kFALSE;
  Double_t nSigmaTPC = fPIDResponse->NumberOfSigmasTPC(inTrack,partType);
  Double_t nSigmaTOF = fPIDResponse->NumberOfSigmasTOF(inTrack,partType);
  return (TMath::Sqrt(nSigmaTPC*nSigmaTPC + nSigmaTOF*nSigmaTOF) < SigmaCut);
}
Int_t AliAnalysisTaskMeanPtV2Corr::GetBayesPIDIndex(AliAODTrack *l_track) {
  Double_t l_Probs[AliPID::kSPECIES];
  Double_t l_MaxProb[] = {0.95,0.85,0.85};
  Bool_t l_TOFUsed = fBayesPID->ComputeProbabilities(l_track, fPIDResponse, l_Probs) & AliPIDResponse::kDetTOF;
  Int_t pidInd = 0;
  for(Int_t i=0;i<AliPID::kSPECIES; i++) pidInd=(l_Probs[i]>l_Probs[pidInd])?i:pidInd;
  Int_t retInd = pidInd-AliPID::kPion; //Not interested in e+mu, so realign to 0
  if(retInd<0 || retInd>2) return -1; //Shouldn't be larger than 2, but just to be safe
  if(l_Probs[pidInd] < l_MaxProb[retInd]) return -1;
  //check nsigma cuts
  if(TMath::Abs(fPIDResponse->NumberOfSigmasTPC(l_track,(AliPID::EParticleType)pidInd))>3) return -1;
  if(l_TOFUsed) if(TMath::Abs(fPIDResponse->NumberOfSigmasTOF(l_track,(AliPID::EParticleType)pidInd))>3) return -1;
  return retInd;
}
Bool_t AliAnalysisTaskMeanPtV2Corr::LoadMyWeights(const Int_t &lRunNo) {
  if(!fWeightList) AliFatal("NUA list not set or does not exist!\n");
  if(lRunNo && lRunNo == fRunNo) return kTRUE;
  TString lBase(""); //base
  TString lSubfix(""); //subfix
  if(fWeightSubfix.IsNull()) { //If none specified, then follow the usual procedure
    lBase = Form("w%i",lRunNo);
    lSubfix = fGFWSelection->NeedsExtraWeight()?fGFWSelection->GetSystPF():"";
  } else {
    Int_t delind = fWeightSubfix.Index(";");
    if(delind<0) {//Only base
      lBase = fWeightSubfix;
      lSubfix = fGFWSelection->NeedsExtraWeight()?fGFWSelection->GetSystPF():"";
    } else if(!delind) {//Standard base, override subfix
      lBase = Form("w%i",lRunNo);
      lSubfix = fWeightSubfix(1,fWeightSubfix.Length());
    } else {
      lBase = fWeightSubfix(0,delind);
      lSubfix = fWeightSubfix(delind+1,fWeightSubfix.Length());
    }
  }
  lBase+=lSubfix;
  // if(!fWeights) { fWeights = new AliGFWWeights*[1]; };
  // if(fWeights[0]) delete fWeights[0];
  fWeights[0] = (AliGFWWeights*)fWeightList->FindObject(lBase.Data());
  if(!fWeights[0]) AliFatal(Form("Weights %s not not found in the list provided!\n",lBase.Data()));
  fWeights[0]->CreateNUA();
  return kTRUE;
}
Double_t AliAnalysisTaskMeanPtV2Corr::GetMyWeight(Double_t eta, Double_t phi, Int_t pidind) {
  Int_t etaind = fNUAHist[pidind]->GetXaxis()->FindBin(eta);
  Int_t phiind = fNUAHist[pidind]->GetYaxis()->FindBin(phi);
  return fNUAHist[pidind]->GetBinContent(etaind,phiind);
}
void AliAnalysisTaskMeanPtV2Corr::SetPtBins(Int_t nPtBins, Double_t *PtBins) {
  if(fPtAxis) delete fPtAxis;
  fPtAxis = new TAxis(nPtBins, PtBins);
}
void AliAnalysisTaskMeanPtV2Corr::SetMultiBins(Int_t nMultiBins, Double_t *multibins) {
  if(fMultiAxis) delete fMultiAxis;
  fMultiAxis = new TAxis(nMultiBins, multibins);
}
void AliAnalysisTaskMeanPtV2Corr::SetV0MBins(Int_t nMultiBins, Double_t *multibins) {
  if(fV0MMultiAxis) delete fV0MMultiAxis;
  fV0MMultiAxis = new TAxis(nMultiBins, multibins);
}
void AliAnalysisTaskMeanPtV2Corr::SetV2dPtMultiBins(Int_t nMultiBins, Double_t *multibins) {
  if(fV2dPtMulti) delete fV2dPtMulti;
  fV2dPtMulti = new TH1D("v2_vs_mpt_mbins","v2_vs_mpt_mbins",nMultiBins, multibins);
}
Double_t *AliAnalysisTaskMeanPtV2Corr::GetBinsFromAxis(TAxis *inax) {
  Int_t lBins = inax->GetNbins();
  Double_t *retBins = new Double_t[lBins+1];
  for(Int_t i=0;i<lBins;i++)
    retBins[i] = inax->GetBinLowEdge(i+1);
  retBins[lBins] = inax->GetBinUpEdge(lBins);
  return retBins;
}
Int_t AliAnalysisTaskMeanPtV2Corr::GetPIDIndex(const Int_t &pdgcode) {
  if(TMath::Abs(pdgcode)==211) return 1;
  if(TMath::Abs(pdgcode)==321) return 2;
  if(TMath::Abs(pdgcode)==2212) return 3;
  if(TMath::Abs(pdgcode)==3222 || TMath::Abs(pdgcode)==3112) return 4;
  if(TMath::Abs(pdgcode)==3312) return 5;
  if(TMath::Abs(pdgcode)==3334) return 6;
  return 0;
}
