#ifndef EXME_MC_APPLICATION_H
#define EXME_MC_APPLICATION_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file ExMEMCApplication.h
/// \brief Definition of the ExMEMCApplication class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo
///
/// \author I. Hrivnacova; IPN, Orsay

#include <initializer_list>

#include <TVirtualMCApplication.h>

#include "ExMEDetectorConstruction.h"
#include "ExMECalorimeterSD.h"

#include <TGeoUniformMagField.h>
#include <TMCVerbose.h>

class ExMEMCStack;
class ExMEPrimaryGenerator;

class TVirtualMCRootManager;

/// \ingroup EME
/// \brief Implementation of the TVirtualMCApplication
///
/// \date 06/03/2003
/// \author I. Hrivnacova; IPN, Orsay

class ExMEMCApplication : public TVirtualMCApplication
{
  public:
    ExMEMCApplication(const char* name,  const char *title,
                      Bool_t isMulti = kFALSE, Bool_t splitSimulation = kFALSE);
    ExMEMCApplication();
    virtual ~ExMEMCApplication();

    // methods
    void InitMC(const char *setup);
    void InitMC(std::initializer_list<const char*> setupMacros);
    void RunMC(Int_t nofEvents);
    void FinishRun();
    void ExportGeometry(const char* path) const;
    void ReadEvent(Int_t i);

    virtual TVirtualMCApplication* CloneForWorker() const;
    virtual void InitForWorker() const;
    virtual void FinishWorkerRun() const;

    virtual void ConstructGeometry();
    virtual void InitGeometry();
    virtual void AddParticles();
    virtual void AddIons();
    virtual void GeneratePrimaries();
    virtual void BeginEvent();
    virtual void BeginPrimary();
    virtual void PreTrack();
    virtual void Stepping();
    virtual void PostTrack();
    virtual void FinishPrimary();
    virtual void FinishEvent();

    // set methods
    void  SetPrintModulo(Int_t value);
    void  SetVerboseLevel(Int_t verboseLevel);
    void  SetControls(Bool_t isConstrols);
    void  SetField(Double_t bz);

    // get methods
    ExMEDetectorConstruction* GetDetectorConstruction() const;
    ExMECalorimeterSD*        GetCalorimeterSD() const;
    ExMEPrimaryGenerator*     GetPrimaryGenerator() const;

    // method for tests
    void SetOldGeometry(Bool_t oldGeometry = kTRUE);

  private:
    // methods
    ExMEMCApplication(const ExMEMCApplication& origin);
    void RegisterStack() const;

    // data members
    mutable TVirtualMCRootManager* fRootManager;//!< Root manager
    Int_t                     fPrintModulo;     ///< The event modulus number to be printed
    Int_t                     fEventNo;         ///< Event counter
    TMCVerbose                fVerbose;         ///< VMC verbose helper
    ExMEMCStack*              fStack;           ///< VMC stack
    ExMEDetectorConstruction* fDetConstruction; ///< Dector construction
    ExMECalorimeterSD*        fCalorimeterSD;   ///< Calorimeter SD
    ExMEPrimaryGenerator*     fPrimaryGenerator;///< Primary generator
    TGeoUniformMagField*      fMagField;        ///< Magnetic field
    Bool_t                    fOldGeometry;     ///< Option for geometry definition
    Bool_t                    fIsControls;      ///< Option to activate special controls
    Bool_t                    fIsMaster;        ///< If is on master thread
    Bool_t                    fIsMultiRun;      ///< Flag if having multiple engines
    Bool_t                    fSplitSimulation; ///< Split geometry given user criteria
    Int_t                     fG3Id;            ///< engine ID of Geant3
    Int_t                     fG4Id;            ///< engine ID of Geant4

    


  ClassDef(ExMEMCApplication,1)  //Interface to MonteCarlo application
};

// inline functions

/// Set the event modulus number to be printed
/// \param value  The new event modulus number value
inline void  ExMEMCApplication::SetPrintModulo(Int_t value)
{ fPrintModulo = value; }

/// Set verbosity
/// \param verboseLevel  The new verbose level value
inline void  ExMEMCApplication::SetVerboseLevel(Int_t verboseLevel)
{ fVerbose.SetLevel(verboseLevel); }

// Set magnetic field
// \param bz  The new field value in z
inline void  ExMEMCApplication::SetField(Double_t bz)
{ fMagField->SetFieldValue(0., 0., bz); }

/// \return The detector construction
inline ExMEDetectorConstruction* ExMEMCApplication::GetDetectorConstruction() const
{ return fDetConstruction; }

/// \return The calorimeter sensitive detector
inline ExMECalorimeterSD* ExMEMCApplication::GetCalorimeterSD() const
{ return fCalorimeterSD; }

/// \return The primary generator
inline ExMEPrimaryGenerator* ExMEMCApplication::GetPrimaryGenerator() const
{ return fPrimaryGenerator; }

/// Switch on/off the old geometry definition  (via VMC functions)
/// \param oldGeometry  If true, geometry definition via VMC functions
inline void ExMEMCApplication::SetOldGeometry(Bool_t oldGeometry)
{ fOldGeometry = oldGeometry; }

/// Switch on/off special process controls
/// \param isControls  If true, special process controls setting is activated
inline void ExMEMCApplication::SetControls(Bool_t isControls)
{ fIsControls = isControls; }

#endif //EXME_MC_APPLICATION_H
