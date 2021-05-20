/**
 * File              : AliAnalysisTaskForStudents.cxx
 * Author            : Anton Riedel <anton.riedel@tum.de>
 * Date              : 07.05.2021
 * Last Modified Date: 20.05.2021
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
#include "TColor.h"
#include "TFile.h"

using std::cout;
using std::endl;

ClassImp(AliAnalysisTaskForStudents)

    //================================================================================================================

    AliAnalysisTaskForStudents::AliAnalysisTaskForStudents(
        const char *name, Bool_t useParticleWeights)
    : AliAnalysisTaskSE(name),
      /* Control histograms */
      /* Base list for all output objects*/
      fHistList(NULL),
      /* list holding all control histograms */
      fControlHistogramsList(NULL),
      /* control histogram for pt */
      fPtHist_beforeCut(NULL), fPtHist_afterCut(NULL), fNbinsPt(1000),
      fMinBinPt(0.), fMaxBinPt(10.),
      /* control histogram for phi */
      fPhiHist_beforeCut(NULL), fPhiHist_afterCut(NULL), fNbinsPhi(1000),
      fMinBinPhi(0.), fMaxBinPhi(10.),
      /* control histogram for eta */
      fEtaHist_beforeCut(NULL), fEtaHist_afterCut(NULL), fNbinsEta(1000),
      fMinBinEta(0.), fMaxBinEta(10.),
      /* control histogram for multiplicity */
      fMulHist_beforeCut(NULL), fMulHist_afterCut(NULL), fNbinsMul(1000),
      fMinBinMul(0.), fMaxBinMul(10.),
      /* control histogram for centrality */
      fCentralityHist_beforeCut(NULL), fCentralityHist_afterCut(NULL),
      fNbinsCentrality(1000), fMinCentrality(0), fMaxCentrality(10),
      // cuts
      fCentralitySelCriterion(""), fPtMin(0), fPtMax(10), fPhiMin(0),
      fPhiMax(10), fEtaMin(0), fEtaMax(10), fPrimayVertexXMin(0),
      fPrimayVertexXMax(10), fPrimayVertexYMin(0), fPrimayVertexYMax(10),
      fPrimayVertexZMin(0), fPrimayVertexZMax(10), fFilterbit(0),
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
    : AliAnalysisTaskSE(),
      /* Control histograms */
      /* Base list for all output objects*/
      fHistList(NULL),
      /* list holding all control histograms */
      fControlHistogramsList(NULL),
      /* control histogram for pt */
      fPtHist_beforeCut(NULL), fPtHist_afterCut(NULL), fNbinsPt(1000),
      fMinBinPt(0.), fMaxBinPt(10.),
      /* control histogram for phi */
      fPhiHist_beforeCut(NULL), fPhiHist_afterCut(NULL), fNbinsPhi(1000),
      fMinBinPhi(0.), fMaxBinPhi(10.),
      /* control histogram for eta */
      fEtaHist_beforeCut(NULL), fEtaHist_afterCut(NULL), fNbinsEta(1000),
      fMinBinEta(0.), fMaxBinEta(10.),
      /* control histogram for multiplicity */
      fMulHist_beforeCut(NULL), fMulHist_afterCut(NULL), fNbinsMul(1000),
      fMinBinMul(0.), fMaxBinMul(10.),
      /* control histogram for centrality */
      fCentralityHist_beforeCut(NULL), fCentralityHist_afterCut(NULL),
      fNbinsCentrality(1000), fMinCentrality(0), fMaxCentrality(10),
      // cuts
      fCentralitySelCriterion(""), fPtMin(0), fPtMax(10), fPhiMin(0),
      fPhiMax(10), fEtaMin(0), fEtaMax(10), fPrimayVertexXMin(0),
      fPrimayVertexXMax(10), fPrimayVertexYMin(0), fPrimayVertexYMax(10),
      fPrimayVertexZMin(0), fPrimayVertexZMax(10), fFilterbit(0),
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

  fCentralityHist_beforeCut->Fill(
      dynamic_cast<AliMultSelection*>(aAOD->FindListObject("MultSelection"))
          ->GetMultiplicityPercentile(fCentralitySelCriterion));

  if (!SurviveEventCut(aAOD)) {
    return;
  }

  fCentralityHist_afterCut->Fill(
      dynamic_cast<AliMultSelection*>(aAOD->FindListObject("MultSelection"))
          ->GetMultiplicityPercentile(fCentralitySelCriterion));

  /* Start analysis over AODs: */
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

    /* get kinematic variables of the track */
    Double_t pt = aTrack->Pt();   // Pt
    Double_t phi = aTrack->Phi(); // azimuthal angle
    Double_t eta = aTrack->Eta(); // pseudorapidity

    /* fill control histograms before cutting */
    fPtHist_beforeCut->Fill(pt);   // filling pt distribution
    fPhiHist_beforeCut->Fill(phi); // filling phi distriubtion
    fEtaHist_beforeCut->Fill(eta); // filling eta distribution
    nTracks_beforeCut++;

    if (!SurviveTrackCut(aTrack)) {
      /* fill control histograms after cutting */
      fPtHist_afterCut->Fill(pt);   // filling pt distribution
      fPhiHist_afterCut->Fill(phi); // filling phi distriubtion
      fEtaHist_afterCut->Fill(eta); // filling eta distribution
      nTracks_afterCut++;
    }
  }

  /* fill control histogram for Multiplicity after counting all tracks */
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
  /* Book all control histograms */

  Color_t colorBeforeCut = kRed;
  Color_t colorAfterCut = kGreen;

  /* Book histograms to hold pt spectra before and after cutting */
  fPtHist_beforeCut = new TH1F("fPtHist_beforeCut", "atrack->Pt(), before cut",
                               fNbinsPt, fMinBinPt, fMaxBinPt);
  fPtHist_beforeCut->SetStats(kFALSE);
  fPtHist_beforeCut->SetFillColor(colorBeforeCut);
  fPtHist_beforeCut->GetXaxis()->SetTitle("p_{t}");
  fControlHistogramsList->Add(fPtHist_beforeCut);

  fPtHist_afterCut =
      dynamic_cast<TH1F *>(fPtHist_beforeCut->Clone("fPtHist_afterCut"));
  fPtHist_afterCut->SetTitle("atrack->Pt(), after cut");
  fPtHist_afterCut->SetFillColor(colorAfterCut);
  fControlHistogramsList->Add(fPtHist_afterCut);

  /* Book histograms to hold phi distribution before and after cutting */
  fPhiHist_beforeCut =
      new TH1F("fPhiHist_beforeCut", "atrack->Phi(), before cut", fNbinsPhi,
               fMinBinPhi, fMaxBinPhi);
  fPhiHist_beforeCut->SetStats(kFALSE);
  fPhiHist_beforeCut->SetFillColor(colorBeforeCut);
  fPhiHist_beforeCut->GetXaxis()->SetTitle("#varphi");
  fControlHistogramsList->Add(fPhiHist_beforeCut);

  fPhiHist_afterCut =
      dynamic_cast<TH1F *>(fPhiHist_beforeCut->Clone("fPhiHist_afterCut"));
  fPhiHist_afterCut->SetTitle("atrack->Phi, after cut");
  fPhiHist_afterCut->SetFillColor(colorAfterCut);
  fControlHistogramsList->Add(fPhiHist_afterCut);

  /* Book histograms to hold eta distribution before and after cutting */
  fEtaHist_beforeCut =
      new TH1F("fEtaHist_beforeCut", "atrack->Eta(), before cut", fNbinsEta,
               fMinBinEta, fMaxBinEta);
  fEtaHist_beforeCut->SetStats(kFALSE);
  fEtaHist_beforeCut->SetFillColor(colorBeforeCut);
  fEtaHist_beforeCut->GetXaxis()->SetTitle("#eta");
  fControlHistogramsList->Add(fEtaHist_beforeCut);

  fEtaHist_afterCut =
      dynamic_cast<TH1F *>(fEtaHist_beforeCut->Clone("fEtaHist_afterCut"));
  fEtaHist_afterCut->SetTitle("atrack->Eta(), after cut");
  fEtaHist_afterCut->SetFillColor(colorAfterCut);
  fControlHistogramsList->Add(fEtaHist_afterCut);

  /* Book histograms to hold multiplicity distribution before and after cutting
   */
  fMulHist_beforeCut =
      new TH1F("fMulHist_beforeCut", "Multiplicity, before cut", fNbinsMul,
               fMinBinMul, fMaxBinMul);
  fMulHist_beforeCut->SetStats(kFALSE);
  fMulHist_beforeCut->SetFillColor(colorBeforeCut);
  fMulHist_beforeCut->GetXaxis()->SetTitle("Multiplicity");
  fControlHistogramsList->Add(fMulHist_beforeCut);

  fMulHist_afterCut =
      dynamic_cast<TH1F *>(fMulHist_beforeCut->Clone("fMulHist_afterCut"));
  fMulHist_afterCut->SetTitle("Multiplicity, after cut");
  fMulHist_afterCut->SetFillColor(colorAfterCut);
  fControlHistogramsList->Add(fMulHist_afterCut);

  /* book histograms holding centrality distribution before and after cutting */
  fCentralityHist_beforeCut =
      new TH1F("fCentralityHist_beforeCut", "Centrality, before cut",
               fNbinsCentrality, fMinCentrality, fMaxCentrality);
  fCentralityHist_beforeCut->SetStats(kFALSE);
  fCentralityHist_beforeCut->SetFillColor(colorBeforeCut);
  fCentralityHist_beforeCut->GetXaxis()->SetTitle("Centrality");
  fControlHistogramsList->Add(fCentralityHist_beforeCut);

  fCentralityHist_afterCut = dynamic_cast<TH1F *>(
      fCentralityHist_beforeCut->Clone("fCentralityHist_afterCut"));
  fCentralityHist_afterCut->SetTitle("Centrality, after cut");
  fCentralityHist_afterCut->SetFillColor(colorAfterCut);
  fControlHistogramsList->Add(fCentralityHist_afterCut);
}

void AliAnalysisTaskForStudents::BookFinalResultsHistograms() {
  /* Book all histograms to hold the final results. */
  Color_t colorFinalResult = kBlue;
  fAveragePhiHist =
      new TH1F("fAveragePhiHist", "Average #varphi", fNbinsAveragePhi,
               fMinBinAveragePhi, fMaxBinAveragePhi);
  fAveragePhiHist->SetStats(kFALSE);
  fAveragePhiHist->SetFillColor(colorFinalResult);
  fFinalResultsList->Add(fAveragePhiHist);
}

Bool_t AliAnalysisTaskForStudents::SurviveEventCut(AliVEvent *ave) {

  /* Check if the current event survives event cuts */

  /* Determine Ali{MC,ESD,AOD}Event: */
  /* AliMCEvent *aMC = dynamic_cast<AliMCEvent*>(ave); */
  /* AliESDEvent *aESD = dynamic_cast<AliESDEvent*>(ave); */
  /* get object for determining centrality */

  /* cast into AOD event */
  AliAODEvent *aAOD = dynamic_cast<AliAODEvent *>(ave);
  if (!aAOD) {
    return kFALSE;
  }

  /* get centrality percentile */
  AliMultSelection *ams =
      (AliMultSelection *)aAOD->FindListObject("MultSelection");
  if (!ams) {
    return kFALSE;
  }
  Float_t MultiplicityPercentile =
      ams->GetMultiplicityPercentile(fCentralitySelCriterion);

  /* cut event if it is not within the centrality percentile */
  if (MultiplicityPercentile <= fMinCentrality) {
    return kFALSE;
  }
  if (MultiplicityPercentile > fMaxCentrality) {
    return kFALSE;
  }

  /* Get primary vertex */
  AliAODVertex *PrimaryVertex = aAOD->GetPrimaryVertex();
  if (!PrimaryVertex) {
    return kFALSE;
  }

  // cut event if vertex is too displaced
  if (PrimaryVertex->GetX() < fPrimayVertexXMin) {
    return kFALSE;
  }
  if (PrimaryVertex->GetX() > fPrimayVertexXMax) {
    return kFALSE;
  }
  if (PrimaryVertex->GetY() < fPrimayVertexYMin) {
    return kFALSE;
  }
  if (PrimaryVertex->GetY() > fPrimayVertexYMax) {
    return kFALSE;
  }
  if (PrimaryVertex->GetZ() < fPrimayVertexZMin) {
    return kFALSE;
  }
  if (PrimaryVertex->GetZ() > fPrimayVertexZMax) {
    return kFALSE;
  }

  return kTRUE;
}

Bool_t AliAnalysisTaskForStudents::SurviveTrackCut(AliAODTrack *aTrack) {
  /* check if current track survives track cut */

  /* cut pt */
  if (aTrack->Pt() < fPtMin) {
    return kFALSE;
  }
  if (aTrack->Pt() > fPtMax) {
    return kFALSE;
  }
  /* cut phi */
  if (aTrack->Phi() < fPhiMin) {
    return kFALSE;
  }
  if (aTrack->Eta() > fPhiMax) {
    return kFALSE;
  }
  /* cut eta */
  if (aTrack->Eta() < fEtaMin) {
    return kFALSE;
  }
  if (aTrack->Eta() > fEtaMax) {
    return kFALSE;
  }

  /* cut with filtertbit */
  /* filter bit 128 denotes TPC-only tracks, use only them for the */
  /* analysis */
  /* for hybrid tracks use filterbit 782 */
  /* for more information about filterbits see the online week */
  /* the filterbits can change from run to run */
  /* fill control histograms */
  if (aTrack->TestFilterBit(fFilterbit)) {
    return kFALSE;
  }

  return kTRUE;
}

void AliAnalysisTaskForStudents::GetPointers(TList *histList) {

  TString sMethodName =
      "void AliAnalysisTaskForStudents::GetPointers(TList *histList)";

  /* Initialize pointer for base list fHistList and profile holding internal
   * flags */
  fHistList = histList;
  if (!fHistList) {
    Fatal(sMethodName.Data(), "fHistList is not around today...");
  }

  this->GetPointersForControlHistograms();
  this->GetPointersForOutputHistograms();
}

void AliAnalysisTaskForStudents::GetPointersForControlHistograms() {

  /* Get pointers for Control Histograms. */
  TString sMethodName =
      "void AliAnalysisTaskForStudents::GetPointersForControlHistograms()";

  /* Get pointer for fControlHistograms: */
  fControlHistogramsList =
      dynamic_cast<TList *>(fHistList->FindObject("ControlHistograms"));
  if (!fControlHistogramsList) {
    Fatal(sMethodName.Data(), "fControlHistogramsList");
  }

  /* initalize all control histograms */
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
  /* Set again all flags: */
  /* fFillBuffers = (Bool_t)fBuffersFlagsPro->GetBinContent(1); */
  /* fMaxBuffer = fBuffersFlagsPro->GetBinContent(2); */
}

void AliAnalysisTaskForStudents::GetPointersForOutputHistograms() {
  /* Get pointers for Output Histograms. */

  TString sMethodName =
      "void AliAnalysisTaskForStudents::GetPointersForOutputHistograms()";

  /* Get pointer for fFinalResultsList: */
  fFinalResultsList =
      dynamic_cast<TList *>(fHistList->FindObject("FinalResults"));
  if (!fControlHistogramsList) {
    Fatal(sMethodName.Data(), "fFinalResultsList");
  }

  /* get pointer for output histograms */
  fAveragePhiHist =
      dynamic_cast<TH1F *>(fFinalResultsList->FindObject("fAveragePhiHist"));
  if (!fAveragePhiHist) {
    Fatal(sMethodName.Data(), "fAveragePhiHist");
  }
  /* Set again all flags: */
  /* fFillBuffers = (Bool_t)fBuffersFlagsPro->GetBinContent(1); */
  /* fMaxBuffer = fBuffersFlagsPro->GetBinContent(2); */
}
