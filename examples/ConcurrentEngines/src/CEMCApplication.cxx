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

using namespace std;

/// \cond CLASSIMP
ClassImp(CEMCApplication)
/// \endcond

//_____________________________________________________________________________
CEMCApplication::CEMCApplication(const char *name, const char *title)
  : TVirtualMCApplication(name,title),
    fStack(0),
    fMagField(0),
    fImedAr(0),
    fImedAl(0),
    fImedPb(0)
{
/// Standard constructor
/// \param name   The MC application name
/// \param title  The MC application description

  // create a user stack
  fStack = new CEMCStack(100);

  // create magnetic field (with zero value)
  fMagField = new TGeoUniformMagField();
}

//_____________________________________________________________________________
CEMCApplication::CEMCApplication()
  : TVirtualMCApplication(),
    fStack(0),
    fMagField(0),
    fImedAr(0),
    fImedAl(0),
    fImedPb(0)
{
/// Default constructor
}

//_____________________________________________________________________________
CEMCApplication::~CEMCApplication()
{
/// Destructor

  delete fStack;
  delete fMagField;
}

//
// private methods
//

void CEMCApplication::SetMCMediaProperties()
{
  cerr << "SKHJKADF" << endl;
}

//_____________________________________________________________________________
void CEMCApplication::ConstructMaterials()
{
/// Construct materials using TGeo modeller

  // Create Root geometry manager
  new TGeoManager("E01_geometry", "E01 VMC example geometry");

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

  // close geometry
  gGeoManager->CloseGeometry();

}

//
// public methods
//

//_____________________________________________________________________________
void CEMCApplication::InitMC(const char* setup)
{
/// Initialize MC.
/// The selection of the concrete MC is done in the macro.
/// \param setup The name of the configuration macro

  if ( TString(setup) != "" ) {
    gROOT->LoadMacro(setup);
    gInterpreter->ProcessLine("Config()");
    if ( ! gMC ) {
      Fatal("InitMC",
            "Processing Config() has failed. (No MC is instantiated.)");
    }
  }

  gMC->SetStack(fStack);
  gMC->SetMagField(fMagField);
  gMC->Init();
  gMC->BuildPhysics();
}

//__________________________________________________________________________
void CEMCApplication::RunMC(Int_t nofEvents)
{
/// Run MC.
/// \param nofEvents Number of events to be processed

  gMC->ProcessRun(nofEvents);
  FinishRun();
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

  cout << "Geometry will be defined via TGeo" << endl;
  ConstructMaterials();
  ConstructVolumes();
}

//_____________________________________________________________________________
void CEMCApplication::InitGeometry()
{
/// Initialize geometry.

  fImedAr = gMC->MediumId("ArgonGas");
  fImedAl = gMC->MediumId("Aluminium");
  fImedPb = gMC->MediumId("Lead");
}

//_____________________________________________________________________________
void CEMCApplication::GeneratePrimaries()
{
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

 // Add particle to stack
 fStack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx, poly, polz,
                  kPPrimary, ntr, 1., 0);

 // Change direction and add particle to stack
 px = 1.;
 py = 0.1;
 pz = 0.;
 fStack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx, poly, polz,
                  kPPrimary, ntr, 1., 0);

 // Change direction and add particle to stack
 px = 1.;
 py = 0.;
 pz = 0.1;
 fStack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx, vy, vz, tof, polx, poly, polz,
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
  gMC->TrackPosition(position);

  cout << "Track "
       << position.X() << " " << position.Y() << " " << position.Z()
       << "  in " <<  gMC->CurrentVolName() << "  ";

  if (gMC->CurrentMedium() == fImedAr) cout <<  "ArgonGas";
  if (gMC->CurrentMedium() == fImedAl) cout <<  "Aluminium";
  if (gMC->CurrentMedium() == fImedPb) cout <<  "Lead";

  cout << endl;


  // // Test other TVirtualMC::TrackPosition() functions

  // Double_t dx, dy, dz;
  // gMC->TrackPosition(dx, dy, dz);

  // Float_t x, y, z;
  // gMC->TrackPosition(x, y, z);

  // cout << "Track position (double): "  << dx << " " << dy << " " << dz
  //      << "  (float): "   << x << " " << y << " " << z << endl;
}

//_____________________________________________________________________________
void CEMCApplication::PostTrack()
{
/// User actions after finishing of each track
/// Nothing to be done this example
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
