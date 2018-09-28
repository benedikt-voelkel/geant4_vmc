//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file CEMCSingleApplication.cxx
/// \brief Implementation of the CEMCSingleApplication class
///
/// Geant4 ExampleN01 adapted to Virtual Monte Carlo \n
///
/// \date 05/04/2002
/// \author I. Hrivnacova; IPN, Orsay

#include "CEMCSingleApplication.h"
#include "CEMCStack.h"

#include <TMCManager.h>
#include <TVirtualMC.h>
#include <TROOT.h>
#include <Riostream.h>
#include <TInterpreter.h>
#include <TMCStackManager.h>
#include <TLorentzVector.h>
#include <TArrayD.h>
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoMaterial.h>
#include <TVirtualGeoTrack.h>
#include <TThread.h>
#include <TError.h>

#include "TParticle.h"

using namespace std;

/// \cond CLASSIMP
ClassImp(CEMCSingleApplication)
/// \endcond

//_____________________________________________________________________________
CEMCSingleApplication::CEMCSingleApplication(const char *name, const char *title)
  : TVirtualMCApplication(name,title),
    fCurrentMCEngine(nullptr),
    fMagField(0),
    fMCStackManager(TMCStackManager::Instance()),
    fImedAr(0),
    fImedAl(0),
    fImedPb(0),
    fNEventsProcessed(0),
    fCurrTrackId(0),
    fCurrGeoTrackId(0),
    fStackSize(100),
    fNTracks(0),
    fNSecondaries(0),
    fNGeneratePrimaries(0),
    fDryRun(kFALSE)

{
/// Standard constructor
/// \param name   The MC application name
/// \param title  The MC application description

  // create magnetic field (with zero value)
  fMagField = new TGeoUniformMagField();
  fStack = new CEMCStack(fStackSize);
  fMCStackManager->RegisterStack(fStack);
  fMCManager->ConnectToCurrentMC(fCurrentMCEngine);
}

//_____________________________________________________________________________
CEMCSingleApplication::CEMCSingleApplication()
  : TVirtualMCApplication(),
    fCurrentMCEngine(nullptr),
    fMagField(0),
    fMCStackManager(TMCStackManager::Instance()),
    fImedAr(0),
    fImedAl(0),
    fImedPb(0),
    fNEventsProcessed(0),
    fCurrTrackId(0),
    fCurrGeoTrackId(0),
    fStackSize(100),
    fNTracks(0),
    fNSecondaries(0),
    fNGeneratePrimaries(0),
    fDryRun(kFALSE)
{
/// Default constructor
}

//_____________________________________________________________________________
CEMCSingleApplication::~CEMCSingleApplication()
{
/// Destructor
  delete fMagField;
}

void CEMCSingleApplication::ExportGeometry(const char* path) const
{
  if(!gGeoManager || !gGeoManager->IsClosed()) {
    Warning("ExportGeometry", "TGeoManager not existing or geometry not closed yet.");
    return;
  }
  Info("ExportGeometry", "Export geometry to %s.", path);
  gGeoManager->Export(path);
}

//_____________________________________________________________________________
void CEMCSingleApplication::ConstructMaterials()
{
/// Construct materials using TGeo modeller

  // Create Root geometry manager
  new TGeoManager("CE_geometry", "CE VMC example geometry");

  Double_t a;        // Mass of a mole in g/mole
  Double_t z;        // Atomic number
  Double_t density;  // Material density in g/cm3

  a = 39.95;
  z = 18.;
  density = 1.782e-03;
  TGeoMaterial* matAr
    = new TGeoMaterial("ArgonGas", a, z, density);

  a = 26.98;
  z = 13.;
  density = 2.7;
  TGeoMaterial* matAl
    = new TGeoMaterial("Aluminium", a, z, density);

  a = 207.19;
  z = 82.;
  density = 11.35;
  TGeoMaterial* matLead
    = new TGeoMaterial("Lead", a, z, density);

/*
  // Set material IDs
  // This step is needed, only if user wants to use the material Ids
  // in his application. Be aware that the material Ids vary
  // with each concrete MC.
  // It is recommended to use Media Ids instead, which values
  // set by user are preserved in all MCs
  Int_t imat = 0;
  matAr->SetUniqueID(imat++);
  matAl->SetUniqueID(imat++);
  matLead->SetUniqueID(imat++);
*/

  //
  // Tracking medias
  //

  Double_t param[20];
  param[0] = 0;     // isvol  - Not used
  param[1] = 2;     // ifield - User defined magnetic field
  param[2] = 10.;   // fieldm - Maximum field value (in kiloGauss)
  param[3] = -20.;  // tmaxfd - Maximum angle due to field deflection
  param[4] = -0.01; // stemax - Maximum displacement for multiple scat
  param[5] = -.3;   // deemax - Maximum fractional energy loss, DLS
  param[6] = .001;  // epsil - Tracking precision
  param[7] = -.8;   // stmin
  for ( Int_t i=8; i<20; ++i) param[i] = 0.;

  fImedAr = 1;
  new TGeoMedium("ArgonGas", fImedAr, matAr, param);

  fImedAl = 2;
  new TGeoMedium("Aluminium", fImedAl, matAl, param);

  fImedPb = 3;
  new TGeoMedium("Lead", fImedPb, matLead, param);

}

//_____________________________________________________________________________
void CEMCSingleApplication::ConstructVolumes()
{
/// Contruct volumes using TGeo modeller

  //------------------------------ experimental hall (world volume)
  //------------------------------ beam line along x axis

  Double_t* ubuf = 0;

  Double_t expHall[3];
  expHall[0] = 300.;
  expHall[1] = 100.;
  expHall[2] = 100.;
  TGeoVolume *top = gGeoManager->Volume("EXPH","BOX", fImedAr, expHall, 3);
  gGeoManager->SetTopVolume(top);

  //------------------------------ a tracker tube

  Double_t trackerTube[3];
  trackerTube[0] = 0.;
  trackerTube[1] = 60.;
  trackerTube[2] = 50.;
  gGeoManager->Volume("TRTU","TUBE", fImedAl, trackerTube, 3);

  Double_t posX = -100.;
  Double_t posY =  0.;
  Double_t posZ =  0.;
  gGeoManager->Node("TRTU", 1 ,"EXPH", posX, posY, posZ, 0, kTRUE, ubuf);

  //------------------------------ a calorimeter block

  Double_t calBox[3];
  calBox[0] = 100.;
  calBox[1] = 50.;
  calBox[2] = 50.;
  gGeoManager->Volume("CALB","BOX", fImedPb, calBox, 3);

  posX = 100.;
  posY = 0.;
  posZ = 0.;
  gGeoManager->Node("CALB", 1 ,"EXPH", posX, posY, posZ, 0, kTRUE, ubuf);

  //------------------------------ calorimeter layers

  Double_t layerBox[3];
  layerBox[0] = 1.;
  layerBox[1] = 40.;
  layerBox[2] = 40.;
  gGeoManager->Volume("LAYB","BOX", fImedAl, layerBox, 3);

  for (Int_t i=0; i<19; i++) {
    posX = (i-9) * 10.;
    posY = 0.;
    posZ = 0.;
    gGeoManager->Node("LAYB", i ,"CALB", posX, posY, posZ, 0, kTRUE, ubuf);
  }

}

//
// public methods
//


//_____________________________________________________________________________
//TVirtualMCApplication* CEMCSingleApplication::CloneForWorker() const
//{
//  return new CEMCSingleApplication(GetName(), GetTitle());
//}

//_____________________________________________________________________________
void CEMCSingleApplication::InitForWorker() const
{
  //gMC->SetStack(fStack);
  //gMC->SetMagField(fMagField);
}

//_____________________________________________________________________________
void CEMCSingleApplication::ConstructGeometry()
{
/// Construct geometry using TGeo functions or
/// TVirtualMC functions (if oldGeometry is selected)

  Info("ConstructGeometry", "Construct geometry for all engines");
  ConstructMaterials();
  ConstructVolumes();
}

//_____________________________________________________________________________
void CEMCSingleApplication::InitGeometry()
{
/// Initialize geometry.
  Info("InitGeometry", "Init geometry for all engines");
}

//_____________________________________________________________________________
void CEMCSingleApplication::GeneratePrimaries()
{

  // Monitor calls to this method
  fNGeneratePrimaries++;
/// Fill the user stack (derived from TVirtualMCStack) with primary particles.

 // Track ID (filled by stack)
 Int_t ntr;

 // Option: to be tracked
 Int_t toBeDone = 1;

 // Geantino
 //Int_t pdg  = 0;
 // Proton
 //Int_t pdg  = 2212;
 // Electron
 Int_t pdg  = 11;
 // Polarization
 Double_t polx = 0.;
 Double_t poly = 0.;
 Double_t polz = 0.;

 // Position
 Double_t vx  = -200.;
 Double_t vy  = 0.;
 Double_t vz  = 0.;
 Double_t tof = 0.;

 // Momentum
 Double_t px, py, pz, e;
 px = 10.;
 py = 0.;
 pz = 0.;
 e  = 10.;



 // Add particle to stack
   fStack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx, poly, polz,
                  kPPrimary, ntr, 1., 0);

 // Change direction and add particle to stack
/*
 px = 10.;
 py = 0.1;
 pz = 0.;
 fStack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx, poly, polz,
                  kPPrimary, ntr, 1., 0);

 // Change direction and add particle to stack
 px = 1000.;
 py = 0.;
 pz = 0.1;
 fStack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx, poly, polz,
                  kPPrimary, ntr, 1., 0);
  */
}


//_____________________________________________________________________________
void CEMCSingleApplication::BeginEvent()
{
/// User actions at beginning of event.
/// Nothing to be done this example
}

//_____________________________________________________________________________
void CEMCSingleApplication::BeginPrimary()
{
/// User actions at beginning of a primary track.
/// Nothing to be done this example
}

//_____________________________________________________________________________
void CEMCSingleApplication::PreTrack()
{
/// User actions at beginning of each track.
/// Print info message.
  return;

  // We know our PDG is 0 (geantino), so use this here
  //fCurrTrackId = gGeoManager->AddTrack(fCurrTrackId, 0);
  fCurrTrackId = fMCStackManager->GetCurrentTrackNumber();
  //cout << "Starting track " << fCurrTrackId;
  if(fTrackIdToGeoTrackId.find(fCurrTrackId) == fTrackIdToGeoTrackId.end()) {
    fTrackIdToGeoTrackId[fCurrTrackId] = gGeoManager->AddTrack(fCurrTrackId, 0);
    //cout << " (new)";
    fSteps[fCurrTrackId] = 0;
  }
  //cout << endl;
  fCurrGeoTrackId = fTrackIdToGeoTrackId[fCurrTrackId];
  fSteps[fCurrTrackId]++;
}

//_____________________________________________________________________________
void CEMCSingleApplication::Stepping()
{
/// User actions at each step.
/// Print track position, the current volume and current medium names.


  TLorentzVector currPosition;
  TLorentzVector currMomentum;
  fCurrentMCEngine->TrackPosition(currPosition);
  fCurrentMCEngine->TrackMomentum(currMomentum);
  //Double_t currTof = fCurrentMCEngine->TrackTime();
  // Temporary pointer to name
  const char* currentEngineName = fCurrentMCEngine->GetName();

  //Info("Stepping", "Stepping in engine %s", currentEngineName);


  /*gGeoManager->GetTrack(fCurrGeoTrackId)->AddPoint(currPosition.X(),
                                                   currPosition.Y(),
                                                   currPosition.Z(),
                                                   currPosition.T());*/
  // Count secondarie
  fNSecondaries += fCurrentMCEngine->NSecondaries();
  // Extract current volume information from VMC
  Int_t copyNo;
  Int_t volID = fCurrentMCEngine->CurrentVolID(copyNo);
  // Get spatial information from navigator
  const Double_t* currPointNav = gGeoManager->GetCurrentPoint();
  /*cout << "\nPosition VMC (t,x,y,z): " << currPosition.T() << ", "
       << currPosition.X() << " " << currPosition.Y() << " " << currPosition.Z()
       << "\nMomentum (E,px,py,pz): " << currMomentum.E() << ", "
       << currMomentum.Px() << ", " << currMomentum.Py() << ", " << currMomentum.Pz()
       << "\ntrackID: " << fCurrTrackId << ", PDGID: "
       << fCurrentMCEngine->TrackPid()
       << "\n(volName: " << fCurrentMCEngine->CurrentVolName() << ", id, copyNo: "
       << volID << ", " << copyNo << ")"
       << "\nPosition navigator (x,y,z): " << currPointNav[0] << ", "
       << currPointNav[1] << ", " << currPointNav[2]
       << " (path nav: " << gGeoManager->GetPath() << ")\n";*/
       /*
  cout


      << ", #secondaries: " << fCurrentMCEngine->NSecondaries()
      << endl;



  Int_t currentMed = fCurrentMCEngine->CurrentMedium();
  if (currentMed == fImedAr) cout << "MediumID: " << fImedAr <<  " ArgonGas";
  if (currentMed == fImedAl) cout << "MediumID: " << fImedAl <<  " Aluminium";
  if (currentMed == fImedPb) cout << "MediumID: " << fImedPb <<  " Lead";
  */

}

//_____________________________________________________________________________
void CEMCSingleApplication::PostTrack()
{
  // Sum number of tracks processed by respective engines.
  fNTracks++;
}

//_____________________________________________________________________________
void CEMCSingleApplication::FinishPrimary()
{
/// User actions after finishing of a primary track.
/// Nothing to be done this example
}

//_____________________________________________________________________________
void CEMCSingleApplication::FinishEvent()
{
/// User actions after finishing of an event
/// Nothing to be done this example
  Int_t nEevent = fCurrentMCEngine->CurrentEvent();

  TString path("CE_geometry");
  path += nEevent;
  path += ".root";
  //ExportGeometry(path.Data());
  //gGeoManager->ClearTracks();
}

void CEMCSingleApplication::Run(Int_t nofEvents)
{
  fCurrentMCEngine->SetRootGeometry();
  // Further init steps for the MCs
  fCurrentMCEngine->Init();
  fCurrentMCEngine->BuildPhysics();
  fCurrentMCEngine->ProcessRun(nofEvents);
}

//_____________________________________________________________________________
void CEMCSingleApplication::PrintStatus() const
{
  cout << "#############################################\n";
  cout << "########## STATUS of MCApplication ##########\n";
  cout << "#############################################\n";
  /*
  cout << "---> Event and track info: \n"
       << "\t# events processed: " << fNEventsProcessed << "\n"
       << "\t# tracks transported: " << fNTracks << "\n"
       << "\t# secondaries produced during transport: " << fNSecondaries << "\n"
       << "\t# explicit primary generation: " << fNGeneratePrimaries << "\n"
  */
}
