#ifndef Ex01_PARTICLE_H
#define Ex01_PARTICLE_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file  CEParticle.h
/// \brief Definition of the CEParticle class
///
/// Geant4 ExampleN01 adapted to Virtual Monte Carlo
///
/// \author I. Hrivnacova; IPN, Orsay

#include <TObject.h>

class TParticle;

/// \ingroup E01
/// \brief Extended TParticle with pointers to mother and daughter
/// particles
///
/// \date 05/04/2002
/// \author I. Hrivnacova; IPN, Orsay

class CEParticle : public TObject
{
  public:
    CEParticle(Int_t id, TParticle* particle);
    CEParticle(Int_t id, TParticle* particle, CEParticle* mother);
    CEParticle();
    virtual ~CEParticle();

    // methods
    void SetMother(CEParticle* particle);
    void AddDaughter(CEParticle* particle);

    // get methods
    Int_t         GetID() const;
    TParticle*    GetParticle() const;
    CEParticle* GetMother() const;
    Int_t         GetNofDaughters() const;
    CEParticle* GetDaughter(Int_t i) const;

  private:
    // data members
    Int_t         fID;        ///< The particle Id
    TParticle*    fParticle;  ///< The particle definition
    CEParticle* fMother;    ///< The particle mother
    TObjArray*    fDaughters; ///< The particle daughters

    ClassDef(CEParticle,1) // Extended TParticle
};

#endif //Ex01_PARTICLE_H
