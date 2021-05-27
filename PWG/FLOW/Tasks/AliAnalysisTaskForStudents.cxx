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
      /* cuts */
      fCentralitySelCriterion("V0M"), fFilterbit(128),
      /* Final results */
      fFinalResultsList(nullptr) {
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
      fHistList(nullptr),
      /* list holding all control histograms */
      fControlHistogramsList(nullptr),
      /* cuts */
      fCentralitySelCriterion("V0M"), fFilterbit(128),
      /* Final results */
      fFinalResultsList(nullptr) {
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
  /* initialize array of bins and edges for track control histograms */
  for (int var = 0; var < LAST_ETRACK; ++var) {
    fNbinsTrackControlHistograms[var] =
        fNbinsTrackControlHistogramsDefault[var];
    for (int mm = 0; mm < LAST_EMINMAX; ++mm) {
      fEdgeTrackControlHistograms[var][mm] =
          fEdgesTrackControlHistogramDefaults[var][mm];
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
  /* initialize bins and edges for event control histograms */
  for (int var = 0; var < LAST_EEVENT; ++var) {
    fNbinsEventControlHistograms[var] =
        fNbinsEventControlHistogramDefaults[var];
    for (int mm = 0; mm < LAST_EMINMAX; ++mm) {
      fEdgeEventControlHistograms[var][mm] =
          fEdgesEventControlHistogramsDefault[var][mm];
    }
  }
}

void AliAnalysisTaskForStudents::InitializeArraysForCuts() {
  /* initialize all arrays for cuts */
  /* initialize array for track cuts */
  for (int var = 0; var < LAST_ETRACK; ++var) {
    for (int mm = 0; mm < LAST_EMINMAX; ++mm) {
      fTrackCuts[var][mm] = fTrackCutsDefault[var][mm];
    }
  }
  /* initialize array for track cuts */
  for (int xyz = 0; xyz < LAST_EXYZ; ++xyz) {
    for (int mm = 0; mm < LAST_EMINMAX; ++mm) {
      fPrimaryVertexCuts[xyz][mm] = fPrimaryVertexCutsDefault[xyz][mm];
    }
  }
}
>>>>>>> Add InitializeArraysForCuts() method

void AliAnalysisTaskForStudents::InitializeArraysForFinalResultHistograms() {
  /* initialize array for final result histograms */
  for (int var = 0; var < LAST_EFINAL; ++var) {
    fFinalResultHistograms[var] = nullptr;
  }
  /* initialize bins and edges for final result histograms */
  for (int var = 0; var < LAST_EFINAL; ++var) {
    fNbinsFinalResultHistograms[var] = fNbinsFinalResultHistogramDefaults[var];
    for (int mm = 0; mm < LAST_EMINMAX; ++mm) {
      fEdgeFinalResultHistograms[var][mm] =
          fEdgesFinalResultHistogramsDefault[var][mm];
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
                   fNbinsTrackControlHistograms[var],
                   fEdgeTrackControlHistograms[var][MIN],
                   fEdgeTrackControlHistograms[var][MAX]);
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
                   fNbinsEventControlHistograms[var],
                   fEdgeEventControlHistograms[var][MIN],
                   fEdgeEventControlHistograms[var][MAX]);
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
        fNbinsFinalResultHistograms[var], fEdgeFinalResultHistograms[var][MIN],
        fEdgeFinalResultHistograms[var][MAX]);
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
