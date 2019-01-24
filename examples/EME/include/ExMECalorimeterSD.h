#ifndef EXME_CALORIMETER_SD_H
#define EXME_CALORIMETER_SD_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file  ExMECalorimeterSD.h
/// \brief Definition of the ExMECalorimeterSD class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo: \n
/// Id: ExN03CalorimeterSD.hh,v 1.4 2002/01/09 17:24:11 ranjard Exp
/// GEANT4 tag Name: geant4-05-00
///
/// \author I. Hrivnacova; IPN, Orsay

#include <TNamed.h>
#include <TClonesArray.h>

class ExMEDetectorConstruction;
class ExMECalorHit;
class TVirtualMC;

/// \ingroup EME
/// \brief The calorimeter sensitive detector
///
/// \date 06/03/2003
/// \author I. Hrivnacova; IPN, Orsay

class ExMECalorimeterSD : public TNamed
{
  public:
    ExMECalorimeterSD(const char* name,
                      ExMEDetectorConstruction* detector);
    ExMECalorimeterSD(const ExMECalorimeterSD& origin,
                      ExMEDetectorConstruction* detector);
    ExMECalorimeterSD();
    virtual ~ExMECalorimeterSD();

    // methods
    void    Initialize();
    Bool_t  ProcessHits();
    void    EndOfEvent();
    void    Register();
    virtual void  Print(Option_t* option = "") const;
    void    PrintTotal() const;


    // set methods
    void SetVerboseLevel(Int_t level);

    // get methods
    ExMECalorHit* GetHit(Int_t i) const;

  private:
    // methods
    void  ResetHits();

    // data members
    TVirtualMC*    fMC;            ///< The VMC implementation
    ExMEDetectorConstruction*  fDetector; ///< Detector construction
    TClonesArray*  fCalCollection; ///< Hits collection
    Int_t          fAbsorberVolId; ///< The absorber volume Id
    Int_t          fGapVolId;      ///< The gap volume Id
    Int_t          fVerboseLevel;  ///< Verbosity level

  ClassDef(ExMECalorimeterSD,1) //ExMECalorimeterSD

};

/// Set verbose level
/// \param level The new verbose level value
inline void ExMECalorimeterSD::SetVerboseLevel(Int_t level)
{ fVerboseLevel = level; }

#endif //EXME_CALORIMETER_SD_H
