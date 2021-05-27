/**
 * File              : AliAnalysisTaskForStudents.h
 * Author            : Anton Riedel <anton.riedel@tum.de>
 * Date              : 07.05.2021
 * Last Modified Date: 27.05.2021
 * Last Modified By  : Anton Riedel <anton.riedel@tum.de>
 */

/*
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved.
 * See cxx source for full Copyright notice
 * $Id$
 */

/**************************************
 * template class for student projects *
 **************************************/

#ifndef ALIANALYSISTASKFORSTUDENTS_H
#define ALIANALYSISTASKFORSTUDENTS_H

#include "AliAODEvent.h"
#include "AliAODTrack.h"
#include "AliAnalysisTaskSE.h"
#include "AliVEvent.h"
#include "TH1F.h"

/* enumerations */
enum eEvent { CEN, MUL, LAST_EEVENT };
enum eTrack { PT, PHI, ETA, LAST_ETRACK };
enum eBeforeAfter { BEFORE, AFTER, LAST_EBEFOREAFTER };
enum eMinMax { MIN, MAX, LAST_EMINMAX };
enum eXYZ { X, Y, Z, LAST_EXYZ };
enum eFinal { PHIAVG, LAST_EFINAL };

class AliAnalysisTaskForStudents : public AliAnalysisTaskSE {
public:
  AliAnalysisTaskForStudents();
  AliAnalysisTaskForStudents(const char *name,
                             Bool_t useParticleWeights = kFALSE);
  virtual ~AliAnalysisTaskForStudents();
  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t *);
  virtual void Terminate(Option_t *);

  /* Methods called in the constructor: */
  virtual void InitializeArrays();
  virtual void InitializeArraysForTrackControlHistograms();
  virtual void InitializeArraysForEventControlHistograms();
  virtual void InitializeArraysForCuts();
  virtual void InitializeArraysForFinalResultHistograms();

  /* Methods called in UserCreateOutputObjects(): */
  virtual void BookAndNestAllLists();
  virtual void BookControlHistograms();
  virtual void BookFinalResultsHistograms();

  /* Methods called in UserExec(Option_t *): */
  virtual Bool_t SurviveEventCut(AliVEvent *ave);
  virtual Bool_t SurviveTrackCut(AliAODTrack *aTrack);

  /* GetPointers Methods in case we need to manually trigger Terminate() */
  virtual void GetPointers(TList *list);
  virtual void GetPointersForControlHistograms();
  virtual void GetPointersForOutputHistograms();

  /* Setters and getters */
  void SetControlHistogramsList(TList *const chl) {
    this->fControlHistogramsList = chl;
  };
  TList *GetControlHistogramsList() const {
    return this->fControlHistogramsList;
  }
  void SetFinalResultsList(TList *const frl) { this->fFinalResultsList = frl; };
  TList *GetFinalResultsList() const { return this->fFinalResultsList; }

  void SetPtBinning(Int_t nbins, Double_t min, Double_t max) {
    this->fNbinsTrackControlHistograms[PT] = nbins;
    this->fEdgeTrackControlHistograms[PT][MIN] = min;
    this->fEdgeTrackControlHistograms[PT][MAX] = max;
  };
  void SetPhiBinning(Int_t nbins, Double_t min, Double_t max) {
    this->fNbinsTrackControlHistograms[PHI] = nbins;
    this->fEdgeTrackControlHistograms[PHI][MIN] = min;
    this->fEdgeTrackControlHistograms[PHI][MAX] = max;
  };
  void SetEtaBinning(Int_t nbins, Double_t min, Double_t max) {
    this->fNbinsTrackControlHistograms[ETA] = nbins;
    this->fEdgeTrackControlHistograms[ETA][MIN] = min;
    this->fEdgeTrackControlHistograms[ETA][MAX] = max;
  };

  void SetCenBinning(Int_t nbins, Double_t min, Double_t max) {
    this->fNbinsEventControlHistograms[CEN] = nbins;
    this->fEdgeEventControlHistograms[CEN][MIN] = min;
    this->fEdgeEventControlHistograms[CEN][MAX] = max;
  };

  void SetMulBinning(Int_t nbins, Double_t min, Double_t max) {
    this->fNbinsEventControlHistograms[MUL] = nbins;
    this->fEdgeEventControlHistograms[MUL][MIN] = min;
    this->fEdgeEventControlHistograms[MUL][MAX] = max;
  };

  void SetAveragePhiBinning(Int_t nbins, Double_t min, Double_t max) {
    this->fNbinsFinalResultHistograms[PHIAVG] = nbins;
    this->fEdgeFinalResultHistograms[PHIAVG][MIN] = min;
    this->fEdgeFinalResultHistograms[PHIAVG][MAX] = max;
  };

  void SetCentralitySelCriterion(TString SelCriterion) {
    this->fCentralitySelCriterion = SelCriterion;
  }

  void SetPtCuts(Double_t min, Double_t max) {
    this->fTrackCuts[PT][MIN] = min;
    this->fTrackCuts[PT][MAX] = max;
  }

  void SetPhiCuts(Double_t min, Double_t max) {
    this->fTrackCuts[PHI][MIN] = min;
    this->fTrackCuts[PHI][MAX] = max;
  }

  void SetEtaCuts(Double_t min, Double_t max) {
    this->fTrackCuts[ETA][MIN] = min;
    this->fTrackCuts[ETA][MAX] = max;
  }

  void SetPrimaryVertexXCuts(Double_t min, Double_t max) {
    this->fPrimaryVertexCuts[X][MIN] = min;
    this->fPrimaryVertexCuts[X][MAX] = max;
  }

  void SetPrimaryVertexYCuts(Double_t min, Double_t max) {
    this->fPrimaryVertexCuts[Y][MIN] = min;
    this->fPrimaryVertexCuts[Y][MAX] = max;
  }

  void SetPrimaryVertexZCuts(Double_t min, Double_t max) {
    this->fPrimaryVertexCuts[Z][MIN] = min;
    this->fPrimaryVertexCuts[Z][MAX] = max;
  }

  void SetFilterbit(Int_t Filterbit) { this->fFilterbit = Filterbit; }

private:
  AliAnalysisTaskForStudents(const AliAnalysisTaskForStudents &aatmpf);
  AliAnalysisTaskForStudents &
  operator=(const AliAnalysisTaskForStudents &aatmpf);

  /* base list to hold all output object (a.k.a. grandmother of all lists) */
  TList *fHistList;
  TString fHistListName = "outputStudentAnalysis";

  /* control histograms */
  TList *fControlHistogramsList;
  TString fControlHistogramsListName = "ControlHistograms";
  /* array to hold control histograms for tracks */
  TH1F *fTrackControlHistograms[LAST_ETRACK][LAST_EBEFOREAFTER];
  /* name of track control histograms */
  TString fTrackControlHistogramNames[LAST_ETRACK][LAST_EBEFOREAFTER][3] = {
      {
          // Name, Title, Name of Xaxis
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
  Int_t fNbinsTrackControlHistograms[LAST_ETRACK];
  Double_t fEdgeTrackControlHistograms[LAST_ETRACK][LAST_EMINMAX];

  /* array to hold control histograms for events */
  TH1F *fEventControlHistograms[LAST_EEVENT][LAST_EBEFOREAFTER];
  /* name of event control histograms */
  TString fEventControlHistogramNames[LAST_ETRACK][LAST_EBEFOREAFTER][3] = {
      {
          // Name , Title, Name of Xaxis
          {"fEventControlHistograms[CEN][BEFORE]", "centrality, before cut",
           "Centrality Percentile"}, // BEFORE
          {"fEventControlHistograms[CEN][AFTER]", "centrality, after cut",
           "Centrality Percentile"}, // AFTER
      },                             // CEN
      {
          {"fEventControlHistograms[MUL][BEFORE]", "multiplicity, before cut",
           "M"}, // BEFORE
          {"fEventControlHistograms[MUL][AFTER]", "multiplicity, after cut",
           "M"}, // AFTER
      },         // MUL
  };
  Int_t fNbinsEventControlHistograms[LAST_EEVENT];
  Double_t fEdgeEventControlHistograms[LAST_EEVENT][LAST_EMINMAX];

  /* cuts */
  TString fCentralitySelCriterion;
  Double_t fTrackCuts[LAST_ETRACK][LAST_EMINMAX];
  Double_t fPrimaryVertexCuts[LAST_EXYZ][LAST_EMINMAX];
  Int_t fFilterbit;

  /* Final results */
  TList *fFinalResultsList;
  TString fFinalResultsListName = "FinalResults";

  TH1F *fFinalResultHistograms[LAST_EFINAL];
  TString fFinalResultHistogramNames[LAST_EFINAL][3] = {
      // Name , Title, Name of Xaxis
      {"fFinalResultHistograms[PHIAVG]", "Average #varphi",
       "#varphi"}, // PHIAVG
  };
  Int_t fNbinsFinalResultHistograms[LAST_EFINAL];
  Double_t fEdgeFinalResultHistograms[LAST_EFINAL][LAST_EMINMAX]; // min bin

  // Increase this counter in each new version:
  ClassDef(AliAnalysisTaskForStudents, 3);
};

#endif
