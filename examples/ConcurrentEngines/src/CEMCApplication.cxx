//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file CEMCApplication.cxx
/// \brief Implementation of the CEMCApplication class
///
/// Geant4 ExampleN01 adapted to Virtual Monte Carlo \n
///
/// \date 05/04/2002
/// \author I. Hrivnacova; IPN, Orsay

#include "CEMCApplication.h"
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
ClassImp(CEMCApplication)
/// \endcond

//_____________________________________________________________________________
CEMCApplication::CEMCApplication(const char *name, const char *title)
  : TVirtualMCConcurrentApplication(name,title),
    fCurrentMCEngine(nullptr),
    fMagField(0),
    fMCStackManager(TMCStackManager::Instance()),
    fImedAr(0),
    fImedAl(0),
    fImedPb(0),
    fPreviousMed(-1),
    fTopID(-1),
    fTrackerTubeID(-1),
    fNEventsProcessed(0),
    fCurrTrackId(0),
    fCurrGeoTrackId(0),
    fStackSize(100),
    fNMovedTracks(0),
    fNTracksG3(0),
    fNTracksG4(0),
    fNSecondaries(0),
    fNSecondariesG3(0),
    fNSecondariesG4(0),
    fNGeneratePrimaries(0),
    fDryRun(kFALSE),
    fChooseEngineForTopVolume(kFALSE)

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
CEMCApplication::CEMCApplication()
  : TVirtualMCConcurrentApplication(),
    fCurrentMCEngine(nullptr),
    fMagField(0),
    fMCStackManager(TMCStackManager::Instance()),
    fImedAr(0),
    fImedAl(0),
    fImedPb(0),
    fPreviousMed(-1),
    fTopID(-1),
    fTrackerTubeID(-1),
    fNEventsProcessed(0),
    fCurrTrackId(0),
    fCurrGeoTrackId(0),
    fStackSize(100),
    fNMovedTracks(0),
    fNTracksG3(0),
    fNTracksG4(0),
    fNSecondaries(0),
    fNSecondariesG3(0),
    fNSecondariesG4(0),
    fNGeneratePrimaries(0),
    fDryRun(kFALSE),
    fChooseEngineForTopVolume(kFALSE)
{
/// Default constructor
}

//_____________________________________________________________________________
CEMCApplication::~CEMCApplication()
{
/// Destructor
  delete fMagField;
}

void CEMCApplication::ExportGeometry(const char* path) const
{
  if(!gGeoManager || !gGeoManager->IsClosed()) {
    Warning("ExportGeometry", "TGeoManager not existing or geometry not closed yet.");
    return;
  }
  TString fullPath(path);
  fullPath += "/CE_geometry.root";
  Info("ExportGeometry", "Export geometry to %s.", fullPath.Data());
  gGeoManager->Export(fullPath.Data());
}

//_____________________________________________________________________________
void CEMCApplication::ConstructMaterials()
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
void CEMCApplication::ConstructVolumes()
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
  fTopID = gGeoManager->GetUID("EXPH");

  //------------------------------ a tracker tube

  Double_t trackerTube[3];
  trackerTube[0] = 0.;
  trackerTube[1] = 60.;
  trackerTube[2] = 50.;
  gGeoManager->Volume("TRTU","TUBE", fImedAl, trackerTube, 3);
  fTrackerTubeID = gGeoManager->GetUID("TRTU");

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
//TVirtualMCApplication* CEMCApplication::CloneForWorker() const
//{
//  return new CEMCApplication(GetName(), GetTitle());
//}

//_____________________________________________________________________________
void CEMCApplication::InitForWorker() const
{
  //gMC->SetStack(fStack);
  //gMC->SetMagField(fMagField);
}

//_____________________________________________________________________________
void CEMCApplication::ConstructGeometry()
{
/// Construct geometry using TGeo functions or
/// TVirtualMC functions (if oldGeometry is selected)

  Info("ConstructGeometry", "Construct geometry for all engines");
  ConstructMaterials();
  ConstructVolumes();
}

//_____________________________________________________________________________
void CEMCApplication::InitGeometryConcurrent()
{
/// Initialize geometry.
  Info("InitGeometry", "Init geometry for all engines");
}

//_____________________________________________________________________________
void CEMCApplication::GeneratePrimaries()
{

  // Monitor calls to this method
  fNGeneratePrimaries++;
/// Fill the user stack (derived from TVirtualMCStack) with primary particles.

 // Track ID (filled by stack)
 Int_t ntr;

 // Option: to be tracked
 Int_t toBeDone = 1;

 // Geantino
 Int_t pdg  = 2212;

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
 fMCStackManager->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx, poly, polz,
                  kPPrimary, ntr, 1., 0);

 // Change direction and add particle to stack
 px = 1000.;
 py = 0.;
 pz = 0.1;
 fMCStackManager->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx, poly, polz,
                  kPPrimary, ntr, 1., 0);
  */
}


//_____________________________________________________________________________
void CEMCApplication::BeginEventConcurrent()
{
/// User actions at beginning of event.
/// Nothing to be done this example
}

//_____________________________________________________________________________
void CEMCApplication::BeginPrimaryConcurrent()
{
/// User actions at beginning of a primary track.
/// Nothing to be done this example
}

//_____________________________________________________________________________
void CEMCApplication::PreTrackConcurrent()
{
/// User actions at beginning of each track.
/// Print info message.

  cout << endl;
  // We know our PDG is 0 (geantino), so use this here
  //fCurrTrackId = gGeoManager->AddTrack(fCurrTrackId, 0);
  fCurrTrackId = fMCStackManager->GetCurrentTrackNumber();
  cout << endl;
  cout << "Starting track " << fCurrTrackId;
  if(fTrackIdToGeoTrackId.find(fCurrTrackId) == fTrackIdToGeoTrackId.end()) {
    fTrackIdToGeoTrackId[fCurrTrackId] = gGeoManager->AddTrack(fCurrTrackId, 0);
    cout << " (new)";
    fSteps[fCurrTrackId] = 0;
  }
  cout << endl;
  fCurrGeoTrackId = fTrackIdToGeoTrackId[fCurrTrackId];
  fSteps[fCurrTrackId]++;
}

//_____________________________________________________________________________
void CEMCApplication::SteppingConcurrent()
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
  // Count the secondaries
  if(strcmp(currentEngineName, "TGeant4") == 0) {
    fNSecondariesG4 += fCurrentMCEngine->NSecondaries();
  } else {
    fNSecondariesG3 += fCurrentMCEngine->NSecondaries();
  }

  gGeoManager->GetTrack(fCurrGeoTrackId)->AddPoint(currPosition.X(),
                                                     currPosition.Y(),
                                                     currPosition.Z(),
                                                     currPosition.T());

  fNSecondaries += fCurrentMCEngine->NSecondaries();
  cout << currentEngineName << ", trackID: " << fCurrTrackId << ", PDGID: " << fCurrentMCEngine->TrackPid() << ", Position (t,x,y,z): "
      << currPosition.T() << ", " << currPosition.X() << " " << currPosition.Y() << " " << currPosition.Z()
      << ", Momentum (E, px, py, pz): " << currMomentum.E() << ", "
      << currMomentum.Px() << ", " << currMomentum.Py() << ", " << currMomentum.Pz()
      << ", #secondaries: " << fCurrentMCEngine->NSecondaries()
      << endl;


  //cout << "#particles on stack: " << fMCStackManager->GetNtrack() << "\n"
      // << "#primaries: 3\n"
      // << "#secondaries: " << fNSecondaries << "(" << fCurrentMCEngine->NSecondaries() << ")" << endl;

  /*
  Int_t copyNo;
  Int_t volID = fCurrentMCEngine->CurrentVolID(copyNo);
  cout << "Track\n"
       << "\tPosition (t, x,y,z): "
       << currPosition.T() << ", " << currPosition.X() << " " << currPosition.Y() << " " << currPosition.Z() << "\n"
       << "\tMomentum (E, px, py, pz): " << currMomentum.E() << ", "
       << currMomentum.Px() << ", " << currMomentum.Py() << ", " << currMomentum.Pz()
       << "\nwith #secondaries: " << fCurrentMCEngine->NSecondaries()
       << "\nin volume " <<  fCurrentMCEngine->CurrentVolName() << " (ID: " << volID << ") ";

  Int_t currentMed = fCurrentMCEngine->CurrentMedium();
  if (currentMed == fImedAr) cout << "MediumID: " << fImedAr <<  " ArgonGas";
  if (currentMed == fImedAl) cout << "MediumID: " << fImedAl <<  " Aluminium";
  if (currentMed == fImedPb) cout << "MediumID: " << fImedPb <<  " Lead";
  cout << "\n----------" << endl;

  const Double_t* currPointNav = gGeoManager->GetCurrentPoint();
  cout << "Current state of navigator:\n";
  cout << "Path: " << gGeoManager->GetPath() << "\n";
  cout << "Point (x, y, z): " << currPointNav[0] << ", "
                                << currPointNav[1] << ", "
                                << currPointNav[2] << endl;
  gGeoManager->GetCurrentNode()->Print();
  cout << "----------" << endl;
  */

  // Nothing to do if volume hasn't changed
  /*
  Int_t copyNo;
  Int_t currentVolID = fCurrentMCEngine->CurrentVolID(copyNo);
  if(fPreviousVolID == curentVolID && !fCurrentMCEngine->IsTrackExiting()) {
    return;
  }

  //SuggestCurrentTrackToBeMoved();
  // Move track to TGeant3 in case TGeant4 if track is about to leave tracker volume.
  if(currentVolID == fTrackerTubeID && fCurrentMCEngine->IsTrackExiting() && strcmp(currentEngineName, "TGeant4") == 0) {
    TVirtualMCStack* futureStack = GetMC("TGeant3TGeo")->GetStack();
    // Return already here if future stack is full to avoid overhead
    if(futureStack->GetNtrack() >= fStackSize) {
      return;
    }
    // Get current track and associated particle
    TVirtualMCStack* currentStack = fCurrentMCEngine->GetStack();
    // Track ID (filled by stack)

    Info("Stepping", "Track exits tracker, move track from TGeant4 stack to TGeant3TGeo stack");
    // Finally stop the track at the current engine
    fCurrentMCEngine->StopTrack();
  }
  */
}

//_____________________________________________________________________________
void CEMCApplication::PostTrackConcurrent()
{
  // Sum number of tracks processed by respective engines.
  if(strcmp(fCurrentMCEngine->GetName(), "TGeant4") == 0) {
    fNTracksG4++;
  } else {
    fNTracksG3++;
  }
}

//_____________________________________________________________________________
void CEMCApplication::FinishPrimaryConcurrent()
{
/// User actions after finishing of a primary track.
/// Nothing to be done this example
}

//_____________________________________________________________________________
void CEMCApplication::FinishEventConcurrent()
{
/// User actions after finishing of an event
/// Nothing to be done this example
}

void CEMCApplication::Run(Int_t nofEvents)
{
  fMCManager->InitMCs();
  fMCManager->RunMCs(nofEvents);
}

//_____________________________________________________________________________
void CEMCApplication::PrintStatus() const
{
  cout << "#############################################\n";
  cout << "########## STATUS of MCApplication ##########\n";
  cout << "#############################################\n";
  cout << "---> Registered MC engines: \n";
  /*
  for(const TVirtualMC* mc : fMCEngines) {
    cout << "\t" << mc->GetName() << "\n";
  }
  cout << "---> Event and track info: \n"
       << "\t# events processed: " << fNEventsProcessed << "\n"
       << "\t# tracks transported by TGeant4: " << fNTracksG4 << "\n"
       << "\t# tracks transported by TGeant3TGeo: " << fNTracksG3 << "\n"
       << "\t# tracks transferred from TGeant4 to TGeant3TGeo: " << fNMovedTracks << "\n"
       << "\t# secondaries produced during TGeant4 transport: " << fNSecondariesG4 << "\n"
       << "\t# secondaries produced during TGeant3TGeo transport: " << fNSecondariesG3 << "\n"
       << "\t# explicit primary generation: " << fNGeneratePrimaries << "\n"
       << "\tstack sizes: \n";
  for(const TVirtualMC* mc : fMCEngines) {
    cout << "\t\t" << mc->GetStack()->GetNtrack() << "\n";
  }
  */
}
