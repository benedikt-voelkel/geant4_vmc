//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file MEMCSingleApplication.cxx
/// \brief Implementation of the MEMCSingleApplication class
///
/// Geant4 ExampleN01 adapted to Virtual Monte Carlo \n
///
/// \date 10/12/2018
/// \author B. Volkel; University Heidelberg

#include "MEMCSingleApplication.h"
#include "MEMCSingleStack.h"

#include <TVirtualMC.h>

#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoMaterial.h>
#include <TVirtualGeoTrack.h>

#include <TROOT.h>
#include <Riostream.h>
#include <TInterpreter.h>
#include <TThread.h>

#include <TLorentzVector.h>
#include <TArrayD.h>
#include <TError.h>

using namespace std;

/// \cond CLASSIMP
ClassImp(MEMCSingleApplication)
/// \endcond

//_____________________________________________________________________________
MEMCSingleApplication::MEMCSingleApplication(const char *name, const char *title)
  : TVirtualMCApplication(name,title),
    fMagField(0),
    fImedAr(0),
    fImedAl(0),
    fImedPb(0),
    fNEventsProcessed(0),
    fCurrTrackId(-1),
    fStackSize(100),
    fNTracks(0),
    fNSteps(0),
    fCurrNTracks(0),
    fCurrNSteps(0)

{
/// Standard constructor
/// \param name   The MC application name
/// \param title  The MC application description

  // create magnetic field (with zero value)
  fMagField = new TGeoUniformMagField();
  fStack = new MEMCSingleStack(fStackSize);
}

//_____________________________________________________________________________
MEMCSingleApplication::MEMCSingleApplication()
  : TVirtualMCApplication(),
    fMagField(0),
    fImedAr(0),
    fImedAl(0),
    fImedPb(0),
    fNEventsProcessed(0),
    fCurrTrackId(-1),
    fStackSize(100),
    fNTracks(0),
    fNSteps(0),
    fCurrNTracks(0),
    fCurrNSteps(0)
{
/// Default constructor
}

//_____________________________________________________________________________
MEMCSingleApplication::MEMCSingleApplication(const MEMCSingleApplication& rhs)
  : TVirtualMCApplication(rhs.GetName(), rhs.GetTitle()),
    fMagField(0),
    fImedAr(0),
    fImedAl(0),
    fImedPb(0),
    fNEventsProcessed(0),
    fCurrTrackId(-1),
    fStackSize(100),
    fNTracks(0),
    fNSteps(0),
    fCurrNTracks(0),
    fCurrNSteps(0)

{
/// Standard constructor
/// \param name   The MC application name
/// \param title  The MC application description

  // create magnetic field (with zero value)
  fMagField = new TGeoUniformMagField();
  fStack = new MEMCSingleStack(rhs.fStackSize);
}

//_____________________________________________________________________________
MEMCSingleApplication::~MEMCSingleApplication()
{
/// Destructor
  delete fMagField;
}

void MEMCSingleApplication::ExportGeometry(const char* path) const
{
  if(!gGeoManager || !gGeoManager->IsClosed()) {
    Warning("ExportGeometry", "TGeoManager not existing or geometry not closed yet.");
    return;
  }
  Info("ExportGeometry", "Export geometry to %s.", path);
  gGeoManager->Export(path);
}

//_____________________________________________________________________________
void MEMCSingleApplication::ConstructMaterials()
{
/// Construct materials using TGeo modeller

  // Create Root geometry manager
  new TGeoManager("ME_geometry", "ME VMC example geometry");

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
void MEMCSingleApplication::ConstructVolumes()
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
//TVirtualMCApplication* MEMCSingleApplication::CloneForWorker() const
//{
//  return new MEMCSingleApplication(GetName(), GetTitle());
//}

//_____________________________________________________________________________
void MEMCSingleApplication::InitForWorker() const
{
  //gMC->SetStack(fStack);
  //gMC->SetMagField(fMagField);
}

//_____________________________________________________________________________
void MEMCSingleApplication::ConstructGeometry()
{
/// Construct geometry using TGeo functions or
/// TVirtualMC functions (if oldGeometry is selected)

  Info("ConstructGeometry", "Construct geometry for all engines");
  ConstructMaterials();
  ConstructVolumes();
}

//_____________________________________________________________________________
void MEMCSingleApplication::InitGeometry()
{
/// Initialize geometry.
  Info("InitGeometry", "Init geometry for all engines");
}

//_____________________________________________________________________________
void MEMCSingleApplication::GeneratePrimaries()
{
/// Fill the user stack (derived from TVirtualMCStack) with primary particles.

 // Track ID (filled by stack)
 Int_t ntr;

 // Option: to be tracked
 Int_t toBeDone = 1;

 // Geantino
 Int_t pdg  = 2212;
 // Proton
 //Int_t pdg  = 2212;
 // Electron
 //Int_t pdg  = 11;
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
   fStack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx,
                     poly, polz, kPPrimary, ntr, 1., 0);

 // Change direction and add particle to stack
/*
 px = 10.;
 py = 0.1;
 pz = 0.;
 fStack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx,
                   poly, polz, kPPrimary, ntr, 1., 0);

 // Change direction and add particle to stack
 px = 1000.;
 py = 0.;
 pz = 0.1;
 fStack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx,
                   poly, polz, kPPrimary, ntr, 1., 0);
  */
}


//_____________________________________________________________________________
void MEMCSingleApplication::BeginEvent()
{
  cout << "----------- Start event " << fNEventsProcessed + 1 << " -----------"
       << endl;
}

//_____________________________________________________________________________
void MEMCSingleApplication::BeginPrimary()
{
/// User actions at beginning of a primary track.
/// Nothing to be done this example
}

//_____________________________________________________________________________
void MEMCSingleApplication::PreTrack()
{
  fNTracks++;
  fCurrNTracks++;
  fCurrTrackId = fStack->GetCurrentTrackNumber();
  cout << "----------- Start track " << fCurrTrackId << " -----------" << endl;
}

//_____________________________________________________________________________
void MEMCSingleApplication::Stepping()
{
  fNSteps++;
  fCurrNSteps++;
  TLorentzVector currPosition;
  TLorentzVector currMomentum;
  fMC->TrackPosition(currPosition);
  fMC->TrackMomentum(currMomentum);
  // Temporary pointer to name
  const char* currentEngineName = fMC->GetName();

  // Extract current volume information from VMC
  Int_t copyNo;
  Int_t volID = fMC->CurrentVolID(copyNo);
  // Get spatial information from navigator
  cout << "---------------- Step " << fCurrNSteps << " ----------------\n"
       << "Position VMC (t,x,y,z): " << currPosition.T() << ", "
       << currPosition.X() << " " << currPosition.Y() << " " << currPosition.Z()
       << "\nMomentum (E,px,py,pz): " << currMomentum.E() << ", "
       << currMomentum.Px() << ", " << currMomentum.Py() << ", " << currMomentum.Pz()
       << "\ntrackID: " << fCurrTrackId << ", PDGID: "
       << fMC->TrackPid()
       << "\n(volName: " << fMC->CurrentVolName() << ", id, copyNo: "
       << volID << ", " << copyNo << ")" << endl;
}

//_____________________________________________________________________________
void MEMCSingleApplication::PostTrack()
{
  cout << "----------- Finish track " << fCurrTrackId << " -----------" << endl;
}

//_____________________________________________________________________________
void MEMCSingleApplication::FinishPrimary()
{
/// User actions after finishing of a primary track.
/// Nothing to be done this example
}

//_____________________________________________________________________________
void MEMCSingleApplication::FinishEvent()
{
/// User actions after finishing of an event
/// Nothing to be done this example
  fNEventsProcessed++;
  cout << "----------- Finish event " << fNEventsProcessed << " -----------\n"
       << "number of tracks: " << fCurrNTracks << "\n"
       << "number of steps: " << fCurrNSteps << endl;
  fStack->Reset();
  fCurrNSteps = 0;
  fCurrNTracks = 0;
}

//_____________________________________________________________________________
void MEMCSingleApplication::InitTransport(std::function<void(TVirtualMC*)>
                                          customInit)
{
  // Check whether there is a TVirtualMCMultiStack and fail if not.
  if(!fStack) {
    Fatal("InitMCs","A TVirtualMCMultiStack is required");
  }

  /// Initialize engine
  fMC->SetStack(fStack);
  fMC->SetRootGeometry();
  customInit(fMC);
  fMC->Init();
  fMC->BuildPhysics();
}

//_____________________________________________________________________________
void MEMCSingleApplication::RunTransport(Int_t nofEvents)
{
  // Check dryrun, so far nothing is done.
  if(nofEvents < 1) {
    Info("RunMCs", "Starting dry run.");
    return;
  }
  fMC->ProcessRun(nofEvents);
}

//_____________________________________________________________________________
TVirtualMCApplication* MEMCSingleApplication::CloneForWorker() const
{
  return new MEMCSingleApplication(*this);
}

//_____________________________________________________________________________
void MEMCSingleApplication::PrintStatus() const
{
  cout << "#############################################\n";
  cout << "########## STATUS of MCApplication ##########\n";
  cout << "#############################################\n";
  cout << "Number of processed events: " << fNEventsProcessed << "\n"
       << "Number of tracks: " << fNTracks << "\n"
       << "Number of steps: " << fNSteps << endl;
}
