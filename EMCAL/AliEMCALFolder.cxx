/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
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

/* $Id$ */

//_________________________________________________________________________
// Top EMCAL folder which will keep all information about EMCAL itself,
// super Modules (SM), modules, towers, set of hists and so on.
//
//*-- Author: Aleksei Pavlinov (WSU, Detroit, USA) 

#include "AliEMCALFolder.h"
#include "AliEMCALHistoUtilities.h"
#include "AliEMCALGeometry.h"
#include "AliEMCALSuperModule.h"
#include "AliEMCALCell.h"
#include "AliESDCaloCluster.h"

#include "AliRun.h"

#include "AliEMCALCalibData.h"
#include "AliCDBMetaData.h"
#include "AliCDBId.h"
#include "AliCDBEntry.h"
#include "AliCDBManager.h"
#include "AliCDBStorage.h"

#include "AliEMCALCalibCoefs.h"
#include "AliEMCALDigit.h"
#include "AliEMCALRecPoint.h"

#include "AliEMCALPi0SelectionParam.h"

#include <cassert>

#include <TROOT.h>
#include <TStyle.h>
#include <TList.h>
#include <TH1.h>
#include <TF1.h>
#include <TFile.h>
#include <TFileIter.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TKey.h>
#include <TNtuple.h>
#include <TLegend.h>
#include <TArrayS.h>

  const TString AliEMCALFolder::fgkBaseFolderName("EMCAL");
  const TString AliEMCALFolder::fgkCCFirstName("CCFIRST");
  const TString AliEMCALFolder::fgkCCinName("CCIN");
  const TString AliEMCALFolder::fgkCCoutName("CCOUT");

typedef  AliEMCALHistoUtilities u;

ClassImp(AliEMCALFolder)

//AliEMCALGeometry* AliEMCALFolder::fGeometry = 0;

  TList *lObj = 0;

AliEMCALFolder::AliEMCALFolder() : 
  TObjectSet(), 
  fCounter(0), fGeometry(0), fNumOfCell(0), fLhists(0), fLofCells(0),fPi0SelPar(0),fCalibData(0),
  fCellNtuple(0)
{
}

AliEMCALFolder::AliEMCALFolder(const char* name, const char* title, Bool_t putToBrowser) : 
  TObjectSet(name),
  fCounter(-1), fGeometry(0), fNumOfCell(0), fLhists(0), fLofCells(0),fPi0SelPar(0),fCalibData(0),
  fCellNtuple(0)
{
  SetTitle(title);
  Init(putToBrowser);
}

AliEMCALFolder::AliEMCALFolder(const Int_t it, const char* title, Bool_t putToBrowser) : 
  TObjectSet(Form("%s_%2.2i", AliEMCALFolder::fgkBaseFolderName.Data(),it)),
  fCounter(it), fGeometry(0), fNumOfCell(0), fLhists(0), fLofCells(0),fPi0SelPar(0),fCalibData(0),
  fCellNtuple(0)
{
  SetTitle(title);
  Init(putToBrowser);
}

AliEMCALFolder::~AliEMCALFolder()
{
  // dtor
}

void AliEMCALFolder::Init(Bool_t putToBrowser)
{
  lObj = new TList;
  lObj->SetName("Objects"); // name is good ?
  this->AddObject((TObject*)lObj, kTRUE);
  // Get default geometry - "SHISH_77_TRD1_2X2_FINAL_110DEG"; May 29, 2007
  fGeometry = AliEMCALGeometry::GetInstance(); // should be define before 
  lObj->Add(fGeometry);

  // Initial cc with decalibration
  char* dbString  = "local:///data/r22b/ALICE/PROD/CALIBRATION_May_2007/PI0/PDSF/10GEV/DECALIB/DeCalibDB"; // hard coded now
  fCalibData = (AliEMCALCalibData*)
    (AliCDBManager::Instance()->GetStorage(dbString)
     ->Get("EMCAL/Calib/Data",gAlice->GetRunNumber())->GetObject());
  lObj->Add(fCalibData);
  // Jun 13, 2007
  Add(AliEMCALCalibCoefs::GetCalibTableFromDb(fgkCCFirstName.Data()));
  if(GetIterationNumber()==1) {
    Add(AliEMCALCalibCoefs::GetCalibTableFromDb()); // first iteration only
  }
  // Selection Parameter
  fPi0SelPar = AliEMCALPi0SelectionParam::Set1();
  this->Add(fPi0SelPar);
  //
  fLhists = BookHists();
  lObj->Add(fLhists);

  // dimension should be get from geometry - 4*12*24*11); 
  fNumOfCell = fGeometry->GetNCells();
  fLofCells  = new AliEMCALCell*[fNumOfCell];
  for(int i=0; i<fNumOfCell; i++) fLofCells[i] = 0;

  printf("<I> Create AliEMCALFolder : it %i : name %s\n ", fCounter, GetName());

  if(putToBrowser) gROOT->GetListOfBrowsables()->Add(this); // for testing purpuse
} 

AliEMCALSuperModule* AliEMCALFolder::GetSuperModule(const Int_t nm)
{
  TDataSet *set = 0; 
  AliEMCALSuperModule *sm = 0;

  set = FindByName(Form("SM%2.2i",nm));
  if(set) sm = (AliEMCALSuperModule*)set;

  return sm;
}


AliEMCALCell* AliEMCALFolder::GetCell(const Int_t absId)
{ // May 30, 2007
  if(absId<0 || absId >= fNumOfCell) return 0;
  else return fLofCells[absId];
}  

void  AliEMCALFolder::SetCell(AliEMCALCell *cell, const Int_t absId) 
{
  if(absId>=0 && absId < fNumOfCell) {
    fLofCells[absId] = cell;
  }
}  

pi0SelectionParam* AliEMCALFolder::GetPi0SelectionParRow(Int_t nrow)
{
  pi0SelectionParam* r=0;
  if(fPi0SelPar) {
    r = fPi0SelPar->GetTable(nrow);
  }
  return r;
}

void AliEMCALFolder::FillPi0Candidate(const Double_t mgg, AliESDCaloCluster* cl1, AliESDCaloCluster* cl2)
{
  static Int_t absIdMax, nm1, nm2;

  u::FillH1(fLhists, 0, 1.);  // number entries 
  u::FillH1(fLhists, 1, mgg);

  nm1 = GetSMNumber(cl1);
  nm2 = GetSMNumber(cl2);

  if(nm1==-1 || nm2==-1) assert(0);

  if(nm1 != nm2) return; // Both cluster should be in the same SM

  AliESDCaloCluster* cl = cl1;
  if(cl1->E() < cl2->E()) cl = cl2; // Get cluster with highest energy

  const Int_t  nDigits  = cl->GetNumberOfDigits();
  //  const UShort_t* adc   = cl->GetDigitAmplitude(); // for future
  const Short_t* absId = cl->GetDigitIndex()->GetArray();

  int    indMax = 0;
  double emax   = cl->GetDigitAmplitude()->At(0);
  if(nDigits > 1) {
    for(int i=1; i<nDigits; i++) {
      if(emax < cl->GetDigitAmplitude()->At(i)) {
        indMax = i;
        emax   = cl->GetDigitAmplitude()->At(i);
      }
    }
  }
  if(emax/cl->E() > 0.5) { // more than 50% of cluster energy
    u::FillH1(fLhists, 0, 2.); // number "good" entries 
    absIdMax = Int_t(absId[indMax]);
    FillPi0Candidate(mgg, absIdMax, nm1);
  }
}

void AliEMCALFolder::FillPi0Candidate(const Double_t mgg, Int_t absIdMax, Int_t nm)
{
  //
  // Jun 08;
  //
  static Int_t nSupModMax, nModuleMax, nIphiMax, nIetaMax, iphiCellMax, ietaCellMax;
  static AliEMCALCell* cell;
  static TDataSet *set; 
  static AliEMCALSuperModule *sm;

  fGeometry->GetCellIndex(absIdMax, nSupModMax, nModuleMax, nIphiMax, nIetaMax);
  if(nm != nSupModMax) assert(0);

  fGeometry->GetCellPhiEtaIndexInSModule(nSupModMax, nModuleMax, nIphiMax, nIetaMax, iphiCellMax, ietaCellMax);

  cell = 0;
  set  = 0;
  sm   = 0;
  if(GetCell(absIdMax)==0) {
    cell = new AliEMCALCell(absIdMax, Form("sm%2.2i:phi%2.2i:eta%2.2i(%4.4i)",nSupModMax,iphiCellMax,ietaCellMax,absIdMax));
    SetCell(cell, absIdMax);
    // For browser
    set  = FindByName(Form("SM%2.2i",nSupModMax));
    if(set==0) {
      sm = new  AliEMCALSuperModule(nSupModMax);
      Add(sm);
      sm->Init();
    } else {
      sm = (AliEMCALSuperModule*)set;
    }
    sm->AddCellToEtaRow(cell, ietaCellMax);
    // 
    cell->SetCCfromCCTable(GetCCIn());
  } else {
    cell = GetCell(absIdMax);
    set  = FindByName(Form("SM%2.2i",nm));
    if(set) sm = (AliEMCALSuperModule*)set;
  }
  if(sm == 0) assert(0);
  if(nm != sm->GetSMNumber()) assert(0);

  u::FillH1(sm->GetHists(), 0, mgg);
  cell->FillEffMass(mgg);
}

void AliEMCALFolder::FitAllSMs()
{ // Jun 14, 2007
  // Only first SM now - should be changed in the future 
   AliEMCALSuperModule *sm0 = GetSuperModule(0);
   sm0->FitForAllCells();
   // Get input calibration table
   AliEMCALCalibCoefs *ccIn = GetCCIn();
   if(ccIn == 0) {
     printf("<E> no input cc \n"); 
     return;
   }
   // New calibration table
   AliEMCALCalibCoefs *ccOut = new AliEMCALCalibCoefs(fgkCCoutName.Data(), ccIn->GetNRows());
   calibCoef *rIn=0, rOut;
   for(Int_t i=0; i<ccIn->GetNRows(); i++){
     rIn  = ccIn->GetTable(i);
     rOut = (*rIn);
     AliEMCALCell* cell = GetCell(rIn->absId);
     if(cell && cell->GetSupMod() == 0) { // only first module now
       rOut.cc = cell->GetCcOut();
     }
     ccOut->AddAt(&rOut);
   }
   ccOut->Purge();
   Add(ccOut);
}

AliEMCALCalibCoefs* AliEMCALFolder::GetCCTable(const char* name)
{
  TDataSet *ds = FindByName(name);
  if(ds) return (AliEMCALCalibCoefs*)ds;
  else   return 0;
}

Int_t AliEMCALFolder::GetSMNumber(AliESDCaloCluster* cl)
{
  static Int_t absId, nSupMod, nModule, nIphi, nIeta;
  nSupMod = -1; // if negative something wrong
  if(cl) {
    absId = Int_t(cl->GetDigitIndex()->At(0));
    fGeometry->GetCellIndex(absId, nSupMod, nModule, nIphi, nIeta);
  }
  return nSupMod;
}

// Recalibration staf - Jun 18,2007
AliEMCALRecPoint* AliEMCALFolder::GetRecPoint(AliESDCaloCluster *cl, AliEMCALCalibCoefs *tOld,AliEMCALCalibCoefs *tNew,
TList *l)
{
  //
  // Static function;
  // Get recalibrated rec.point from ESD cluster
  //
  // printf(" AliEMCALFolder::GetRecPoint() : RECALIBRATION \n");
  static Double_t ADCCHANNELEC = 0.0153;  // Update 24 Apr 2007: 250./16/1024 - width of one ADC channel in GeV
  static Float_t ECAW0 = 4.5; // hard coded now - see AliEMCALClusterizerv1::InitParameters()
  Int_t    ampDigi=0;
  Double_t eDigiNew=0.0; 

  AliEMCALRecPoint *rp=0;
  calibCoef *rOld=0, *rNew=0;
  if(cl && tOld && tNew){
    // cl->PrintClusterInfo(1);
    const Int_t ndg   = cl->GetNumberOfDigits();
    //const Int_t nprim = cl->GetNumberOfPrimaries();

    const Short_t* prim    = cl->GetLabels()->GetArray();
    const Short_t* dgAbsId = cl->GetDigitIndex()->GetArray();
    //    const UShort_t* dgAmp   = cl->GetDigitAmplitude()(); // This is energy - bad definition

    rp = new AliEMCALRecPoint(""); // opt=""
    rp->SetClusterType(AliESDCaloCluster::kClusterv1);
    AliEMCALDigit* dg=0;
    TClonesArray digits("AliEMCALDigit", ndg);
    Int_t absId = 0;
    for(Int_t i=0; i<ndg; i++){
      // Save just abs id and amplitude of digits which will be used for recalculation of
      // cluster energy and position
      absId = Int_t(dgAbsId[i]);
      rOld  = tOld->GetTable(absId);
      rNew  = tNew->GetTable(absId);
      ampDigi = Int_t(cl->GetDigitAmplitude()->At(i) / rOld->cc);

      new(digits[i]) AliEMCALDigit(Int_t(prim[i]),0,absId, ampDigi, 0.0, i, 0.0); 
      dg     =  (AliEMCALDigit*)digits[i];
      eDigiNew = ampDigi * rNew->cc;
      //eDigiNew = ampDigi * ADCCHANNELEC;
      //eDigiNew = Double_t(cl->GetDigitAmplitude()->At(i)); // Copy from ESD for checking
      rp->AddDigit(*dg, Float_t(eDigiNew));
      u::FillH1(l, 14, eDigiNew);
      u::FillH1(l, 15, Double_t(absId));
      //printf("<I> digit %i amp %i rOld->cc %6.5f GeV rNew->cc %6.5f GeV\n", i, ampDigi, rOld->cc, rNew->cc);
    }
    //printf("<I> recalibration of digits was done ! \n");
    //    rp->EvalAll(ECAW0, &digits);
    rp->EvalLocalPosition(ECAW0, &digits); // I need just position
    digits.Delete();
  }
  //rp->Print("print");
  return rp;
}

void  AliEMCALFolder::Save(const char *fn, const char *opt)
{ 
  //
  // Jun 5, 2007; See TFileIter and StFMC.cxx
  //
  TString FN(fn);
  if(FN.Contains(".root")==0) FN += ".root";
  TFileIter f(FN.Data(),opt,"EMCAL object");
  UInt_t eventNum  = 0; // just one object
  UInt_t runNumber = 0; // 0 now, - may statistics on selectorr
  f.NextEventPut(this, eventNum, runNumber);
  printf(" Save %s to file %s\n", GetName(), FN.Data());
}

AliEMCALFolder* AliEMCALFolder::Read(const char *fn, const char *opt)
{ 
  //
  // Jun 27, 2007
  //
  AliEMCALFolder* EMCAL = 0;
  TH1::AddDirectory(0); // this is obligatory

  TFile f(fn,opt);
  TList *l = f.GetListOfKeys();
  printf(" The total number of the objects: %d in file %s\n", l->GetSize(), fn);

  TKey *key = (TKey*)l->At(0);
  EMCAL = (AliEMCALFolder*)key->ReadObj();
  f.Close();
  if(EMCAL) EMCAL->InitAfterRead();

  return EMCAL;
}


void  AliEMCALFolder::InitAfterRead()
{
  lObj    = (TList*)fObj;
  fLhists = (TList*)lObj->FindObject("HistsOfEmcal");
}

void AliEMCALFolder::DrawQA(const int nsm)
{ 
  //
  // Jun 25, 2007
  //

  AliEMCALSuperModule* sm = GetSuperModule(nsm);
  if(sm==0) return;
  TList *l = sm-> GetHists();
  Int_t nx=2, ny=2, wh=530, ww=750;

  TCanvas *c = new TCanvas(Form("QA_%i",GetIterationNumber()), Form("QA_%i",GetIterationNumber()),
			   10, 10, ww, wh);
  c->Divide(nx,ny);

  int ic=1;
  c->cd(ic++);
  TH1 *h1 = (TH1*)l->At(0);
  sm->FitEffMassHist(); 
  u::DrawHist(h1,2);
  h1->SetAxisRange(0.03, 0.28);

  c->cd(ic++);
  sm->DrawCC(0);  
  TH1 *hccin = (TH1*)l->At(1);
  hccin->SetAxisRange(14., 20.);

  gStyle->SetOptStat(1111);
  c->cd(ic++);
  TH1 *hmass = (TH1*)l->At(3);
  u::DrawHist(hmass, 2);
  hmass->SetAxisRange(0.12, 0.16);

  c->cd(ic++);
  TH1 *hres = (TH1*)l->At(4);
  u::DrawHist(hres, 2);
  hres->SetAxisRange(0.05, 0.120);

  if(ic<nx*ny) {
    c->cd(ic++);
    u::DrawHist((TH1*)l->At(5), 2);

    c->cd(ic++);
    u::DrawHist((TH1*)l->At(6), 2);
  }
  c->Update();
}

TList* AliEMCALFolder::BookHists()
{
  gROOT->cd();
  TH1::AddDirectory(1);  

  new TH1F("00_HStat", "hist of common EMCAL statistics", 100, 0.5, 100.5);
  new TH1F("01_EffMassAll", "effective mass of #gamma,#gamma(m_{#pi^{0}}=134.9766 MeV) - whole EMCAL", 250,0.0,0.5);

  TList *l = AliEMCALHistoUtilities::MoveHistsToList("HistsOfEmcal", kFALSE);

  TH1::AddDirectory(0);
  return l;
}

void AliEMCALFolder::CreateCellNtuple()
{
  // Jun 28, 2007
  if(fCellNtuple) { // Already exist
    fCellNtuple->Print();
    return;
  }
  // Create ntuple
  Int_t bsize = int(1.e+5);
  fCellNtuple = new TNtuple("cells","Cells Ntuple for quick analysis",
			    "fAbsId:fSupMod:fModule:fPhi:fEta:fPhiCell:fEtaCell:fCcIn:fCcOut", bsize);
  AliEMCALCell *cell=0;
  // Fill ntuple
  AliEMCALSuperModule* sm = GetSuperModule(0);
  if(sm) printf(" TNtuple was created ! sm0 %s \n", sm->GetName());
  for(int eta=0; eta<48; eta++) { // eta row
    TDataSet *setEta = sm->FindByName(Form("ETA%2.2i",eta));
    if(setEta) {
      printf(" ***** eta row %s ******\n", setEta->GetName());
      for(int phi=0; phi<setEta->GetListSize(); phi++) { // cycle on cells (phi directions)
        cell = (AliEMCALCell*)setEta->At(phi);
        if(cell) {
          cell->FillCellNtuple(fCellNtuple);
          //printf(" fill cell %s : %s \n", cell->GetName(), cell->GetTitle());
	}
      }
    }
  }
  fCellNtuple->Print();
  lObj->Add(fCellNtuple); 
  // --
  CreateAndFillAdditionalHists();
}

void AliEMCALFolder::CreateAndFillAdditionalHists()
{
  TH1::AddDirectory(0);  
  fLhists->Add(new TH1F("02_CCoutOnEdge", "cc out on edge of calorimeter (in MeV)", 70, 12., 19.));
  fLhists->Add(new TH1F("03_CCoutInside", "cc out inside  of calorimeter (in MeV)", 70, 12., 19.));
  // Fill
  Float_t* args;
  for(Int_t i=0; i<(Int_t)fCellNtuple->GetEntries(); i++){
    fCellNtuple->GetEvent(i);
    args = fCellNtuple->GetArgs();
    Int_t phi = (Int_t)args[5];
    Int_t eta = (Int_t)args[6];
    Double_t cc = (Double_t)args[8]*1000.;
    if((phi==0||phi==23) || (eta==0||eta==47)) u::FillH1(fLhists, 2, cc);
    else                                       u::FillH1(fLhists, 3, cc);
  }
  // Drawing
  Int_t wh=530, ww=750;
  TCanvas *c = new TCanvas("c_edge","CEDGE", 10, 10, ww, wh);

  gStyle->SetOptStat(1100);
  gStyle->SetOptFit(111);
  TH1 *h1 = (TH1*)fLhists->At(3);
  TF1 *g = u::Gausi("ccInside", 14.7, 16.4, h1);
  g->SetLineColor(kRed);
  h1->Fit(g,"Q+","", 14.7, 16.4);
  u::DrawHist(h1,2);
  h1->SetTitle("CC distribution after #pi^{0} calibration");
  h1->SetXTitle("  MeV  ");
  h1->SetYTitle("  N  ");
  TLatex *lat1 = u::lat(Form("rel.width = %4.2f%%", 
  100.*h1->GetRMS()/ h1->GetMean()), 16.5, 100., 12, 0.045);
  TLatex *lat2 = u::lat(Form("rel.width = %4.2f%% (from fit)", 
			     100.*g->GetParameter(2)/ g->GetParameter(1)), 16.5, 70., 12, 0.045);

  if(0) {
    TH1 *h2 = (TH1*)fLhists->At(2);
    u::DrawHist(h2,2,1,"same",2);
  }

  TH1F *hccFirst = AliEMCALCalibCoefs::GetHistOfCalibTableFromDb("ccTmp");
  u::DrawHist(hccFirst,2,1,"same",3);

  
  TLegend *leg = new TLegend(0.1,0.6, 0.45,0.85);
  leg->AddEntry(hccFirst, "Initial cc ", "L");
  leg->AddEntry(h1, "Final cc", "L");
  leg->Draw();

  c->Update();
}

void AliEMCALFolder::TestSMStruct()
{
  // testing May 22, 2007
  for(int m=0; m<12; m++) {
    AliEMCALSuperModule *sm = new AliEMCALSuperModule(m);
    Add(sm);
  }
}

