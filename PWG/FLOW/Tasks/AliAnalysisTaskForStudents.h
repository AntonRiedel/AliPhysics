/**
 * File              : AliAnalysisTaskForStudents.h
 * Author            : Anton Riedel <anton.riedel@tum.de>
 * Date              : 07.05.2021
 * Last Modified Date: 29.05.2021
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
enum eFinal { PHIAVG, LAST_EFINAL };
enum eBins { BIN, LEDGE, UEDGE, LAST_EBINS };
enum eName { NAME, TITLE, XAXIS, LAST_ENAME };
enum eBeforeAfter { BEFORE, AFTER, LAST_EBEFOREAFTER };
enum eMinMax { MIN, MAX, LAST_EMINMAX };
enum eXYZ { X, Y, Z, LAST_EXYZ };

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
    this->fBinsTrackControlHistograms[PT][BIN] = nbins;
    this->fBinsTrackControlHistograms[PT][LEDGE] = min;
    this->fBinsTrackControlHistograms[PT][UEDGE] = max;
  };
  void SetPhiBinning(Int_t nbins, Double_t min, Double_t max) {
    this->fBinsTrackControlHistograms[PHI][BIN] = nbins;
    this->fBinsTrackControlHistograms[PHI][LEDGE] = min;
    this->fBinsTrackControlHistograms[PHI][UEDGE] = max;
  };
  void SetEtaBinning(Int_t nbins, Double_t min, Double_t max) {
    this->fBinsTrackControlHistograms[ETA][BIN] = nbins;
    this->fBinsTrackControlHistograms[ETA][LEDGE] = min;
    this->fBinsTrackControlHistograms[ETA][UEDGE] = max;
  };

  void SetCenBinning(Int_t nbins, Double_t min, Double_t max) {
    this->fBinsEventControlHistograms[CEN][BIN] = nbins;
    this->fBinsEventControlHistograms[CEN][LEDGE] = min;
    this->fBinsEventControlHistograms[CEN][UEDGE] = max;
  };

  void SetMulBinning(Int_t nbins, Double_t min, Double_t max) {
    this->fBinsEventControlHistograms[MUL][BIN] = nbins;
    this->fBinsEventControlHistograms[MUL][LEDGE] = min;
    this->fBinsEventControlHistograms[MUL][UEDGE] = max;
  };

  void SetAveragePhiBinning(Int_t nbins, Double_t min, Double_t max) {
    this->fBinsEventControlHistograms[PHIAVG][BIN] = nbins;
    this->fBinsEventControlHistograms[PHIAVG][LEDGE] = min;
    this->fBinsEventControlHistograms[PHIAVG][UEDGE] = max;
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

  /* base list holding all output object (a.k.a. grandmother of all lists) */
  TList *fHistList;
  TString fHistListName;

  /* control histograms */
  TList *fControlHistogramsList;
  TString fControlHistogramsListName;
  /* array holding control histograms for tracks */
  TH1F *fTrackControlHistograms[LAST_ETRACK][LAST_EBEFOREAFTER];
  /* name of track control histograms */
  TString fTrackControlHistogramNames[LAST_ETRACK][LAST_EBEFOREAFTER]
                                     [LAST_ENAME];
  Double_t fBinsTrackControlHistograms[LAST_ETRACK][LAST_EBINS];
  /* array holding control histograms for events */
  TH1F *fEventControlHistograms[LAST_EEVENT][LAST_EBEFOREAFTER];
  TString fEventControlHistogramNames[LAST_EEVENT][LAST_EBEFOREAFTER]
                                     [LAST_ENAME];
  Double_t fBinsEventControlHistograms[LAST_EEVENT][LAST_EBINS];

  /* cuts */
  TString fCentralitySelCriterion;
  Double_t fTrackCuts[LAST_ETRACK][LAST_EMINMAX];
  Double_t fPrimaryVertexCuts[LAST_EXYZ][LAST_EMINMAX];
  Int_t fFilterbit;

  /* Final results */
  TList *fFinalResultsList;
  TString fFinalResultsListName;

  /* array holding final result histograms */
  TH1F *fFinalResultHistograms[LAST_EFINAL];
  TString fFinalResultHistogramNames[LAST_EFINAL][LAST_ENAME];
  Double_t fBinsFinalResultHistograms[LAST_EFINAL][LAST_EBINS];

  // Increase this counter in each new version:
  ClassDef(AliAnalysisTaskForStudents, 3);
};

#endif
