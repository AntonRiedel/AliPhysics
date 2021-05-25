/**
 * File              : AliAnalysisTaskForStudents.h
 * Author            : Anton Riedel <anton.riedel@tum.de>
 * Date              : 07.05.2021
 * Last Modified Date: 25.05.2021
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

  void SetPtBinning(Int_t const nbins, const Float_t min, const Float_t max) {
    this->fNbinsPt = nbins;
    this->fMinBinPt = min;
    this->fMaxBinPt = max;
  };
  void SetPhiBinning(Int_t const nbins, const Float_t min, const Float_t max) {
    this->fNbinsPhi = nbins;
    this->fMinBinPhi = min;
    this->fMaxBinPhi = max;
  };
  void SetEtaBinning(Int_t const nbins, const Float_t min, const Float_t max) {
    this->fNbinsEta = nbins;
    this->fMinBinEta = min;
    this->fMaxBinEta = max;
  };

  void SetMulBinning(Int_t const nbins, const Float_t min, const Float_t max) {
    this->fNbinsMul = nbins;
    this->fMinBinMul = min;
    this->fMaxBinMul = max;
  };

  void SetAveragePhiBinning(Int_t const nbins, const Float_t min,
                            const Float_t max) {
    this->fNbinsAveragePhi = nbins;
    this->fMinBinAveragePhi = min;
    this->fMaxBinAveragePhi = max;
  };

  void SetCentralitySelCriterion(TString SelCriterion) {
    this->fCentralitySelCriterion = SelCriterion;
  }

  void SetCentralityBinning(Int_t const nbins, Float_t min, Float_t max) {
    this->fNbinsCentrality = nbins;
    this->fMinCentrality = min;
    this->fMaxCentrality = max;
  };

  void SetPtCuts(Float_t ptMin, Float_t ptMax) {
    this->fPtMin = ptMin;
    this->fPtMax = ptMax;
  }

  void SetPhiCuts(Float_t phiMin, Float_t phiMax) {
    this->fPhiMin = phiMin;
    this->fPhiMax = phiMax;
  }

  void SetEtaCuts(Float_t etaMin, Float_t etaMax) {
    this->fEtaMin = etaMin;
    this->fEtaMax = etaMax;
  }

  void SetPrimaryVertexXCuts(Float_t PrimaryVertexXMin,
                             Float_t PrimaryVertexXMax) {
    this->fPrimayVertexXMin = PrimaryVertexXMin;
    this->fPrimayVertexXMax = PrimaryVertexXMax;
  }

  void SetPrimaryVertexYCuts(Float_t PrimaryVertexYMin,
                             Float_t PrimaryVertexYMax) {
    this->fPrimayVertexYMin = PrimaryVertexYMin;
    this->fPrimayVertexYMax = PrimaryVertexYMax;
  }

  void SetPrimaryVertexZCuts(Float_t PrimaryVertexZMin,
                             Float_t PrimaryVertexZMax) {
    this->fPrimayVertexZMin = PrimaryVertexZMin;
    this->fPrimayVertexZMax = PrimaryVertexZMax;
  }

  void SetFilterbit(Int_t Filterbit) { this->fFilterbit = Filterbit; }

private:
  AliAnalysisTaskForStudents(const AliAnalysisTaskForStudents &aatmpf);
  AliAnalysisTaskForStudents &
  operator=(const AliAnalysisTaskForStudents &aatmpf);

  /* base list to hold all output object (a.k.a. grandmother of all lists) */
  TList *fHistList;

  /* control histograms */
  /* list to hold all control histograms */
  TList *fControlHistogramsList;

  /* control histogram for pt */
  TH1F *fPtHist_beforeCut; // atrack->Pt(), before cutting
  TH1F *fPtHist_afterCut;  // atrack->Pt(), after cutting
  Int_t fNbinsPt;          // number of bins
  Float_t fMinBinPt;       // min bin
  Float_t fMaxBinPt;       // max bin

  /* control histogram for phi */
  TH1F *fPhiHist_beforeCut; // atrack->Phi(), before cutting
  TH1F *fPhiHist_afterCut;  // atrack->Phi(), after cutting
  Int_t fNbinsPhi;          // number of bins
  Float_t fMinBinPhi;       // min bin
  Float_t fMaxBinPhi;       // max bin

  /* control histogram for eta */
  TH1F *fEtaHist_beforeCut; // atrack->Eta(), before cutting
  TH1F *fEtaHist_afterCut;  // atrack->Eta(), after cutting
  Int_t fNbinsEta;          // number of bins
  Float_t fMinBinEta;       // min bin
  Float_t fMaxBinEta;       // max bin

  /* control histogram for multiplicity */
  TH1F *fMulHist_beforeCut; // AliAODEvent->GetMultiplicity(), before cutting
  TH1F *fMulHist_afterCut;  // AliAODEvent->GetMultiplicity(), after cutting
  Int_t fNbinsMul;          // number of bins
  Float_t fMinBinMul;       // min bin
  Float_t fMaxBinMul;       // max bin

  /* control histogram for centrality */
  /* (AliMultSelection*)AliAODEvent->FindListObject("MultSelection")->GetMultiplicityPercentile(fCentralitySelCriterion)
   */
  TH1F *fCentralityHist_beforeCut; // before cutting
  TH1F *fCentralityHist_afterCut;  // after cutting
  Int_t fNbinsCentrality;          // number of bins
  Float_t fMinCentrality;          // min centrality
  Float_t fMaxCentrality;          // max centrality

  /* cuts */
  TString fCentralitySelCriterion; // selection criterion for centrality
  /* Float_t fMinCentrality;          // min centrality for cutting, already
   * defined */
  /* Float_t fMaxCentrality;          // max centrality for cutting, already
   * defined */
  Float_t fPtMin;            // minimal pt for cutting
  Float_t fPtMax;            // maximal pt for cutting
  Float_t fPhiMin;           // minimal phi for cutting
  Float_t fPhiMax;           // maximal phi for cutting
  Float_t fEtaMin;           // minimal eta for cutting
  Float_t fEtaMax;           // maximal eta for cutting
  Float_t fPrimayVertexXMin; // minimal x displacement of primary vertex
  Float_t fPrimayVertexXMax; // maximal x displacement of primary vertex
  Float_t fPrimayVertexYMin; // minimal y displacement of primary vertex
  Float_t fPrimayVertexYMax; // maximal y displacement of primary vertex
  Float_t fPrimayVertexZMin; // minimal z displacement of primary vertex
  Float_t fPrimayVertexZMax; // maximal z displacement of primary vertex
  Int_t fFilterbit;          // filterbit

  /* Final results: */
  TList *fFinalResultsList;  // list to hold all histograms with final results
  TH1F *fAveragePhiHist;     // Mean of Phi
  Int_t fNbinsAveragePhi;    // number of bins
  Float_t fMinBinAveragePhi; // min bin
  Float_t fMaxBinAveragePhi; // max bin

  // Increase this counter in each new version:
  ClassDef(AliAnalysisTaskForStudents, 2);
};

#endif
