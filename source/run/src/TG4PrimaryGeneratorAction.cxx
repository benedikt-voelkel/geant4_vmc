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
#include <TMCManagerStack.h>
#include <TParticle.h>
#include <TMCParticleStatus.h>

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

  // Clear cached VMC stack info from former event
  TG4TrackManager* trackManager = TG4TrackManager::Instance();
  trackManager->PrepareNewEvent();

  // Cache pointers to thread-local objects
  TVirtualMCStack* mcStack = gMC->GetStack();
  TG4ParticlesManager* particlesManager = TG4ParticlesManager::Instance();

  G4int nofParticles = mcStack->GetNtrack();
  if (nofParticles <= 0) {
    TG4Globals::Warning(
      "TG4PrimaryGeneratorAction", "TransformPrimaries",
      "No primary particles found on the stack.");
  }

  if (VerboseLevel() > 1)
    G4cout << "TG4PrimaryGeneratorAction::TransformPrimaries: "
           << nofParticles << " particles" << G4endl;


  G4PrimaryVertex* previousVertex = 0;
  G4ThreeVector previousPosition = G4ThreeVector();
  G4double previousTime = 0.;

  for (G4int i=0; i<nofParticles; i++) {

    // get the particle from the stack
    TParticle* particle = mcStack->PopPrimaryForTracking(i);

    if (particle) {
      // only particles that didn't die (decay) in primary generator
      // will be transformed to G4 objects

      // Pass this particle Id (in the VMC stack) to Track manager
      trackManager->AddPrimaryParticleId(i);

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
      if ( i==0 || previousVertex ==0 ||
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
        G4cout << i << "th primary particle: " << G4endl;
        primaryParticle->Print();
      }
    }
  }
}


//_____________________________________________________________________________
void TG4PrimaryGeneratorAction::TransformTracks(G4Event* event)
{
/// Create a new G4PrimaryVertex objects for each TParticle
/// in the VMC stack.

  // Cache pointers to thread-local objects
  // TVirtualMCStack* mcStack = gMC->GetStack();
  // The TMCManagerStack has additional info on the current track status,
  // e.g. kinematics and geometry state.
  // Calling TG4PrimaryGeneratorAction::TransformTracks implies that
  // If it exists, it is the same as the TVirtualMCStack above
  TMCManagerStack* mcManagerStack = gMC->GetManagerStack();
  TG4ParticlesManager* particlesManager = TG4ParticlesManager::Instance();
  TG4TrackManager* trackManager = TG4TrackManager::Instance();
  trackManager->PrepareNewEvent();

  G4int nofParticles = mcManagerStack->GetNtrack();
  if (nofParticles <= 0) {
    TG4Globals::Warning(
      "TG4PrimaryGeneratorAction", "TransformPrimaries",
      "No primary particles found on the stack.");
  }

  if (VerboseLevel() > 1)
    G4cout << "TG4PrimaryGeneratorAction::TransformPrimaries: "
           << nofParticles << " particles" << G4endl;

  G4PrimaryVertex* previousVertex = 0;
  G4ThreeVector previousPosition = G4ThreeVector();
  G4double previousTime = 0.;

  const TParticle* particle = nullptr;

  Int_t trackId = -1;

  while((particle = mcManagerStack->PopNextTrack(trackId))) {

    if (particle) {
      const TMCParticleStatus* particleStatus = mcManagerStack->GetParticleStatus(trackId);
      const TLorentzVector& particlePosition = particleStatus->fCurrentPosition;
      const TLorentzVector& particleMomentum = particleStatus->fCurrentMomentum;
      const TVector3& polarization = particleStatus->fCurrentPolarization;
      const G4double& weight = particleStatus->fCurrentWeight;

      // Pass current status of the particle to the trackManager containing information
      // about potential made steps and track length != 0 in case the track was
      // transported before
      trackManager->AddParticleStatus(particleStatus);

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
      G4ThreeVector position(particlePosition.X()*TG4G3Units::Length(),
                             particlePosition.Y()*TG4G3Units::Length(),
                             particlePosition.Z()*TG4G3Units::Length());
      G4double time = particlePosition.T()*TG4G3Units::Time();
      G4PrimaryVertex* vertex;
      //if ( i==0 || previousVertex ==0 ||
      // i(aka trackId)==0 would, in the old logic, imply that previous vertex is 0
      // Here, the trackId can be different anything >= 0
      if (previousVertex ==0 ||
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
      G4ThreeVector momentum(particleMomentum.Px()*TG4G3Units::Energy(),
                             particleMomentum.Py()*TG4G3Units::Energy(),
                             particleMomentum.Pz()*TG4G3Units::Energy());
      G4double energy
        = particleMomentum.Energy()*TG4G3Units::Energy();
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
      primaryParticle
        ->SetPolarization(polarization.X(), polarization.Y(), polarization.Z());

      // Set weight
      primaryParticle->SetWeight(weight);

      // Add primary particle to the vertex
      vertex->SetPrimary(primaryParticle);

      // Verbose
      if (VerboseLevel() > 1) {
        G4cout << trackId << "th primary particle: " << G4endl;
        primaryParticle->Print();
      }
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

  TG4RunManager* runManager = TG4RunManager::Instance();
  // Cache pointer to thread-local MC application
  TVirtualMCApplication* mcApplication = TVirtualMCApplication::Instance();

  // Begin of event
  TG4StateManager::Instance()->SetNewState(kInEvent);

  // If TG4RunManager::IsInterruptibleEvent() rely on BeginEvent() has been
  // called already.
  if(!runManager->IsInterruptibleEvent()) {
    mcApplication->BeginEvent();
  }

  // Update cached pointer to MC stack which is set to MC in some application
  // only in MCApplication::BeginEvent()
  TG4RunManager::Instance()->CacheMCStack();

  // Don't generate primaries if this is a complex interruptible event
  if(runManager->UseExternalParticleGeneration()) {
    TransformTracks(event);
    return;
  }

  // Generate primaries and fill the VMC stack
  mcApplication->GeneratePrimaries();

  // Transform Root particle objects to G4 objects
  TransformPrimaries(event);
}
