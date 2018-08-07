//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file CEParticle.cxx
/// \brief Implementation of the CEParticle class
///
/// Geant4 ExampleN01 adapted to Virtual Monte Carlo
///
/// \date 05/04/2002
/// \author I. Hrivnacova; IPN, Orsay

#include "CEParticle.h"

#include <TParticle.h>
#include <TObjArray.h>

/// \cond CLASSIMP
ClassImp(CEParticle)
/// \endcond

//_____________________________________________________________________________
CEParticle::CEParticle(Int_t id, TParticle* particle)
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
CEParticle::CEParticle(Int_t id, TParticle* particle, CEParticle* mother)
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
CEParticle::CEParticle()
  : fID(0),
    fParticle(0),
    fMother(0),
    fDaughters(0)
{
/// Default constructor
}

//_____________________________________________________________________________
CEParticle::~CEParticle()
{
/// Destructor

  if (fDaughters) delete fDaughters;
}

// operators

// public methods

//_____________________________________________________________________________
void CEParticle::AddDaughter(CEParticle* particle)
{
/// Add particles daughter
/// \param particle  The daughter particle

  if (!fDaughters) fDaughters = new TObjArray();

  fDaughters->Add(particle);
}

//_____________________________________________________________________________
void CEParticle::SetMother(CEParticle* particle)
{
/// Set particle mother
/// \param  particle  The mother particle

  fMother = particle;
}

//_____________________________________________________________________________
Int_t  CEParticle:: GetID() const
{
/// \return The particle Id.

  return fID;
}


//_____________________________________________________________________________
TParticle*  CEParticle::GetParticle() const
{
/// \return The particle definition (TParticle).

  return fParticle;
}

//_____________________________________________________________________________
CEParticle* CEParticle::GetMother() const
{
/// \return The particle mother.

  return fMother;
}

//_____________________________________________________________________________
Int_t CEParticle::GetNofDaughters() const
{
/// \return The number of daughters.

  if (!fDaughters) return 0;

  return fDaughters->GetEntriesFast();
}

//_____________________________________________________________________________
CEParticle* CEParticle::GetDaughter(Int_t i) const
{
/// \return   \em i -th daughter
/// \param i  The daughter index

  // add test if i

  return (CEParticle*) fDaughters->At(i);
}
