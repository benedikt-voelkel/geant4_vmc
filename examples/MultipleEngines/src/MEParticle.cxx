//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file MEParticle.cxx
/// \brief Implementation of the MEParticle class
///
/// Geant4 ExampleN01 adapted to Virtual Monte Carlo
///
/// \date 10/12/2018
/// \author B. Volkel; University Heidelberg

#include "MEParticle.h"

#include <TParticle.h>
#include <TObjArray.h>

/// \cond CLASSIMP
ClassImp(MEParticle)
/// \endcond

//_____________________________________________________________________________
MEParticle::MEParticle(Int_t id, TParticle* particle)
  : fID(id),
    fParticle(particle),
    fMother(0),
    fDaughters(0)
{
/// Standard constructor
/// \param  id        The particle id
/// \param  particle  The particle definition (TParticle)
}

//_____________________________________________________________________________
MEParticle::MEParticle(Int_t id, TParticle* particle, MEParticle* mother)
  : fID(id),
    fParticle(particle),
    fMother(mother),
    fDaughters(0)
{
/// Standard constructor
/// \param  id        The particle id
/// \param  particle  The particle definition (TParticle)
/// \param  mother    The particle mother
}

//_____________________________________________________________________________
MEParticle::MEParticle()
  : fID(0),
    fParticle(0),
    fMother(0),
    fDaughters(0)
{
/// Default constructor
}

//_____________________________________________________________________________
MEParticle::~MEParticle()
{
/// Destructor

  if (fDaughters) delete fDaughters;
}

// operators

// public methods

//_____________________________________________________________________________
void MEParticle::AddDaughter(MEParticle* particle)
{
/// Add particles daughter
/// \param particle  The daughter particle

  if (!fDaughters) fDaughters = new TObjArray();

  fDaughters->Add(particle);
}

//_____________________________________________________________________________
void MEParticle::SetMother(MEParticle* particle)
{
/// Set particle mother
/// \param  particle  The mother particle

  fMother = particle;
}

//_____________________________________________________________________________
Int_t  MEParticle:: GetID() const
{
/// \return The particle Id.

  return fID;
}


//_____________________________________________________________________________
TParticle*  MEParticle::GetParticle() const
{
/// \return The particle definition (TParticle).

  return fParticle;
}

//_____________________________________________________________________________
MEParticle* MEParticle::GetMother() const
{
/// \return The particle mother.

  return fMother;
}

//_____________________________________________________________________________
Int_t MEParticle::GetNofDaughters() const
{
/// \return The number of daughters.

  if (!fDaughters) return 0;

  return fDaughters->GetEntriesFast();
}

//_____________________________________________________________________________
MEParticle* MEParticle::GetDaughter(Int_t i) const
{
/// \return   \em i -th daughter
/// \param i  The daughter index

  // add test if i

  return (MEParticle*) fDaughters->At(i);
}
