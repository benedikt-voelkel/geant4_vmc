// $Id$

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007, 2008 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: vmc@pcroot.cern.ch
//-------------------------------------------------

/// \file Ex03MCApplication.cxx 
/// \brief Implementation of the Ex03MCApplication class 
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo
///
/// \date 06/03/2002
/// \author I. Hrivnacova; IPN, Orsay

#include <TROOT.h>
#include <TInterpreter.h>
#include <TVirtualMC.h>
#include <TRandom.h>
#include <TPDGCode.h>
#include <TVector3.h>
#include <Riostream.h>
#include <TGeoManager.h>
#include <TVirtualGeoTrack.h>

#include "Ex03MCApplication.h"
#include "Ex03MCStack.h"
#include "Ex03PrimaryGenerator.h"
#include "Ex03DetectorConstructionOld.h"

/// \cond CLASSIMP
ClassImp(Ex03MCApplication)
/// \endcond

//_____________________________________________________________________________
Ex03MCApplication::Ex03MCApplication(const char *name, const char *title,
                                     FileMode fileMode) 
  : TVirtualMCApplication(name,title),
    fPrintModulo(1),
    fEventNo(0),
    fVerbose(0),
    fStack(0),
    fDetConstruction(0),
    fCalorimeterSD(0),
    fPrimaryGenerator(0),
    fFieldB(0),
    fRootManager("example03", fileMode),
    fOldGeometry(kFALSE)
{
/// Standard constructor
/// \param name   The MC application name 
/// \param title  The MC application description
/// \param fileMode  Option for opening Root file (read or write mode)

  // Create a user stack
  fStack = new Ex03MCStack(1000);
  
  // Create detector construction
  fDetConstruction = new Ex03DetectorConstruction();
  
  // Create a calorimeter SD
  fCalorimeterSD = new Ex03CalorimeterSD("Calorimeter", fDetConstruction); 
  
  // Create a primary generator
  fPrimaryGenerator = new Ex03PrimaryGenerator(fStack);
  
  // Constant magnetic field (in kiloGauss)
  fFieldB = new Double_t[3];
  fFieldB[0] = 0.;
  fFieldB[1] = 0.;
  fFieldB[2] = 0.;
}

//_____________________________________________________________________________
Ex03MCApplication::Ex03MCApplication()
  : TVirtualMCApplication(),
    fPrintModulo(1),
    fEventNo(0),
    fStack(0),
    fDetConstruction(0),
    fCalorimeterSD(0),
    fPrimaryGenerator(0),
    fFieldB(0),
    fRootManager(),
    fOldGeometry(kFALSE)
{    
/// Default constructor
}

//_____________________________________________________________________________
Ex03MCApplication::~Ex03MCApplication() 
{
/// Destructor  
  
  delete fStack;
  delete fDetConstruction;
  delete fCalorimeterSD;
  delete fPrimaryGenerator;
  delete fFieldB;
  delete gMC;
  gMC = 0;
}

//
// private methods
//

//_____________________________________________________________________________
void Ex03MCApplication::RegisterStack()
{
/// Register stack in the Root manager.

  fRootManager.Register("stack", "Ex03MCStack", &fStack);   
}  

//
// public methods
//

//_____________________________________________________________________________
void Ex03MCApplication::InitMC(const char* setup)
{    
/// Initialize MC.
/// The selection of the concrete MC is done in the macro.
/// \param setup The name of the configuration macro 

  fVerbose.InitMC();

  gROOT->LoadMacro(setup);
  gInterpreter->ProcessLine("Config()");
 
  gMC->SetStack(fStack);
  gMC->Init();
  gMC->BuildPhysics(); 
  
  RegisterStack();
}                                   

//_____________________________________________________________________________
void Ex03MCApplication::RunMC(Int_t nofEvents)
{    
/// Run MC.
/// \param nofEvents Number of events to be processed

  fVerbose.RunMC(nofEvents);

  gMC->ProcessRun(nofEvents);
  FinishRun();
}

//_____________________________________________________________________________
void Ex03MCApplication::FinishRun()
{    
/// Finish MC run.

  fVerbose.FinishRun();

  fRootManager.WriteAll();
}

//_____________________________________________________________________________
void Ex03MCApplication::ReadEvent(Int_t i) 
{
/// Read \em i -th event and prints hits.
/// \param i The number of event to be read    

  fCalorimeterSD->Register();
  RegisterStack();
  fRootManager.ReadEvent(i);
}  
  
//_____________________________________________________________________________
void Ex03MCApplication::ConstructGeometry()
{    
/// Construct geometry using detector contruction class.
/// The detector contruction class is using TGeo functions or
/// TVirtualMC functions (if oldGeometry is selected)

  fVerbose.ConstructGeometry();

  if ( ! fOldGeometry ) {
    fDetConstruction->ConstructMaterials();  
    fDetConstruction->ConstructGeometry();  
    //TGeoManager::Import("geometry.root");
    //gMC->SetRootGeometry();
  }
  else {
    Ex03DetectorConstructionOld detConstructionOld;
    detConstructionOld.ConstructMaterials(); 
    detConstructionOld.ConstructGeometry();
  }    
}

//_____________________________________________________________________________
void Ex03MCApplication::InitGeometry()
{    
/// Initialize geometry
  
  fVerbose.InitGeometry();
  
  fDetConstruction->SetCuts();
  fCalorimeterSD->Initialize();
}

//_____________________________________________________________________________
void Ex03MCApplication::AddParticles()
{    
/// Example of user defined particle with user defined decay mode
  
  fVerbose.AddParticles();
  
  // Define particle
  gMC->DefineParticle(1000020050, "He5", kPTHadron,
                      5.03427 , 2.0, 0.002 , 
                      "Ion", 0.0, 0, 1, 0, 0, 0, 0, 0, 5, kFALSE); 

  // Define the 2 body  phase space decay  for He5
  Int_t mode[6][3];                  
  Float_t bratio[6];

  for (Int_t kz = 0; kz < 6; kz++) {
     bratio[kz] = 0.;
     mode[kz][0] = 0;
     mode[kz][1] = 0;
     mode[kz][2] = 0;
  }
  bratio[0] = 100.;
  mode[0][0] =2112;       // neutron 
  mode[0][1] =1000020040 ; // alpha

  gMC->SetDecayMode(1000020050 ,bratio,mode);
}

//_____________________________________________________________________________
void Ex03MCApplication::AddIons()
{    
/// Example of user defined ion
  
  fVerbose.AddIons();
  
  gMC->DefineIon("MyIon", 34, 70, 12, 0.); 

}

//_____________________________________________________________________________
void Ex03MCApplication::GeneratePrimaries()
{    
/// Fill the user stack (derived from TVirtualMCStack) with primary particles.
  
  fVerbose.GeneratePrimaries();

  TVector3 origin(fDetConstruction->GetWorldSizeX(),
                  fDetConstruction->GetCalorSizeYZ(),
                  fDetConstruction->GetCalorSizeYZ());
		     
  fPrimaryGenerator->GeneratePrimaries(origin);
}

//_____________________________________________________________________________
void Ex03MCApplication::BeginEvent()
{    
/// User actions at beginning of event

  fVerbose.BeginEvent();

  // Clear TGeo tracks (if filled)
  if (   TString(gMC->GetName()) == "TGeant3TGeo" && 
         gGeoManager->GetListOfTracks() &&
         gGeoManager->GetTrack(0) &&
       ((TVirtualGeoTrack*)gGeoManager->GetTrack(0))->HasPoints() ) {
       
       gGeoManager->ClearTracks();	  
       //if (gPad) gPad->Clear();	  
  }    

  fEventNo++;
  if (fEventNo % fPrintModulo == 0) { 
    cout << "\n---> Begin of event: " << fEventNo << endl;
    // ??? How to do this in VMC
    // HepRandom::showEngineStatus();
  }
}

//_____________________________________________________________________________
void Ex03MCApplication::BeginPrimary()
{    
/// User actions at beginning of a primary track

  fVerbose.BeginPrimary();
}

//_____________________________________________________________________________
void Ex03MCApplication::PreTrack()
{    
/// User actions at beginning of each track

  fVerbose.PreTrack();
}

//_____________________________________________________________________________
void Ex03MCApplication::Stepping()
{    
/// User actions at each step

  // Work around for Fluka VMC, which does not call
  // MCApplication::PreTrack()
  //
  static Int_t trackId = 0;
  if ( TString(gMC->GetName()) == "TFluka" &&
       gMC->GetStack()->GetCurrentTrackNumber() != trackId ) {
    fVerbose.PreTrack();
    trackId = gMC->GetStack()->GetCurrentTrackNumber();
  }      
    
  fVerbose.Stepping();

  fCalorimeterSD->ProcessHits();
}

//_____________________________________________________________________________
void Ex03MCApplication::PostTrack()
{    
/// User actions after finishing of each track

  fVerbose.PostTrack();
}

//_____________________________________________________________________________
void Ex03MCApplication::FinishPrimary()
{    
/// User actions after finishing of a primary track

  fVerbose.FinishPrimary();
}

//_____________________________________________________________________________
void Ex03MCApplication::FinishEvent()
{    
/// User actions after finishing of an event

  fVerbose.FinishEvent();

  // Geant3
  if (TString(gMC->GetName()) == "TGeant3") {
    // add scale (1.4)
    gMC->Gdraw("WRLD", 30., 30., 0, 10., 10., .75, .75);
  }  

  // Geant3 + TGeo
  // (use TGeo functions for visualization)
  if ( TString(gMC->GetName()) == "TGeant3TGeo") {
  
     // Draw volume 
     gGeoManager->SetVisOption(0);	 
     gGeoManager->SetTopVisible();
     gGeoManager->GetTopVolume()->Draw();

     // Draw tracks (if filled)
     if ( gGeoManager->GetListOfTracks() &&
          gGeoManager->GetTrack(0) &&
        ((TVirtualGeoTrack*)gGeoManager->GetTrack(0))->HasPoints() ) {
       
       gGeoManager->DrawTracks("/*");  // this means all tracks
          // Drawing G3 tracks via TGeo is available only
	  // if geant3 is compile with -DCOLLECT_TRACK flag
	  // (to be activated in geant3/TGeant3/TGeant3gu.cxx)
    }	  
  }    
 
  fRootManager.Fill();

  if (fEventNo % fPrintModulo == 0) 
    fCalorimeterSD->PrintTotal();

  fCalorimeterSD->EndOfEvent();

  fStack->Reset();
} 

//_____________________________________________________________________________
void Ex03MCApplication::Field(const Double_t* /*x*/, Double_t* b) const
{
/// Uniform magnetic field
/// \param b   The field value
  
   for (Int_t i=0; i<3; i++) b[i] = fFieldB[i];
}
 
