/**
 * File              : AliAnalysisTaskForStudents.h
 * Author            : Anton Riedel <anton.riedel@tum.de>
 * Date              : 07.05.2021
 * Last Modified Date: 08.05.2021
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

//================================================================================================================

class AliAnalysisTaskForStudents : public AliAnalysisTaskSE {
public:
  AliAnalysisTaskForStudents();
  AliAnalysisTaskForStudents(const char *name,
                             Bool_t useParticleWeights = kFALSE);
  virtual ~AliAnalysisTaskForStudents();
  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t *);
  virtual void Terminate(Option_t *);

  // 0.) Methods called in the constructor:
  virtual void InitializeArrays();

  // 1.) Methods called in UserCreateOutputObjects():
  virtual void BookAndNestAllLists();
  virtual void BookControlHistograms();
  virtual void BookFinalResultsHistograms();

  // 2.) Methods called in UserExec(Option_t *):
  // ...

  // 3.) Setters and getters:
  void SetControlHistogramsList(TList *const chl) {
    this->fControlHistogramsList = chl;
  };
  TList *GetControlHistogramsList() const {
    return this->fControlHistogramsList;
  }
  void SetFinalResultsList(TList *const frl) { this->fFinalResultsList = frl; };
  TList *GetFinalResultsList() const { return this->fFinalResultsList; }

  void SetPtBinning(Int_t const nbins, Float_t min, Float_t max) {
    this->fNbinsPt = nbins;
    this->fMinBinPt = min;
    this->fMaxBinPt = max;
  };
  void SetPhiBinning(Int_t const nbins, Float_t min, Float_t max) {
    this->fNbinsPhi = nbins;
    this->fMinBinPhi = min;
    this->fMaxBinPhi = max;
  };
  void SetEtaBinning(Int_t const nbins, Float_t min, Float_t max) {
    this->fNbinsEta = nbins;
    this->fMinBinEta = min;
    this->fMaxBinEta = max;
  };

  void SetMulBinning(Int_t const nbins, Float_t min, Float_t max) {
    this->fNbinsMul = nbins;
    this->fMinBinMul = min;
    this->fMaxBinMul = max;
  };

  void SetAveragePhiBinning(Int_t const nbins, Float_t min, Float_t max) {
    this->fNbinsAveragePhi = nbins;
    this->fMinBinAveragePhi = min;
    this->fMaxBinAveragePhi = max;
  };

private:
  AliAnalysisTaskForStudents(const AliAnalysisTaskForStudents &aatmpf);
  AliAnalysisTaskForStudents &
  operator=(const AliAnalysisTaskForStudents &aatmpf);

  // 0.) Base lists:
  TList *fHistList; // base list to hold all output object (a.k.a. grandmother
                    // of all lists)

  // 1.) Control histograms:
  TList *fControlHistogramsList; // list to hold all control histograms
  TH1F *fPtHist;                 // atrack->Pt()
  Int_t fNbinsPt;                // number of bins
  Float_t fMinBinPt;             // min bin
  Float_t fMaxBinPt;             // max bin
  TH1F *fPhiHist;                // atrack->Phi()
  Int_t fNbinsPhi;               // number of bins
  Float_t fMinBinPhi;            // min bin
  Float_t fMaxBinPhi;            // max bin
  TH1F *fEtaHist;                // atrack->Eta()
  Int_t fNbinsEta;               // number of bins
  Float_t fMinBinEta;            // min bin
  Float_t fMaxBinEta;            // max bin
  TH1F *fMulHist;                // AliAODEvent->GetMultiplicity()
  Int_t fNbinsMul;               // number of bins
  Float_t fMinBinMul;            // min bin
  Float_t fMaxBinMul;            // max bin

  // 2.) Final results:
  TList *fFinalResultsList;  // list to hold all histograms with final results
  TH1F *fAveragePhiHist;     // Mean of Phi
  Int_t fNbinsAveragePhi;    // number of bins
  Float_t fMinBinAveragePhi; // min bin
  Float_t fMaxBinAveragePhi; // max bin

  // Increase this counter in each new version:
  ClassDef(AliAnalysisTaskForStudents, 1);
};

//================================================================================================================

#endif
