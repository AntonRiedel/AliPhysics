/**
 * File              : AliAnalysisTaskForStudents.cxx
 * Author            : Anton Riedel <anton.riedel@tum.de>
 * Date              : 07.05.2021
 * Last Modified Date: 04.06.2021
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
#include <ostream>

ClassImp(AliAnalysisTaskForStudents)

    AliAnalysisTaskForStudents::AliAnalysisTaskForStudents(
        const char *name, Bool_t useParticleWeights)
    : AliAnalysisTaskSE(name),
      /* Base list for all output objects*/
      fHistList(nullptr),
      /* list holding all control histograms */
      fControlHistogramsList(nullptr),
      // cuts
      fCentralitySelCriterion(""), fFilterbit(0),
      // Final results:
      fFinalResultsList(nullptr), fAveragePhiHist(nullptr),
      fNbinsAveragePhi(1000), fMinBinAveragePhi(0.), fMaxBinAveragePhi(10.) {
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

/* Dummy constructor */
AliAnalysisTaskForStudents::AliAnalysisTaskForStudents()
    : AliAnalysisTaskSE(),
      /* Base list for all output objects*/
      fHistList(nullptr),
      /* list holding all control histograms */
      fControlHistogramsList(nullptr),
      // cuts
      fCentralitySelCriterion(""), fFilterbit(0),
      // Final results:
      fFinalResultsList(nullptr), fAveragePhiHist(nullptr),
      fNbinsAveragePhi(1000), fMinBinAveragePhi(0.), fMaxBinAveragePhi(10.) {

  this->InitializeArrays();

  AliDebug(2, "AliAnalysisTaskForStudents::AliAnalysisTaskForStudents()");

} // AliAnalysisTaskForStudents::AliAnalysisTaskForStudents():

/* Destructor */
AliAnalysisTaskForStudents::~AliAnalysisTaskForStudents() {

  if (fHistList)
    delete fHistList;

} // AliAnalysisTaskForStudents::~AliAnalysisTaskForStudents()

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

/* Main loop (called for each event) */
void AliAnalysisTaskForStudents::UserExec(Option_t *) {
  /* general strategy */
  /* 1. Get pointer to AOD event */
  /* 2. Start analysis over AODs */
  /* 3. Reset event-by-event objects */
  /* 4. PostData */

  /* 1. Get pointer to AOD event */
  AliAODEvent *aAOD = dynamic_cast<AliAODEvent *>(InputEvent()); // from TaskSE
  if (!aAOD) {
    return;
  }

  Double_t centralityPercentile =
      dynamic_cast<AliMultSelection *>(aAOD->FindListObject("MultSelection"))
          ->GetMultiplicityPercentile(fCentralitySelCriterion);

  /* fill centrality control histgrogram before event cut */
  fEventControlHistograms[CEN][BEFORE]->Fill(centralityPercentile);

  /* cut event */
  if (!SurviveEventCut(aAOD)) {
    return;
  }

  /* fill centrality control histogram after event cut */
  fEventControlHistograms[CEN][AFTER]->Fill(centralityPercentile);

  /* 2. Start analysis over AODs: */

  /* number of all tracks in current event */
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
    Double_t pt = aTrack->Pt();   // transverse momentum
    Double_t phi = aTrack->Phi(); // azimuthal angle
    Double_t eta = aTrack->Eta(); // pseudorapidity

    /* fill track control histograms before cutting */
    fTrackControlHistograms[PT][BEFORE]->Fill(pt);   // filling pt histogram
    fTrackControlHistograms[PHI][BEFORE]->Fill(phi); // filling phi histogram
    fTrackControlHistograms[ETA][BEFORE]->Fill(eta); // filling eta histogram
    nTracks_beforeCut++;

    /* cut track */
    if (!SurviveTrackCut(aTrack)) {
      continue;
    }

    /* fill track control histograms after cutting */
    fTrackControlHistograms[PT][AFTER]->Fill(pt);   // filling pt histogram
    fTrackControlHistograms[PHI][AFTER]->Fill(phi); // filling phi histogram
    fTrackControlHistograms[ETA][AFTER]->Fill(eta); // filling eta histogram
    nTracks_afterCut++;
  }

  /* fill control histogram for Multiplicity after counting all tracks */
  fEventControlHistograms[MUL][BEFORE]->Fill(nTracks_beforeCut);
  fEventControlHistograms[MUL][AFTER]->Fill(nTracks_afterCut);

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

/* Accessing the merged output list */
void AliAnalysisTaskForStudents::Terminate(Option_t *) {

  fHistList = (TList *)GetOutputData(1);
  if (!fHistList) {
    std::cout << "fHistList not around?" << std::endl;
  }

  // Do some calculation in offline mode here:

  // ... your code for offline calculations ...

  fAveragePhiHist->SetBinContent(
      1, fTrackControlHistograms[PHI][AFTER]->GetMean());
}

void AliAnalysisTaskForStudents::InitializeArrays() {
  /* Initialize all data members which are arrays in this method */
  InitializeArraysForTrackControlHistograms();
  InitializeArraysForEventControlHistograms();
  InitializeArraysForCuts();
}

void AliAnalysisTaskForStudents::InitializeArraysForTrackControlHistograms() {
  /* initialize array of track control histograms */
  for (int var = 0; var < LAST_ETRACK; ++var) {
    for (int ba = 0; ba < LAST_EBEFOREAFTER; ++ba) {
      fTrackControlHistograms[var][ba] = nullptr;
    }
  }

  /* default number of bins for track control histograms*/
  Int_t NbinsTrack[LAST_ETRACK] = {
      100, // pt
      360, // phi
      200, // eta
  };

  /* default edges for track control histograms */
  Double_t EdgesTrack[LAST_ETRACK][LAST_EMINMAX] = {
      {0., 10.},            // pt
      {0., TMath::TwoPi()}, // phi
      {-2., 2.}             // eta
  };

  /* initialize array of binning and edges for track control histograms */
  for (int var = 0; var < LAST_ETRACK; ++var) {
    fNbinsTrackControlHistograms[var] = NbinsTrack[var];
    for (int mm = 0; mm < LAST_EMINMAX; ++mm) {
      fEdgeTrackControlHistograms[var][mm] = EdgesTrack[var][mm];
    }
  }
}

void AliAnalysisTaskForStudents::InitializeArraysForEventControlHistograms() {
  /* initialize array of event control histograms */
  for (int var = 0; var < LAST_EEVENT; ++var) {
    for (int ba = 0; ba < LAST_EBEFOREAFTER; ++ba) {
      fEventControlHistograms[var][ba] = nullptr;
    }
  }

  /* default number of bins for event control histograms */
  Int_t NbinsEvent[LAST_EEVENT] = {
      10,   // cen
      1000, // mul
  };

  /* default edges for track control histograms */
  Double_t EdgesEvent[LAST_EEVENT][LAST_EMINMAX] = {
      {0., 100},    // cen
      {0., 20000.}, // mul
  };

  /* initialize binning and edges for track control histograms */
  for (int var = 0; var < LAST_EEVENT; ++var) {
    fNbinsEventControlHistograms[var] = NbinsEvent[var];
    for (int mm = 0; mm < LAST_EMINMAX; ++mm) {
      fEdgeEventControlHistograms[var][mm] = EdgesEvent[var][mm];
    }
  }
}

void AliAnalysisTaskForStudents::InitializeArraysForCuts() {
  /* initialize all arrays for cuts */

  /* default track cuts */
  Double_t TrackCutsDefault[LAST_ETRACK][LAST_EMINMAX] = {
      // MIN MAX
      {0., 5.},             // PT
      {0., TMath::TwoPi()}, // PHI
      {-3., 3.},            // ETA
  };
  /* initialize array for track cuts */
  for (int var = 0; var < LAST_ETRACK; ++var) {
    for (int mm = 0; mm < LAST_EMINMAX; ++mm) {
      fTrackCuts[var][mm] = TrackCutsDefault[var][mm];
    }
  }

  /* default primary vertex cuts */
  Double_t PrimaryVertexCutsDefault[LAST_EXYZ][LAST_EMINMAX] = {
      // MIN MAX
      {-10., 10.}, // X
      {-10., 10.}, // Y
      {-10., 10.}, // Z
  };
  /* initialize array for track cuts */
  for (int xyz = 0; xyz < LAST_EXYZ; ++xyz) {
    for (int mm = 0; mm < LAST_EMINMAX; ++mm) {
      fPrimaryVertexCuts[xyz][mm] = PrimaryVertexCutsDefault[xyz][mm];
    }
  }
}
>>>>>>> Add InitializeArraysForCuts() method

void AliAnalysisTaskForStudents::BookAndNestAllLists() {
  /* Book and nest all lists nested in the base list fHistList. */

  /* 1. Book and nest lists for control histograms; */
  /* 2. Book and nest lists for final results. */

  TString sMethodName =
      "void AliAnalysisTaskForStudents::BookAndNestAllLists()";
  if (!fHistList) {
    Fatal(sMethodName.Data(), "fHistList is nullptr");
  }

  /* 1. Book and nest lists for control histograms: */
  fControlHistogramsList = new TList();
  fControlHistogramsList->SetName("ControlHistograms");
  fControlHistogramsList->SetOwner(kTRUE);
  fHistList->Add(fControlHistogramsList);

  /* 2. Book and nest lists for final results: */
  fFinalResultsList = new TList();
  fFinalResultsList->SetName("FinalResults");
  fFinalResultsList->SetOwner(kTRUE);
  fHistList->Add(fFinalResultsList);

} // void AliAnalysisTaskForStudents::BookAndNestAllLists()

void AliAnalysisTaskForStudents::BookControlHistograms() {
  /* Book all control histograms */

  /* fill colors */
  Color_t fillColor[LAST_EBEFOREAFTER] = {kRed - 10, kGreen - 10};

  /* name of track control histograms */
  const char *TrackNames[LAST_ETRACK][LAST_EBEFOREAFTER][3] = {
      {
          // Name , Title, Name of Xaxis
          {"fTrackControlHistograms[PT][BEFORE]", "pT, before cut", "p_{T}"},
          {"fTrackControlHistograms[PT][AFTER]", "pT, after cut", "p_{T}"},
      },
      {
          {"fTrackControlHistograms[PHI][BEFORE]", "#varphi, before cut",
           "#varphi"},
          {"fTrackControlHistograms[PHI][AFTER]", "#varphi, after cut",
           "#varphi"},
      },
      {
          {"fTrackControlHistograms[ETA][BEFORE]", "#eta, before cut", "#eta"},
          {"fTrackControlHistograms[ETA][AFTER]", "#eta, after cut", "#eta"},
      }};

  /* book track control histograms */
  for (int i = 0; i < LAST_ETRACK; ++i) {
    for (int j = 0; j < LAST_EBEFOREAFTER; ++j) {
      fTrackControlHistograms[i][j] = new TH1F(
          TrackNames[i][j][0], TrackNames[i][j][1],
          fNbinsTrackControlHistograms[i], fEdgeTrackControlHistograms[i][MIN],
          fEdgeTrackControlHistograms[i][MAX]);
      fTrackControlHistograms[i][j]->SetStats(kFALSE);
      fTrackControlHistograms[i][j]->SetFillColor(fillColor[j]);
      fTrackControlHistograms[i][j]->GetXaxis()->SetTitle(TrackNames[i][j][2]);
      fControlHistogramsList->Add(fTrackControlHistograms[i][j]);
    }
  }

  /* name of event control histograms */
  const char *EventNames[LAST_ETRACK][LAST_EBEFOREAFTER][3] = {
      {
          // Name , Title, Name of Xaxis
          {"fEventControlHistograms[CEN][BEFORE]", "centrality, before cut",
           "Centrality Percentile"},
          {"fEventControlHistograms[CEN][AFTER]", "centrality, after cut",
           "Centrality Percentile"},
      },
      {
          {"fEventControlHistograms[MUL][BEFORE]", "multiplicity, before cut",
           "M"},
          {"fEventControlHistograms[MUL][AFTER]", "multiplicity, after cut",
           "M"},
      },
  };

  /* book event control histograms */
  for (int var = 0; var < LAST_EEVENT; ++var) {
    for (int ba = 0; ba < LAST_EBEFOREAFTER; ++ba) {
      fEventControlHistograms[var][ba] =
          new TH1F(EventNames[var][ba][0], EventNames[var][ba][1],
                   fNbinsEventControlHistograms[var],
                   fEdgeEventControlHistograms[var][MIN],
                   fEdgeEventControlHistograms[var][MAX]);
      fEventControlHistograms[var][ba]->SetStats(kFALSE);
      fEventControlHistograms[var][ba]->SetFillColor(fillColor[ba]);
      fEventControlHistograms[var][ba]->GetXaxis()->SetTitle(
          EventNames[var][ba][2]);
      fControlHistogramsList->Add(fEventControlHistograms[var][ba]);
    }
  }
}

void AliAnalysisTaskForStudents::BookFinalResultsHistograms() {

  /* Book all histograms to hold the final results. */
  Color_t colorFinalResult = kBlue - 10;
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
  Double_t MultiplicityPercentile =
      ams->GetMultiplicityPercentile(fCentralitySelCriterion);

  /* cut event if it is not within the centrality percentile */
  /* use edges of the event control histogram for cutting */
  if (MultiplicityPercentile < fEdgeEventControlHistograms[CEN][MIN])
    return kFALSE;
  if (MultiplicityPercentile > fEdgeEventControlHistograms[CEN][MAX]) {
    return kFALSE;
  }

  /* Get primary vertex */
  AliAODVertex *PrimaryVertex = aAOD->GetPrimaryVertex();
  if (!PrimaryVertex) {
    return kFALSE;
  }

  // cut event if vertex is too out of center
  if (PrimaryVertex->GetX() < fPrimaryVertexCuts[X][MIN]) {
    return kFALSE;
  }
  if (PrimaryVertex->GetX() > fPrimaryVertexCuts[X][MAX]) {
    return kFALSE;
  }
  if (PrimaryVertex->GetY() < fPrimaryVertexCuts[Y][MIN]) {
    return kFALSE;
  }
  if (PrimaryVertex->GetY() > fPrimaryVertexCuts[Y][MAX]) {
    return kFALSE;
  }
  if (PrimaryVertex->GetZ() < fPrimaryVertexCuts[Z][MIN]) {
    return kFALSE;
  }
  if (PrimaryVertex->GetZ() > fPrimaryVertexCuts[Z][MAX]) {
    return kFALSE;
  }

  return kTRUE;
}

Bool_t AliAnalysisTaskForStudents::SurviveTrackCut(AliAODTrack *aTrack) {
  /* check if current track survives track cut */

  /* cut pt */
  if (aTrack->Pt() < fTrackCuts[PT][MIN]) {
    return kFALSE;
  }
  if (aTrack->Pt() > fTrackCuts[PT][MAX]) {
    return kFALSE;
  }
  /* cut phi */
  if (aTrack->Phi() < fTrackCuts[PHI][MIN]) {
    return kFALSE;
  }
  if (aTrack->Phi() > fTrackCuts[PHI][MAX]) {
    return kFALSE;
  }
  /* cut eta */
  if (aTrack->Eta() < fTrackCuts[ETA][MIN]) {
    return kFALSE;
  }
  if (aTrack->Eta() > fTrackCuts[ETA][MAX]) {
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

  /* /1* initalize all control histograms *1/ */
  /* fPtHist_beforeCut = dynamic_cast<TH1F *>( */
  /*     fControlHistogramsList->FindObject("fPtHist_beforeCut")); */
  /* if (!fPtHist_beforeCut) { */
  /*   Fatal(sMethodName.Data(), "fPtHist_beforeCut"); */
  /* } */
  /* fPtHist_afterCut = dynamic_cast<TH1F *>( */
  /*     fControlHistogramsList->FindObject("fPtHist_afterCut")); */
  /* if (!fPtHist_afterCut) { */
  /*   Fatal(sMethodName.Data(), "fPtHist_afterCut"); */
  /* } */
  /* fEtaHist_beforeCut = dynamic_cast<TH1F *>( */
  /*     fControlHistogramsList->FindObject("fEtaHist_beforeCut")); */
  /* if (!fEtaHist_beforeCut) { */
  /*   Fatal(sMethodName.Data(), "fEtaHist_beforeCut"); */
  /* } */
  /* fEtaHist_afterCut = dynamic_cast<TH1F *>( */
  /*     fControlHistogramsList->FindObject("fEtaHist_afterCut")); */
  /* if (!fEtaHist_afterCut) { */
  /*   Fatal(sMethodName.Data(), "fEtaHist_afterCut"); */
  /* } */
  /* fPhiHist_beforeCut = dynamic_cast<TH1F *>( */
  /*     fControlHistogramsList->FindObject("fPhiHist_beforeCut")); */
  /* if (!fPhiHist_beforeCut) { */
  /*   Fatal(sMethodName.Data(), "fPhiHist_beforeCut"); */
  /* } */
  /* fPhiHist_afterCut = dynamic_cast<TH1F *>( */
  /*     fControlHistogramsList->FindObject("fPhiHist_afterCut")); */
  /* if (!fPhiHist_afterCut) { */
  /*   Fatal(sMethodName.Data(), "fPhiHist_afterCut"); */
  /* } */
  /* fMulHist_beforeCut = dynamic_cast<TH1F *>( */
  /*     fControlHistogramsList->FindObject("fMulHist_beforeCut")); */
  /* if (!fMulHist_beforeCut) { */
  /*   Fatal(sMethodName.Data(), "fMulHist"); */
  /* } */
  /* fMulHist_afterCut = dynamic_cast<TH1F *>( */
  /*     fControlHistogramsList->FindObject("fMulHist_afterCut")); */
  /* if (!fMulHist_afterCut) { */
  /*   Fatal(sMethodName.Data(), "fMulHist"); */
  /* } */
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
