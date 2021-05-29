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
      fHistList(nullptr), fHistListName("outputStudentAnalysis"),
      /* list holding all control histograms */
      fControlHistogramsList(nullptr),
      fControlHistogramsListName("ControlHistograms"),
      /* cuts */
      fCentralitySelCriterion("V0M"), fFilterbit(128),
      /* Final results */
      fFinalResultsList(nullptr), fFinalResultsListName("FinalResults") {
  /* Constructor */

  AliDebug(2, "AliAnalysisTaskForStudents::AliAnalysisTaskForStudents(const "
              "char *name, Bool_t useParticleWeights)");

  /* Base list */
  fHistList = new TList();
  fHistList->SetName(fHistListName);
  fHistList->SetOwner(kTRUE);

  /* Initialize all arrays */
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
    /* TBI */
  }
}

AliAnalysisTaskForStudents::AliAnalysisTaskForStudents()
    : AliAnalysisTaskSE(),
      /* Base list for all output objects*/
      fHistList(nullptr), fHistListName("outputStudentAnalysis"),
      /* list holding all control histograms */
      fControlHistogramsList(nullptr),
      fControlHistogramsListName("ControlHistograms"),
      /* cuts */
      fCentralitySelCriterion("V0M"), fFilterbit(128),
      /* Final results */
      fFinalResultsList(nullptr), fFinalResultsListName("FinalResults") {
  /* Dummy constructor */

  /* initialze arrays in dummy constructor !!!! */
  this->InitializeArrays();

  AliDebug(2, "AliAnalysisTaskForStudents::AliAnalysisTaskForStudents()");
}

AliAnalysisTaskForStudents::~AliAnalysisTaskForStudents() {
  /* Destructor */

  /* fHlist owns all other data members, if we delete it, we will recursively
   * delete all other objects associative with this object */
  if (fHistList)
    delete fHistList;
}

void AliAnalysisTaskForStudents::UserCreateOutputObjects() {
  /* Called at every worker node to initialize. */

  /* 1) Trick to avoid name clashes, part 1; */
  /* 2) Book and nest all lists; */
  /* 3) Book all objects; */
  /* *) Trick to avoid name clashes, part 2. */

  /* 1) Trick to avoid name clashes, part 1 */
  Bool_t oldHistAddStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  /* 2) Book and nest all lists */
  this->BookAndNestAllLists();

  /* 3) Book all objects */
  this->BookControlHistograms();
  this->BookFinalResultsHistograms();

  // *) Trick to avoid name clashes, part 2:
  TH1::AddDirectory(oldHistAddStatus);

  PostData(1, fHistList);
}

void AliAnalysisTaskForStudents::UserExec(Option_t *) {
  /* Main loop (called for each event) */
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

  /* get centrality percentile */
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

  /* count number of valid tracks before and after cutting for computing
   * multiplicity */
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
    Double_t pt = aTrack->Pt();
    Double_t phi = aTrack->Phi();
    Double_t eta = aTrack->Eta();

    /* fill track control histograms before cutting */
    fTrackControlHistograms[PT][BEFORE]->Fill(pt);
    fTrackControlHistograms[PHI][BEFORE]->Fill(phi);
    fTrackControlHistograms[ETA][BEFORE]->Fill(eta);
    nTracks_beforeCut++;

    /* cut track */
    if (!SurviveTrackCut(aTrack)) {
      continue;
    }

    /* fill track control histograms after cutting */
    fTrackControlHistograms[PT][AFTER]->Fill(pt);
    fTrackControlHistograms[PHI][AFTER]->Fill(phi);
    fTrackControlHistograms[ETA][AFTER]->Fill(eta);
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
}

void AliAnalysisTaskForStudents::Terminate(Option_t *) {
  /* Accessing the merged output list for final compution or for off-line
   * computations (i.e. after merging)*/

  /* fHistList = (TList *)GetOutputData(1); */
  if (!fHistList) {
    std::cout << __LINE__ << ": Did not get " << fHistListName << std::endl;
  }

  // Do some calculation in offline mode here:

  // ... your code for offline calculations ...

  /* get average value of phi and write it into its own histogram */
  fFinalResultHistograms[PHIAVG]->SetBinContent(
      1, fTrackControlHistograms[PHI][AFTER]->GetMean());
}

void AliAnalysisTaskForStudents::InitializeArrays() {
  /* Initialize all data members which are arrays in this method */
  InitializeArraysForTrackControlHistograms();
  InitializeArraysForEventControlHistograms();
  InitializeArraysForCuts();
  InitializeArraysForFinalResultHistograms();
}

void AliAnalysisTaskForStudents::InitializeArraysForTrackControlHistograms() {
  /* initialize array of track control histograms */
  for (int var = 0; var < LAST_ETRACK; ++var) {
    for (int ba = 0; ba < LAST_EBEFOREAFTER; ++ba) {
      fTrackControlHistograms[var][ba] = nullptr;
    }
  }

  /* names for track control histograms */
  TString
      TrackControlHistogramNames[LAST_ETRACK][LAST_EBEFOREAFTER][LAST_ENAME] = {
          {
              // NAME, TITLE, XAXIS
              {"fTrackControlHistograms[PT][BEFORE]", "pT, before cut",
               "p_{T}"}, // BEFORE
              {"fTrackControlHistograms[PT][AFTER]", "pT, after cut",
               "p_{T}"}, // AFTER
          },             // PT

          {
              {"fTrackControlHistograms[PHI][BEFORE]", "#varphi, before cut",
               "#varphi"}, // BEFORE
              {"fTrackControlHistograms[PHI][AFTER]", "#varphi, after cut",
               "#varphi"}, // AFTER
          },               // PHI
          {
              {"fTrackControlHistograms[ETA][BEFORE]", "#eta, before cut",
               "#eta"}, // BEFORE
              {"fTrackControlHistograms[ETA][AFTER]", "#eta, after cut",
               "#eta"}, // AFTER
          },            // ETA
      };
  /* initialize names for track control histograms */
  for (int var = 0; var < LAST_ETRACK; ++var) {
    for (int ba = 0; ba < LAST_EBEFOREAFTER; ++ba) {
      for (int name = 0; name < LAST_ENAME; ++name) {
        fTrackControlHistogramNames[var][ba][name] =
            TrackControlHistogramNames[var][ba][name];
        std::cout << TrackControlHistogramNames[var][ba][name] << std::endl;
      }
    }
  }

  /* default bins for track control histograms */
  Double_t BinsTrackControlHistogramDefaults[LAST_ETRACK][LAST_EBINS] = {
      // BIN LEDGE UEDGE
      {100., 0., 10.},            // PT
      {360., 0., TMath::TwoPi()}, // PHI
      {200., -2., 2.}             // ETA
  };
  /* initialize array of bins and edges for track control histograms */
  for (int var = 0; var < LAST_ETRACK; ++var) {
    for (int bin = 0; bin < LAST_EBINS; ++bin) {
      fBinsTrackControlHistograms[var][bin] =
          BinsTrackControlHistogramDefaults[var][bin];
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

  /* name of event control histograms */
  TString
      EventControlHistogramNames[LAST_ETRACK][LAST_EBEFOREAFTER][LAST_ENAME] = {
          {
              // NAME, TITLE, XAXIS
              {"fEventControlHistograms[CEN][BEFORE]", "centrality, before cut",
               "Centrality Percentile"}, // BEFORE
              {"fEventControlHistograms[CEN][AFTER]", "centrality, after cut",
               "Centrality Percentile"}, // AFTER
          },                             // CEN
          {
              {"fEventControlHistograms[MUL][BEFORE]",
               "multiplicity, before cut", "M"}, // BEFORE
              {"fEventControlHistograms[MUL][AFTER]", "multiplicity, after cut",
               "M"}, // AFTER
          },         // MUL
      };
  /* initialize names for event control histograms */
  for (int var = 0; var < LAST_EEVENT; ++var) {
    for (int ba = 0; ba < LAST_EBEFOREAFTER; ++ba) {
      for (int name = 0; name < LAST_ENAME; ++name) {
        fEventControlHistogramNames[var][ba][name] =
            EventControlHistogramNames[var][ba][name];
        std::cout << EventControlHistogramNames[var][ba][name] << std::endl;
      }
    }
  }

  /* default bins for track control histograms */
  Double_t BinsEventControlHistogramDefaults[LAST_EEVENT][LAST_EBINS] = {
      // BIN LEDGE UEDGE
      {10., 0., 100},     // CEN
      {200., 0., 20000.}, // MUL
  };
  /* initialize array of bins and edges for track control histograms */
  for (int var = 0; var < LAST_EEVENT; ++var) {
    for (int bin = 0; bin < LAST_EBINS; ++bin) {
      fBinsEventControlHistograms[var][bin] =
          BinsEventControlHistogramDefaults[var][bin];
    }
  }
}

void AliAnalysisTaskForStudents::InitializeArraysForCuts() {
  /* initialize all arrays for cuts */

  /* default track cuts */
  Double_t TrackCutDefaults[LAST_ETRACK][LAST_EMINMAX] = {
      // MIN MAX
      {0., 5.},             // PT
      {0., TMath::TwoPi()}, // PHI
      {-3., 3.},            // ETA
  };
  /* initialize array for track cuts */
  for (int var = 0; var < LAST_ETRACK; ++var) {
    for (int mm = 0; mm < LAST_EMINMAX; ++mm) {
      fTrackCuts[var][mm] = TrackCutDefaults[var][mm];
    }
  }

  /* default primary vertex cuts */
  Double_t PrimaryVertexCutDefaults[LAST_EXYZ][LAST_EMINMAX] = {
      // MIN MAX
      {-10., 10.}, // X
      {-10., 10.}, // Y
      {-10., 10.}, // Z
  };
  /* initialize array for track cuts */
  for (int xyz = 0; xyz < LAST_EXYZ; ++xyz) {
    for (int mm = 0; mm < LAST_EMINMAX; ++mm) {
      fPrimaryVertexCuts[xyz][mm] = PrimaryVertexCutDefaults[xyz][mm];
    }
  }
}
>>>>>>> Add InitializeArraysForCuts() method

void AliAnalysisTaskForStudents::InitializeArraysForFinalResultHistograms() {
  /* initialize array for final result histograms */
  for (int var = 0; var < LAST_EFINAL; ++var) {
    fFinalResultHistograms[var] = nullptr;
  }

  TString FinalResultHistogramNames[LAST_EFINAL][LAST_ENAME] = {
      // NAME, TITLE, XAXIS
      {"fFinalResultHistograms[PHIAVG]", "Average #varphi",
       "#varphi"}, // PHIAVG
  };

  /* initialize names for event control histograms */
  for (int var = 0; var < LAST_EFINAL; ++var) {
    for (int name = 0; name < LAST_ENAME; ++name) {
      fFinalResultHistogramNames[var][name] =
          FinalResultHistogramNames[var][name];
    }
  }

  /* default bins for final result histograms */
  Double_t BinsFinalResultHistogramDefaults[LAST_EFINAL][LAST_EBINS] = {
      // BIN LEDGE UEDGE
      {1., 0., 1.}, // AVGPHI
  };
  /* initialize array of bins and edges for track control histograms */
  for (int var = 0; var < LAST_EFINAL; ++var) {
    for (int bin = 0; bin < LAST_EBINS; ++bin) {
      fBinsFinalResultHistograms[var][bin] =
          BinsFinalResultHistogramDefaults[var][bin];
    }
  }
}

void AliAnalysisTaskForStudents::BookAndNestAllLists() {
  /* Book and nest all lists nested in the base list fHistList */

  /* 1. Book and nest lists for control histograms */
  /* 2. Book and nest lists for final results */

  if (!fHistList) {
    std::cout << __LINE__ << ": Did not get " << fHistListName << std::endl;
  }

  /* 1. Book and nest lists for control histograms: */
  fControlHistogramsList = new TList();
  fControlHistogramsList->SetName(fControlHistogramsListName);
  fControlHistogramsList->SetOwner(kTRUE);
  fHistList->Add(fControlHistogramsList);

  /* 2. Book and nest lists for final results: */
  fFinalResultsList = new TList();
  fFinalResultsList->SetName(fFinalResultsListName);
  fFinalResultsList->SetOwner(kTRUE);
  fHistList->Add(fFinalResultsList);
}

void AliAnalysisTaskForStudents::BookControlHistograms() {
  /* Book all control histograms */

  /* fill colors */
  Color_t fillColor[LAST_EBEFOREAFTER] = {kRed - 10, kGreen - 10};

  /* book track control histograms */
  for (int var = 0; var < LAST_ETRACK; ++var) {
    for (int ba = 0; ba < LAST_EBEFOREAFTER; ++ba) {
      fTrackControlHistograms[var][ba] =
          new TH1F(fTrackControlHistogramNames[var][ba][0],
                   fTrackControlHistogramNames[var][ba][1],
                   fBinsTrackControlHistograms[var][BIN],
                   fBinsTrackControlHistograms[var][LEDGE],
                   fBinsTrackControlHistograms[var][UEDGE]);
      fTrackControlHistograms[var][ba]->SetStats(kFALSE);
      fTrackControlHistograms[var][ba]->SetFillColor(fillColor[ba]);
      fTrackControlHistograms[var][ba]->GetXaxis()->SetTitle(
          fTrackControlHistogramNames[var][ba][2]);
      fControlHistogramsList->Add(fTrackControlHistograms[var][ba]);
    }
  }

  /* book event control histograms */
  for (int var = 0; var < LAST_EEVENT; ++var) {
    for (int ba = 0; ba < LAST_EBEFOREAFTER; ++ba) {
      fEventControlHistograms[var][ba] =
          new TH1F(fEventControlHistogramNames[var][ba][0],
                   fEventControlHistogramNames[var][ba][1],
                   fBinsEventControlHistograms[var][BIN],
                   fBinsEventControlHistograms[var][LEDGE],
                   fBinsEventControlHistograms[var][UEDGE]);
      fEventControlHistograms[var][ba]->SetStats(kFALSE);
      fEventControlHistograms[var][ba]->SetFillColor(fillColor[ba]);
      fEventControlHistograms[var][ba]->GetXaxis()->SetTitle(
          fEventControlHistogramNames[var][ba][2]);
      fControlHistogramsList->Add(fEventControlHistograms[var][ba]);
    }
  }
}

void AliAnalysisTaskForStudents::BookFinalResultsHistograms() {

  /* Book all histograms to hold the final results */
  Color_t colorFinalResult = kBlue - 10;

  /* book event control histograms */
  for (int var = 0; var < LAST_EFINAL; ++var) {
    fFinalResultHistograms[var] = new TH1F(
        fFinalResultHistogramNames[var][0], fFinalResultHistogramNames[var][1],
        fBinsFinalResultHistograms[var][BIN],
        fBinsFinalResultHistograms[var][LEDGE],
        fBinsFinalResultHistograms[var][UEDGE]);
    fFinalResultHistograms[var]->SetStats(kFALSE);
    fFinalResultHistograms[var]->SetFillColor(colorFinalResult);
    fFinalResultHistograms[var]->GetXaxis()->SetTitle(
        fFinalResultHistogramNames[var][2]);
    fFinalResultsList->Add(fFinalResultHistograms[var]);
  }
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
  if (MultiplicityPercentile < fBinsEventControlHistograms[CEN][LEDGE])
    return kFALSE;
  if (MultiplicityPercentile > fBinsEventControlHistograms[CEN][UEDGE]) {
    return kFALSE;
  }

  /* Get primary vertex */
  AliAODVertex *PrimaryVertex = aAOD->GetPrimaryVertex();
  if (!PrimaryVertex) {
    return kFALSE;
  }

  // cut event if primary vertex is too out of center
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

  /* cut PT */
  if (aTrack->Pt() < fTrackCuts[PT][MIN]) {
    return kFALSE;
  }
  if (aTrack->Pt() > fTrackCuts[PT][MAX]) {
    return kFALSE;
  }
  /* cut PHI */
  if (aTrack->Phi() < fTrackCuts[PHI][MIN]) {
    return kFALSE;
  }
  if (aTrack->Phi() > fTrackCuts[PHI][MAX]) {
    return kFALSE;
  }
  /* cut ETA */
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
  /* Initialize pointer for base list fHistList so we can initialize all other
   * objects and call terminate off-line*/

  fHistList = histList;
  if (!fHistList) {
    std::cout << __LINE__ << ": Did not get " << fHistListName << std::endl;
  }

  /* initialize all other objects */
  this->GetPointersForControlHistograms();
  this->GetPointersForOutputHistograms();
}

void AliAnalysisTaskForStudents::GetPointersForControlHistograms() {
  /* Get pointers for Control Histograms */

  /* Get pointer for fControlHistograms */
  fControlHistogramsList =
      dynamic_cast<TList *>(fHistList->FindObject(fControlHistogramsListName));
  if (!fControlHistogramsList) {
    std::cout << __LINE__ << ": Did not get " << fControlHistogramsListName
              << std::endl;
  }

  /* get all pointers for track control histograms */
  for (int var = 0; var < LAST_ETRACK; ++var) {
    for (int ba = 0; ba < LAST_EBEFOREAFTER; ++ba) {
      fTrackControlHistograms[var][ba] =
          dynamic_cast<TH1F *>(fControlHistogramsList->FindObject(
              fTrackControlHistogramNames[var][ba][0]));
      if (!fTrackControlHistograms[var][ba]) {
        std::cout << __LINE__ << ": Did not get "
                  << fTrackControlHistogramNames[var][ba][0] << std::endl;
      }
    }
  }

  /* get all pointers for event control histograms */
  for (int var = 0; var < LAST_EEVENT; ++var) {
    for (int ba = 0; ba < LAST_EBEFOREAFTER; ++ba) {
      fEventControlHistograms[var][ba] =
          dynamic_cast<TH1F *>(fControlHistogramsList->FindObject(
              fEventControlHistogramNames[var][ba][0]));
      if (!fEventControlHistograms[var][ba]) {
        std::cout << __LINE__ << ": Did not get "
                  << fEventControlHistogramNames[var][ba][0] << std::endl;
      }
    }
  }
}

void AliAnalysisTaskForStudents::GetPointersForOutputHistograms() {
  /* Get pointers for Output Histograms */

  /* Get pointer for fFinalResultsList */
  fFinalResultsList =
      dynamic_cast<TList *>(fHistList->FindObject(fFinalResultsListName));
  if (!fFinalResultsList) {
    std::cout << __LINE__ << ": Did not get " << fFinalResultsListName
              << std::endl;
  }

  /* get all pointers for final result histograms */
  for (int var = 0; var < LAST_EFINAL; ++var) {
    fFinalResultHistograms[var] = dynamic_cast<TH1F *>(
        fFinalResultsList->FindObject(fFinalResultHistogramNames[var][0]));
    if (!fTrackControlHistograms[var]) {
      std::cout << __LINE__ << ": Did not get "
                << fFinalResultHistogramNames[var][0] << std::endl;
    }
  }

  /* Set again all flags: */
  /* fFillBuffers = (Bool_t)fBuffersFlagsPro->GetBinContent(1); */
  /* fMaxBuffer = fBuffersFlagsPro->GetBinContent(2); */
}
