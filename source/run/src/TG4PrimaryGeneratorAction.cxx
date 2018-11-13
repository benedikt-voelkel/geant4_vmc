//------------------------------------------------
// The Geant4 Virtual Monte Carlo package
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file TG4PrimaryGeneratorAction.cxx
/// \brief Implementation of the TG4PrimaryGeneratorAction class
///
/// \author I. Hrivnacova; IPN, Orsay

#include "TG4PrimaryGeneratorAction.h"
#include "TG4RunManager.h"
#include "TG4ParticlesManager.h"
#include "TG4TrackManager.h"
#include "TG4StateManager.h"
#include "TG4UserIon.h"
#include "TG4G3Units.h"
#include "TG4Globals.h"

#include <G4Event.hh>
#include <G4ParticleTable.hh>
#include <G4IonTable.hh>
#include <G4ParticleDefinition.hh>

#include <TVirtualMC.h>
#include <TVirtualMCApplication.h>
#include <TVirtualMCStack.h>
#include <TParticle.h>

// Moved after Root includes to avoid shadowed variables
// generated from short units names
#include <G4SystemOfUnits.hh>

//_____________________________________________________________________________
TG4PrimaryGeneratorAction::TG4PrimaryGeneratorAction()
  : TG4Verbose("primaryGeneratorAction")
{
/// Default constructor
}

//_____________________________________________________________________________
TG4PrimaryGeneratorAction::~TG4PrimaryGeneratorAction()
{
/// Destructor
}

//
// private methods
//

//_____________________________________________________________________________
void TG4PrimaryGeneratorAction::TransformPrimaries(G4Event* event)
{
/// Create a new G4PrimaryVertex objects for each TParticle
/// in the VMC stack.

  // Cache pointers to thread-local objects
  TVirtualMCStack* stack = gMC->GetStack();
  TG4ParticlesManager* particlesManager = TG4ParticlesManager::Instance();
  TG4TrackManager* trackManager = TG4TrackManager::Instance();

  // Get all particles from the stack
  // \note There might be
  // 1) primaries
  // 1.1) not yet transported
  // 1.2) already transported before in other engines until they were not respoonsible
  //      for this track due to selection criteria
  // 2) secondaries produced in this or another engine and will now again be
  //    transported by GEANT4 due to certain selection criteria
  // Although this method as well as their caller is supposed to only handle real
  // primaries it will be used to also deal with secondaries already processed/produced
  // by another engine
  // Check the number of particles on the stack
  G4int nofTracks = stack->GetNtrack();
  if (nofTracks <= 0) {
    TG4Globals::Exception(
      "TG4PrimaryGeneratorAction", "TransformPrimaries",
      "No primary particles found on the stack.");
  }

  if (VerboseLevel() > 1)
    G4cout << "TG4PrimaryGeneratorAction::TransformPrimaries: "
           << nofTracks << " particles" << G4endl;


  G4PrimaryVertex* previousVertex = 0;
  G4ThreeVector previousPosition = G4ThreeVector();
  G4double previousTime = 0.;
  // To reserve/resize and therefore prepare lists/vectors with proper size.
  trackManager->ExpectNewPrimaries(nofTracks);

  const TParticle* particle = nullptr;
  Int_t particleId = -1;
  Int_t geoStateIndex = -1;
  // Pop particles until the stack is empty and fill a G4Event
  // \note Don't use the following implementation
  //
  //  for (G4int i=0; i<nofTracks; i++) {
  //
  //    // get the particle from the stack
  //    TParticle* particle = mcStack->PopPrimaryForTracking(i);
  //    ...
  //    trackManager->AddPrimaryParticleId(i);
  //
  // Since at this point TGeant4 makes assumptions about the underlying structure
  // of the VMC stack namely that the ordering corresponds to the VMC track id
  // which does not need to be true. Again, popping the first primary does not imply it
  // has to have ID = 0 (or 1).
  while((particle = stack->PopNextTrack(particleId, geoStateIndex))) {
    // only particles that didn't die (decay) in primary generator
    // will be transformed to G4 objects

    // Pass this particle Id (in the VMC stack) to Track manager
    trackManager->NotifyOnNewVMCTrack(particleId, geoStateIndex);
    //G4cout << "Add track with ID " << particleId << " to GEANT4 stack" << G4endl;
    // Get particle definition from TG4ParticlesManager
    //
    G4ParticleDefinition* particleDefinition
      = particlesManager->GetParticleDefinition(particle, false);

    if (!particleDefinition) {
      TString text = "pdgEncoding=";
      text += particle->GetPdgCode();
      TG4Globals::Exception(
       "TG4PrimaryGeneratorAction", "TransformPrimaries",
       "G4ParticleTable::FindParticle() failed for " +
       TString(particle->GetName()) + "  "  + text + ".");
    }

    // Get/Create vertex
    G4ThreeVector position
      = particlesManager->GetParticlePosition(particle);
    G4double time = particle->T()*TG4G3Units::Time();
    G4PrimaryVertex* vertex;
    // \note i==0 implies previousVertex==0, so checking the latter is enough
    //if ( i==0 || previousVertex ==0 ||
    if ( previousVertex ==0 ||
         position != previousPosition || time != previousTime ) {
      // Create a new vertex
      // in case position and time of gun particle are different from
      // previous values
      // (vertex objects are destroyed in G4EventManager::ProcessOneEvent()
      // when event is deleted)
      vertex = new G4PrimaryVertex(position, time);
      event->AddPrimaryVertex(vertex);

      previousVertex = vertex;
      previousPosition = position;
      previousTime = time;
    }
    else
      vertex = previousVertex;

    // Create a primary particle and add it to the vertex
    // (primaryParticle objects are destroyed in G4EventManager::ProcessOneEvent()
    // when event and then vertex is deleted)
    G4ThreeVector momentum
      = particlesManager->GetParticleMomentum(particle);
    G4double energy
      = particle->Energy()*TG4G3Units::Energy();
    G4PrimaryParticle* primaryParticle
      = new G4PrimaryParticle(particleDefinition,
                              momentum.x(), momentum.y(), momentum.z(), energy);

    // Set charge
    G4double charge = particleDefinition->GetPDGCharge();
    if ( G4IonTable::IsIon(particleDefinition) &&
         particleDefinition->GetParticleName() != "proton" ) {
      // Get dynamic charge defined by user
      TG4UserIon* userIon = particlesManager->GetUserIon(particle->GetName(), false);
      if ( userIon ) charge = userIon->GetQ() * eplus;
    }
    primaryParticle->SetCharge(charge);

    // Set polarization
    TVector3 polarization;
    particle->GetPolarisation(polarization);
    primaryParticle
      ->SetPolarization(polarization.X(), polarization.Y(), polarization.Z());

    // Set weight
    G4double weight =  particle->GetWeight();
    primaryParticle->SetWeight(weight);

    // Add primary particle to the vertex
    vertex->SetPrimary(primaryParticle);

    // Verbose
    if (VerboseLevel() > 1) {
      G4cout << "Primary particle (VMC stack ID: " << particleId << "):" << G4endl;
      primaryParticle->Print();
    }
  }
}

//
// public methods
//

//_____________________________________________________________________________
void TG4PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
/// Generate primary particles by the selected generator.

  // Cache pointer to thread-local MC application
  TVirtualMCApplication* mcApplication = TVirtualMCApplication::Instance();

  // Begin of event
  TG4StateManager::Instance()->SetNewState(kInEvent);
  mcApplication->BeginEvent();

  // Update cached pointer to MC stack which is set to MC in some application
  // only in MCApplication::BeginEvent()
  // \todo That needs proper documentation
  TG4RunManager::Instance()->CacheMCStack();

  // Generate primaries and fill the VMC stack
  mcApplication->GeneratePrimaries();

  // Transform Root particle objects to G4 objects
  TransformPrimaries(event);
}
