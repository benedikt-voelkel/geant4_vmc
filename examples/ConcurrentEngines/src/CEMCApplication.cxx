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

#include <TROOT.h>
#include <Riostream.h>
#include <TInterpreter.h>
#include <TVirtualMC.h>
#include <TLorentzVector.h>
#include <TArrayD.h>
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoMaterial.h>
#include <TThread.h>
#include <TError.h>

#include "TParticle.h"

using namespace std;

/// \cond CLASSIMP
ClassImp(CEMCApplication)
/// \endcond

//_____________________________________________________________________________
CEMCApplication::CEMCApplication(const char *name, const char *title)
  : TVirtualMCApplication(name,title),
    fMagField(0),
    fImedAr(0),
    fImedAl(0),
    fImedPb(0),
    fPreviousMed(-1),
    fTopID(-1),
    fTrackerTubeID(-1),
    fNEventsProcessed(0),
    fStackSize(100),
    fNMovedTracks(0),
    fNTracksG3(0),
    fNTracksG4(0),
    fNSecondariesG3(0),
    fNSecondariesG4(0),
    fNGeneratePrimaries(0)

{
/// Standard constructor
/// \param name   The MC application name
/// \param title  The MC application description

  // create magnetic field (with zero value)
  fMagField = new TGeoUniformMagField();
}

//_____________________________________________________________________________
CEMCApplication::CEMCApplication()
  : TVirtualMCApplication(),
    fMagField(0),
    fImedAr(0),
    fImedAl(0),
    fImedPb(0),
    fPreviousMed(-1),
    fTopID(-1),
    fTrackerTubeID(-1),
    fNEventsProcessed(0),
    fStackSize(100),
    fNMovedTracks(0),
    fNTracksG3(0),
    fNTracksG4(0),
    fNSecondariesG3(0),
    fNSecondariesG4(0),
    fNGeneratePrimaries(0)

{
/// Default constructor
}

//_____________________________________________________________________________
CEMCApplication::~CEMCApplication()
{
/// Destructor
  for(auto& stack : fStacks) {
    delete stack;
  }
  delete fMagField;
}

//
// private methods
//

TVirtualMC* CEMCApplication::GetMC(const char* name)
{
  for(TVirtualMC* mc : fMCEngines) {
    if(strcmp(mc->GetName(), name) == 0) {
      return mc;
    }
  }
  return nullptr;
}

void CEMCApplication::SetMCMediaProperties()
{
  Info("SetMCMediaProperties", "Setup media properties for transportation");
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
void CEMCApplication::InitMCs()
{
/// Initialize MCs.

  // Check for presence of G3 and G4 by name, required
  Bool_t foundG3 = kFALSE;
  Bool_t foundG4 = kFALSE;
  for(TVirtualMC* mc : fMCEngines) {
    if(strcmp(mc->GetName(), "TGeant3TGeo") == 0) {
      foundG3 = kTRUE;
    } else if(strcmp(mc->GetName(), "TGeant4") == 0) {
      foundG4 = kTRUE;
    }
  }
  if(!foundG3 || !foundG4) {
    Fatal("CEMCApplication::InitMCs","Could not find both TGeant3 and TGeant4...exit");
  }

  for(TVirtualMC* mc : fMCEngines) {
    // Notify to use geometry built using TGeo
    mc->SetRootGeometry();
    // create a user stack for each engine
    CEMCStack* stack = new CEMCStack(fStackSize);
    fStacks.push_back(stack);
    fCurrentMCEngine = mc;
    mc->SetStack(stack);
    mc->SetMagField(fMagField);
    mc->Init();
    mc->BuildPhysics();
  }
}

//__________________________________________________________________________
void CEMCApplication::RunMCs(Int_t nofEvents)
{
/// Run MC.
/// \param nofEvents Number of events to be processed
// First see list of TGeoNavigator objects registered to TGeoManager
Info("RunMCs", "There are %i navigators registered to TGeoManager", gGeoManager->GetListOfNavigators()->GetEntries());
  // Run 1 event nofEvents times to cover TGeant3 and TGeant4 per event
  for(Int_t i = 0; i < nofEvents; i++) {
    // Start with TGeant4
    fCurrentMCEngine = GetMC("TGeant4");
    Info("RunMCs", "Starting one event for engine %s", fCurrentMCEngine->GetName());
    fCurrentMCEngine->ProcessRun(1);
    // Now change to TGeant3
    fCurrentMCEngine = GetMC("TGeant3TGeo");
    Info("RunMCs", "Switching to engine %s", fCurrentMCEngine->GetName());
    fCurrentMCEngine->ProcessRun(1);
    // Increment the number of events finished
    fNEventsProcessed++;
  }
}

//_____________________________________________________________________________
void CEMCApplication::FinishRun()
{
/// Finish MC run.
}

//_____________________________________________________________________________
TVirtualMCApplication* CEMCApplication::CloneForWorker() const
{
  return new CEMCApplication(GetName(), GetTitle());
}

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
void CEMCApplication::InitGeometry()
{
/// Initialize geometry.
  Info("InitGeometry", "Init geometry for engine %s", fCurrentMCEngine->GetName());
}

//_____________________________________________________________________________
void CEMCApplication::GeneratePrimaries()
{
  // The stack is filled in Stepping, only TGeant4 is filled.
  if(strcmp(fCurrentMCEngine->GetName(), "TGeant3TGeo") == 0 ) {
    return;
  }
  // Monitor calls to this method
  fNGeneratePrimaries++;
/// Fill the user stack (derived from TVirtualMCStack) with primary particles.

 // Track ID (filled by stack)
 Int_t ntr;

 // Option: to be tracked
 Int_t toBeDone = 1;

 // Geantino
 Int_t pdg  = 0;

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
 px = 1.;
 py = 0.;
 pz = 0.;
 e  = 1.;

 TVirtualMCStack* stack = fCurrentMCEngine->GetStack();
 // Add particle to stack
 stack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx, poly, polz,
                  kPPrimary, ntr, 1., 0);

 // Change direction and add particle to stack
 px = 1.;
 py = 0.1;
 pz = 0.;
 stack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx, poly, polz,
                  kPPrimary, ntr, 1., 0);

 // Change direction and add particle to stack
 px = 1.;
 py = 0.;
 pz = 0.1;
 stack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx, poly, polz,
                  kPPrimary, ntr, 1., 0);
}

//_____________________________________________________________________________
void CEMCApplication::BeginEvent()
{
/// User actions at beginning of event.
/// Nothing to be done this example
}

//_____________________________________________________________________________
void CEMCApplication::BeginPrimary()
{
/// User actions at beginning of a primary track.
/// Nothing to be done this example
}

//_____________________________________________________________________________
void CEMCApplication::PreTrack()
{
/// User actions at beginning of each track.
/// Print info message.

  cout << endl;
  cout << "Starting new track" << endl;
}

//_____________________________________________________________________________
void CEMCApplication::Stepping()
{
/// User actions at each step.
/// Print track position, the current volume and current medium names.

  TLorentzVector position;
  fCurrentMCEngine->TrackPosition(position);
  // Temporary pointer to name
  const char* currentEngineName = fCurrentMCEngine->GetName();

  Info("Stepping", "Stepping in engine %s", currentEngineName);
  // Count the secondaries
  if(strcmp(currentEngineName, "TGeant4")) {
    fNSecondariesG4 += fCurrentMCEngine->NSecondaries();
  } else {
    fNSecondariesG3 += fCurrentMCEngine->NSecondaries();
  }

  cout << "Track "
       << position.X() << " " << position.Y() << " " << position.Z()
       << "  in " <<  fCurrentMCEngine->CurrentVolName() << "  ";


  Int_t currentMed = fCurrentMCEngine->CurrentMedium();
  if (currentMed == fImedAr) cout << "MediumID: " << fImedAr <<  " ArgonGas";
  if (currentMed == fImedAl) cout << "MediumID: " << fImedAl <<  " Aluminium";
  if (currentMed == fImedPb) cout << "MediumID: " << fImedPb <<  " Lead";
  cout << endl;

  // Nothing to do if medium hasn't changed
  if(fPreviousMed == currentMed) {
    return;
  }
  Int_t copyNo;
  Int_t currentVolID = fCurrentMCEngine->CurrentVolID(copyNo);
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
    Int_t ntr;
    // Move that track to future stack
    Double_t currPosX;
    Double_t currPosY;
    Double_t currPosZ;
    Double_t currMomX;
    Double_t currMomY;
    Double_t currMomZ;
    Double_t currE;
    Int_t currPDG = fCurrentMCEngine->TrackPid();
    Double_t currTof = fCurrentMCEngine->TrackTime();
    // \note \todo These are the initial polarisations
    TParticle* particle = currentStack->GetCurrentTrack();
    TVector3 v;
    particle->GetPolarisation(v);
    fCurrentMCEngine->TrackPosition(currPosX, currPosY, currPosZ);
    fCurrentMCEngine->TrackMomentum(currMomX, currMomY, currMomZ, currE);
    futureStack->PushTrack(1, -1, currPDG, currMomX, currMomY, currMomZ, currE,
                           currPosX, currPosY, currPosZ, 0., v.X(), v.Y(), v.Z(),
                           kPTransportation, ntr, 1., 0);
    fNMovedTracks++;
    Info("Stepping", "Track exits tracker, move track from TGeant4 stack to TGeant3TGeo stack");
    cout << "Some track properties:\n";
    cout << "x, y, z = " << currPosX << ", " << currPosY << ", " << currPosZ << "\n";
    cout << "E, p_x, p_y, p_z = " << currE << ", " << currMomX << ", " << currMomY << ", " << currMomZ << endl;
    // Finally stop the track at the current engine
    fCurrentMCEngine->StopTrack();
  }
}

//_____________________________________________________________________________
void CEMCApplication::PostTrack()
{
  // Sum number of tracks processed by respective engines.
  if(strcmp(fCurrentMCEngine->GetName(), "TGeant4") == 0) {
    fNTracksG4++;
  } else {
    fNTracksG3++;
  }
}

//_____________________________________________________________________________
void CEMCApplication::FinishPrimary()
{
/// User actions after finishing of a primary track.
/// Nothing to be done this example
}

//_____________________________________________________________________________
void CEMCApplication::FinishEvent()
{
/// User actions after finishing of an event
/// Nothing to be done this example
}

//_____________________________________________________________________________
void CEMCApplication::PrintSummary() const
{
  Info("PrintSummary", "Print summary");
  cout << "# events processed: " << fNEventsProcessed << "\n";
  cout << "# tracks transported by TGeant4: " << fNTracksG4 << "\n";
  cout << "# tracks transported by TGeant3TGeo: " << fNTracksG3 << "\n";
  cout << "# tracks transferred from TGeant4 to TGeant3TGeo: " << fNMovedTracks << "\n";
  cout << "# secondaries produced during TGeant4 transport: " << fNSecondariesG4 << "\n";
  cout << "# secondaries produced during TGeant3TGeo transport: " << fNSecondariesG4 << "\n";
  cout << "# explicit primary generation: " << fNGeneratePrimaries << "\n";
}

//_____________________________________________________________________________
void CEMCApplication::SetPrimaryMCEngine(TVirtualMC* mc)
{
  SetPrimaryMCEngine(mc->GetName());
}

//_____________________________________________________________________________
void CEMCApplication::SetPrimaryMCEngine(const char* mcName)
{
  for(TVirtualMC* mc : fMCEngines) {
    if(strcmp(mc->GetName(), mcName) == 0) {
      //fPrimaryMCEngine = mc;
      return;
    }
  }
  Fatal("SetPrimaryMCEngine", "Engine %s not found.", mcName);
}
