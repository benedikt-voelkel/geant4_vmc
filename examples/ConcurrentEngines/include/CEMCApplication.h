#ifndef CE_MC_APPLICATION_H
#define CE_MC_APPLICATION_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file CEMCApplication.h
/// \brief Definition of the CEMCApplication class
///
/// Adapting example E01 to demonstrate the usage of GEANT3 and GEANT4 running concurrently.
///
/// \author B. Volkel; Heidelberg University

#include <TVirtualMCApplication.h>
#include <TVirtualMCStack.h>

class TVirtualMagField;

/// \ingroup ConcurrentEngines
/// \brief Implementation of the TVirtualMCApplication
///
/// \date 02/08/2018
/// \author B. Volkel; Heidelberg University

class CEMCApplication : public TVirtualMCApplication
{
  public:
    CEMCApplication(const char *name, const char *title);
    CEMCApplication();
    virtual ~CEMCApplication();

    // static access method
    static CEMCApplication* Instance();

    // methods
    void InitMC(const char *setup);
    void RunMC(Int_t nofEvents);
    void FinishRun();

    virtual TVirtualMCApplication* CloneForWorker() const;
    virtual void InitForWorker() const;
    virtual void ConstructGeometry();
    virtual void InitGeometry();
    virtual void GeneratePrimaries();
    virtual void BeginEvent();
    virtual void BeginPrimary();
    virtual void PreTrack();
    virtual void Stepping();
    virtual void PostTrack();
    virtual void FinishPrimary();
    virtual void FinishEvent();
    virtual void SetMCMediaProperties();

    // methods for tests
    void SetOldGeometry(Bool_t oldGeometry = kTRUE);
    void TestVMCGeometryGetters();

  private:
    // methods
    void ConstructMaterials();
    void ConstructVolumes();

    // data members
    TVirtualMCStack*  fStack;       ///< The VMC stack
    TVirtualMagField* fMagField;    ///< The magnetic field
    Int_t             fImedAr;      ///< The Argon gas medium Id
    Int_t             fImedAl;      ///< The Aluminium medium Id
    Int_t             fImedPb;      ///< The Lead medium Id

  ClassDef(CEMCApplication,1)  //Interface to MonteCarlo application
};

// inline functions

inline CEMCApplication* CEMCApplication::Instance()
{
  /// \return The MC application instance
  return (CEMCApplication*)(TVirtualMCApplication::Instance());
}

#endif //CE_MC_APPLICATION_H
