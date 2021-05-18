/**
 * File              : AliAnalysisTaskForStudents.cxx
 * Author            : Anton Riedel <anton.riedel@tum.de>
 * Date              : 07.05.2021
 * Last Modified Date: 18.05.2021
 * Last Modified By  : Anton Riedel <anton.riedel@tum.de>
 */
/*************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/**************************************
 * template class for student projects *
 **************************************/

#include "AliAODEvent.h"
#include "AliAODInputHandler.h"
#include "AliAnalysisTaskForStudents.h"
#include "AliLog.h"
#include "AliMultSelection.h"
#include "Riostream.h"
#include "TFile.h"

using std::cout;
using std::endl;

ClassImp(AliAnalysisTaskForStudents)

    //================================================================================================================

    AliAnalysisTaskForStudents::AliAnalysisTaskForStudents(
        const char *name, Bool_t useParticleWeights)
    : AliAnalysisTaskSE(name), fHistList(NULL),
      // Control histograms:
      fControlHistogramsList(NULL), fPtHist_beforeCut(NULL),
      fPtHist_afterCut(NULL), fNbinsPt(1000), fMinBinPt(0.), fMaxBinPt(10.),
      fPhiHist_beforeCut(NULL), fPhiHist_afterCut(NULL), fNbinsPhi(1000),
      fMinBinPhi(0.), fMaxBinPhi(10.), fEtaHist_beforeCut(NULL),
      fEtaHist_afterCut(NULL), fNbinsEta(1000), fMinBinEta(0.), fMaxBinEta(10.),
      fMulHist_beforeCut(NULL), fMulHist_afterCut(NULL), fNbinsMul(1000),
      fMinBinMul(0.), fMaxBinMul(10.),
      /* centrality selection */
      fCentralityHist_beforeCut(NULL), fCentralityHist_afterCut(NULL),
      fNbinsCentrality(0), fMinCentrality(0), fMaxCentrality(0),
      /* pt cuts */
      fptMin(0), fptMax(0),
      /* filterbit */
      fFilterbit(0),
      // Final results:
      fFinalResultsList(NULL), fAveragePhiHist(NULL), fNbinsAveragePhi(1000),
      fMinBinAveragePhi(0.), fMaxBinAveragePhi(10.) {
  // Constructor.

  AliDebug(2, "AliAnalysisTaskForStudents::AliAnalysisTaskForStudents(const "
              "char *name, Bool_t useParticleWeights)");

  // Base list:
  fHistList = new TList();
  fHistList->SetName("outputStudentAnalysis");
  fHistList->SetOwner(kTRUE);

  // Initialize all arrays:
  this->InitializeArrays();

  // Define input and output slots here
  // Input slot #0 works with an AliFlowEventSimple
  // DefineInput(0, AliFlowEventSimple::Class());
  // Input slot #1 is needed for the weights input file:
  // if(useParticleWeights)
  //{
  // DefineInput(1, TList::Class());
  //}
  // Output slot #0 is reserved
  // Output slot #1 writes into a TList container

  DefineOutput(1, TList::Class());

  if (useParticleWeights) {
    // not needed for the time being
  }

} // AliAnalysisTaskForStudents::AliAnalysisTaskForStudents(const char *name,
  // Bool_t useParticleWeights):

//================================================================================================================

AliAnalysisTaskForStudents::AliAnalysisTaskForStudents()
    : AliAnalysisTaskSE(), fHistList(NULL),
      // Control histograms:
      fControlHistogramsList(NULL), fPtHist_beforeCut(NULL),
      fPtHist_afterCut(NULL), fNbinsPt(1000), fMinBinPt(0.), fMaxBinPt(10.),
      fPhiHist_beforeCut(NULL), fPhiHist_afterCut(NULL), fNbinsPhi(1000),
      fMinBinPhi(0.), fMaxBinPhi(10.), fEtaHist_beforeCut(NULL),
      fEtaHist_afterCut(NULL), fNbinsEta(1000), fMinBinEta(0.), fMaxBinEta(10.),
      fMulHist_beforeCut(NULL), fMulHist_afterCut(NULL), fNbinsMul(1000),
      fMinBinMul(0.), fMaxBinMul(10.),
      /* centrality selection */
      fCentralityHist_beforeCut(NULL), fCentralityHist_afterCut(NULL),
      fNbinsCentrality(0), fMinCentrality(0), fMaxCentrality(0),
      /* pt cuts */
      fptMin(0), fptMax(0),
      /* filterbit */
      fFilterbit(0),
      // Final results:
      fFinalResultsList(NULL), fAveragePhiHist(NULL), fNbinsAveragePhi(1000),
      fMinBinAveragePhi(0.), fMaxBinAveragePhi(10.) {
  // Dummy constructor.

  AliDebug(2, "AliAnalysisTaskForStudents::AliAnalysisTaskForStudents()");

} // AliAnalysisTaskForStudents::AliAnalysisTaskForStudents():

//================================================================================================================

AliAnalysisTaskForStudents::~AliAnalysisTaskForStudents() {
  // Destructor.

  if (fHistList)
    delete fHistList;

} // AliAnalysisTaskForStudents::~AliAnalysisTaskForStudents()

//================================================================================================================

void AliAnalysisTaskForStudents::UserCreateOutputObjects() {
  // Called at every worker node to initialize.

  // a) Trick to avoid name clashes, part 1;
  // b) Book and nest all lists;
  // c) Book all objects;
  // *) Trick to avoid name clashes, part 2.

  // a) Trick to avoid name clashes, part 1:
  Bool_t oldHistAddStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  // b) Book and nest all lists:
  this->BookAndNestAllLists();

  // c) Book all objects:
  this->BookControlHistograms();
  this->BookFinalResultsHistograms();

  // *) Trick to avoid name clashes, part 2:
  TH1::AddDirectory(oldHistAddStatus);

  PostData(1, fHistList);

} // void AliAnalysisTaskForStudents::UserCreateOutputObjects()

//================================================================================================================

void AliAnalysisTaskForStudents::UserExec(Option_t *) {
  // Main loop (called for each event).
  // a) Get pointer to AOD event:
  // b) Start analysis over AODs;
  // c) Reset event-by-event objects;
  // d) PostData.

  // a) Get pointer to AOD event:
  AliAODEvent *aAOD = dynamic_cast<AliAODEvent *>(InputEvent()); // from TaskSE
  if (!aAOD) {
    return;
  }

  /* get object for determining centrality */
  AliMultSelection *ams =
      (AliMultSelection *)aAOD->FindListObject("MultSelection");
  if (!ams) {
    return;
  }
  /* get centrality percentile */
  Float_t MultiplicityPercentile =
      ams->GetMultiplicityPercentile(fCentralitySelCriterion);

  /* cut event if it is not within the centrality percentile */
  if (MultiplicityPercentile >= fMinCentrality &&
      MultiplicityPercentile < fMaxCentrality) {
    fCentralityHist_beforeCut->Fill(MultiplicityPercentile);
  } else {
    return;
  }

  /* cut event if the primary vertex is too displaced along the z direction
   * (->beam direction) */
  /* if the displacement is too large, the detectors will miss many produced
   * particles */
  /* this cuts essential the multiplicity */
  if (aAOD->GetPrimaryVertex()->GetZ() > fzDisMax) {
    return;
  }

  /* fill control histogram for multiplicity percentile */
  fCentralityHist_afterCut->Fill(ams->GetMultiplicityPercentile("V0M"));

  // b) Start analysis over AODs:
  /* number of all tracks in current event */
  /* use this as Multiplicity */
  Int_t nTracks = aAOD->GetNumberOfTracks();

  /* count number of tracks for computing multiplicity */
  Int_t nTracks_beforeCut = 0;
  Int_t nTracks_afterCut = 0;

  /* loop over all tracks in the event */
  for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
    /* getting a pointer to a track */
    AliAODTrack *aTrack = dynamic_cast<AliAODTrack *>(aAOD->GetTrack(iTrack));
    /* protect against invalid pointers */
    if (!aTrack) {
      continue;
    }

    /* fill control histograms before cutting */
    Double_t pt = aTrack->Pt();    // Pt
    Double_t phi = aTrack->Phi();  // azimuthal angle
    Double_t eta = aTrack->Eta();  // pseudorapidity
    fPtHist_beforeCut->Fill(pt);   // filling pt distribution
    fPhiHist_beforeCut->Fill(phi); // filling phi distriubtion
    fEtaHist_beforeCut->Fill(eta); // filling eta distribution
    nTracks_beforeCut++;

    /* apply cuts, i.e. */
    /* filterbit */
    /* momentun */

    /* filter bit 128 denotes TPC-only tracks, use only them for the */
    /* analysis */
    /* for hybrid tracks use filterbit 782 */
    /* for more information about filterbits see the online week */
    /* the filterbits can change from run to run */
    /* fill control histograms */
    if ((!aTrack->TestFilterBit(fFilterbit)) && (fptMin < pt) &&
        (pt < fptMax)) {
      /* fill control histograms after cutting */
      fPtHist_afterCut->Fill(pt);   // filling pt distribution
      fPhiHist_afterCut->Fill(phi); // filling phi distriubtion
      fEtaHist_afterCut->Fill(eta); // filling eta distribution
      nTracks_afterCut++;
    }
  }

  /* fill control histogram for Multiplicity after counting all tracks*/
  fMulHist_beforeCut->Fill(nTracks_beforeCut);
  fMulHist_afterCut->Fill(nTracks_afterCut);

  // c) Reset event-by-event objects:
  // ...
  // Double_t px = aTrack->Px(); // x-component of momenta
  // Double_t py = aTrack->Py(); // y-component of momenta
  // Double_t pz = aTrack->Pz(); // z-component of momenta
  // Double_t e = aTrack->E();  // energy
  // Double_t charge = aTrack->Charge(); // charge

  // d) PostData:
  PostData(1, fHistList);

} // void AliAnalysisTaskForStudents::UserExec(Option_t *)

//================================================================================================================

void AliAnalysisTaskForStudents::Terminate(Option_t *) {
  // Accessing the merged output list.

  /* fHistList = (TList *)GetOutputData(1); */
  if (!fHistList) {
    cout << "fHistList not around?" << endl;
  }

  // Do some calculation in offline mode here:

  // ... your code for offline calculations ...

  fAveragePhiHist->SetBinContent(1, fPhiHist_afterCut->GetMean());

} // end of void AliAnalysisTaskForStudents::Terminate(Option_t *)

//================================================================================================================

void AliAnalysisTaskForStudents::InitializeArrays() {
  // Initialize all data members which are arrays in this method.

 // This is important, since these objects cannot be initialized directly in the constructor list. 

} // void AliAnalysisTaskForStudents::InitializeArrays()

//=======================================================================================================================

void AliAnalysisTaskForStudents::BookAndNestAllLists() {
  // Book and nest all lists nested in the base list fHistList.

  // a) Book and nest lists for control histograms;
  // b) Book and nest lists for final results.

  TString sMethodName =
      "void AliAnalysisTaskForStudents::BookAndNestAllLists()";
  if (!fHistList) {
    Fatal(sMethodName.Data(), "fHistList is NULL");
  }

  // a) Book and nest lists for control histograms:
  fControlHistogramsList = new TList();
  fControlHistogramsList->SetName("ControlHistograms");
  fControlHistogramsList->SetOwner(kTRUE);
  fHistList->Add(fControlHistogramsList);

  // b) Book and nest lists for final results:
  fFinalResultsList = new TList();
  fFinalResultsList->SetName("FinalResults");
  fFinalResultsList->SetOwner(kTRUE);
  fHistList->Add(fFinalResultsList);

} // void AliAnalysisTaskForStudents::BookAndNestAllLists()

//=======================================================================================================================

void AliAnalysisTaskForStudents::BookControlHistograms() {
  // Book all control histograms.

  // a) Book histogram to hold pt spectra:
  fPtHist_beforeCut = new TH1F("fPtHist_beforeCut", "atrack->Pt(), before cut",
                               fNbinsPt, fMinBinPt, fMaxBinPt);
  fPtHist_beforeCut->SetStats(kFALSE);
  fPtHist_beforeCut->SetFillColor(kBlue - 10);
  fPtHist_beforeCut->GetXaxis()->SetTitle("p_{t}");
  fControlHistogramsList->Add(fPtHist_beforeCut);

  fPtHist_afterCut = new TH1F("fPtHist_afterCut", "atrack->Pt(), after cut",
                              fNbinsPt, fMinBinPt, fMaxBinPt);
  fPtHist_afterCut->SetStats(kFALSE);
  fPtHist_afterCut->SetFillColor(kRed - 10);
  fPtHist_afterCut->GetXaxis()->SetTitle("p_{t}");
  fControlHistogramsList->Add(fPtHist_afterCut);

  // b) Book histogram to hold phi distribution
  fPhiHist_beforeCut =
      new TH1F("fPhiHist_beforeCut", "atrack->Phi(), before cut", fNbinsPhi,
               fMinBinPhi, fMaxBinPhi);
  fPhiHist_beforeCut->SetStats(kFALSE);
  fPhiHist_beforeCut->SetFillColor(kBlue - 10);
  fPhiHist_beforeCut->GetXaxis()->SetTitle("#varphi");
  fControlHistogramsList->Add(fPhiHist_beforeCut);

  fPhiHist_afterCut = new TH1F("fPhiHist_afterCut", "atrack->Phi(), after cut",
                               fNbinsPhi, fMinBinPhi, fMaxBinPhi);
  fPhiHist_afterCut->SetStats(kFALSE);
  fPhiHist_afterCut->SetFillColor(kRed - 10);
  fPhiHist_afterCut->GetXaxis()->SetTitle("#varphi");
  fControlHistogramsList->Add(fPhiHist_afterCut);

  // c) Book histogram to hold eta distribution
  fEtaHist_beforeCut =
      new TH1F("fEtaHist_beforeCut", "atrack->Eta(), before cut", fNbinsEta,
               fMinBinEta, fMaxBinEta);
  fEtaHist_beforeCut->SetStats(kFALSE);
  fEtaHist_beforeCut->SetFillColor(kBlue - 10);
  fEtaHist_beforeCut->GetXaxis()->SetTitle("#eta");
  fControlHistogramsList->Add(fEtaHist_beforeCut);

  fEtaHist_afterCut = new TH1F("fEtaHist_afterCut", "atrack->Eta(), after cut",
                               fNbinsEta, fMinBinEta, fMaxBinEta);
  fEtaHist_afterCut->SetStats(kFALSE);
  fEtaHist_afterCut->SetFillColor(kRed - 10);
  fEtaHist_afterCut->GetXaxis()->SetTitle("#eta");
  fControlHistogramsList->Add(fEtaHist_afterCut);

  // d) Book histogram to hold multiplicity distribution
  fMulHist_beforeCut =
      new TH1F("fMulHist_beforeCut", "Multiplicity, before cut", fNbinsMul,
               fMinBinMul, fMaxBinMul);
  fMulHist_beforeCut->SetStats(kFALSE);
  fMulHist_beforeCut->SetFillColor(kBlue - 10);
  fMulHist_beforeCut->GetXaxis()->SetTitle("Multiplicity");
  fControlHistogramsList->Add(fMulHist_beforeCut);

  fMulHist_afterCut = new TH1F("fMulHist_afterCut", "Multiplicity, after cut",
                               fNbinsMul, fMinBinMul, fMaxBinMul);
  fMulHist_afterCut->SetStats(kFALSE);
  fMulHist_afterCut->SetFillColor(kRed - 10);
  fMulHist_afterCut->GetXaxis()->SetTitle("Multiplicity");
  fControlHistogramsList->Add(fMulHist_afterCut);

  /* e) book histogram holding centrality distribution */
  fCentralityHist_beforeCut =
      new TH1F("fCentralityHist_beforeCut", "Centrality Bins, before cut",
               fNbinsCentrality, fMinCentrality, fMaxCentrality);
  fCentralityHist_beforeCut->SetStats(kFALSE);
  fCentralityHist_beforeCut->SetFillColor(kBlue - 10);
  fCentralityHist_beforeCut->GetXaxis()->SetTitle("Centrality Bins");
  fControlHistogramsList->Add(fCentralityHist_beforeCut);

  fCentralityHist_afterCut =
      new TH1F("fCentralityHist_afterCut", "Centrality Bins, after cut",
               fNbinsCentrality, fMinCentrality, fMaxCentrality);
  fCentralityHist_afterCut->SetStats(kFALSE);
  fCentralityHist_afterCut->SetFillColor(kRed - 10);
  fCentralityHist_afterCut->GetXaxis()->SetTitle("Centrality Bins");
  fControlHistogramsList->Add(fCentralityHist_afterCut);

} // void AliAnalysisTaskForStudents::BookControlHistograms()

//=======================================================================================================================

void AliAnalysisTaskForStudents::BookFinalResultsHistograms() {
  // Book all histograms to hold the final results.
  fAveragePhiHist =
      new TH1F("fAveragePhiHist", "Average #varphi", fNbinsAveragePhi,
               fMinBinAveragePhi, fMaxBinAveragePhi);
  fAveragePhiHist->SetStats(kFALSE);
  fAveragePhiHist->SetFillColor(kBlue - 10);
  fFinalResultsList->Add(fAveragePhiHist);
} // void AliAnalysisTaskForStudents::BookFinalResultsHistograms()

//=======================================================================================================================
//
void AliAnalysisTaskForStudents::GetPointers(TList *histList) {

  TString sMethodName =
      "void AliAnalysisTaskForStudents::GetPointers(TList *histList)";

  // a) Get pointer for base list fHistList and profile holding internal flags;
  fHistList = histList;
  if (!fHistList) {
    Fatal(sMethodName.Data(), "fHistList is not around today...");
  }

  this->GetPointersForControlHistograms();
  this->GetPointersForOutputHistograms();
}

void AliAnalysisTaskForStudents::GetPointersForControlHistograms() {
  // Get pointers for Control Histograms.

  TString sMethodName =
      "void AliAnalysisTaskForStudents::GetPointersForControlHistograms()";

  // a) Get pointer for fControlHistograms:
  fControlHistogramsList =
      dynamic_cast<TList *>(fHistList->FindObject("ControlHistograms"));
  if (!fControlHistogramsList) {
    Fatal(sMethodName.Data(), "fControlHistogramsList");
  }

  /* b) initalize all control histograms */
  fPtHist_beforeCut = dynamic_cast<TH1F *>(
      fControlHistogramsList->FindObject("fPtHist_beforeCut"));
  if (!fPtHist_beforeCut) {
    Fatal(sMethodName.Data(), "fPtHist_beforeCut");
  }
  fPtHist_afterCut = dynamic_cast<TH1F *>(
      fControlHistogramsList->FindObject("fPtHist_afterCut"));
  if (!fPtHist_afterCut) {
    Fatal(sMethodName.Data(), "fPtHist_afterCut");
  }
  fEtaHist_beforeCut = dynamic_cast<TH1F *>(
      fControlHistogramsList->FindObject("fEtaHist_beforeCut"));
  if (!fEtaHist_beforeCut) {
    Fatal(sMethodName.Data(), "fEtaHist_beforeCut");
  }
  fEtaHist_afterCut = dynamic_cast<TH1F *>(
      fControlHistogramsList->FindObject("fEtaHist_afterCut"));
  if (!fEtaHist_afterCut) {
    Fatal(sMethodName.Data(), "fEtaHist_afterCut");
  }
  fPhiHist_beforeCut = dynamic_cast<TH1F *>(
      fControlHistogramsList->FindObject("fPhiHist_beforeCut"));
  if (!fPhiHist_beforeCut) {
    Fatal(sMethodName.Data(), "fPhiHist_beforeCut");
  }
  fPhiHist_afterCut = dynamic_cast<TH1F *>(
      fControlHistogramsList->FindObject("fPhiHist_afterCut"));
  if (!fPhiHist_afterCut) {
    Fatal(sMethodName.Data(), "fPhiHist_afterCut");
  }
  fMulHist_beforeCut = dynamic_cast<TH1F *>(
      fControlHistogramsList->FindObject("fMulHist_beforeCut"));
  if (!fMulHist_beforeCut) {
    Fatal(sMethodName.Data(), "fMulHist");
  }
  fMulHist_afterCut = dynamic_cast<TH1F *>(
      fControlHistogramsList->FindObject("fMulHist_afterCut"));
  if (!fMulHist_afterCut) {
    Fatal(sMethodName.Data(), "fMulHist");
  }
  // c) Set again all flags:
  /* fFillBuffers = (Bool_t)fBuffersFlagsPro->GetBinContent(1); */
  /* fMaxBuffer = fBuffersFlagsPro->GetBinContent(2); */
}

void AliAnalysisTaskForStudents::GetPointersForOutputHistograms() {
  // Get pointers for Output Histograms.

  TString sMethodName =
      "void AliAnalysisTaskForStudents::GetPointersForOutputHistograms()";

  // a) Get pointer for fFinalResultsList:
  fFinalResultsList =
      dynamic_cast<TList *>(fHistList->FindObject("FinalResults"));
  if (!fControlHistogramsList) {
    Fatal(sMethodName.Data(), "fFinalResultsList");
  }

  /* b) get pointer for output histograms */
  fAveragePhiHist =
      dynamic_cast<TH1F *>(fFinalResultsList->FindObject("fAveragePhiHist"));
  if (!fAveragePhiHist) {
    Fatal(sMethodName.Data(), "fAveragePhiHist");
  }
  // c) Set again all flags:
  /* fFillBuffers = (Bool_t)fBuffersFlagsPro->GetBinContent(1); */
  /* fMaxBuffer = fBuffersFlagsPro->GetBinContent(2); */
}
