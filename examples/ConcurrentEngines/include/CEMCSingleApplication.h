#ifndef CE_MC_SINGLE_APPLICATION_H
#define CE_MC_SINGLE_APPLICATION_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file CEMCSingleApplication.h
/// \brief Definition of the CEMCSingleApplication class
///
/// Adapting example E01 to demonstrate the usage of GEANT3 and GEANT4 running concurrently.
///
/// \author B. Volkel; Heidelberg University

#include <map> // For testing/providing mapping between VMC stack track and TGeoTrack

#include <TVirtualMCApplication.h>
#include <TVirtualMCStack.h>

class TVirtualMagField;
class TMCStackManager;
class TVirtualMC;

/// \ingroup ConcurrentEngines
/// \brief Implementation of the TVirtualMCApplication
///
/// \date 02/08/2018
/// \author B. Volkel; Heidelberg University

class CEMCSingleApplication : public TVirtualMCApplication
{
  public:
    CEMCSingleApplication(const char *name, const char *title);
    CEMCSingleApplication();
    virtual ~CEMCSingleApplication();

    // static access method
    static CEMCSingleApplication* Instance();

    // Export the geometry for further inspection
    void ExportGeometry(const char* path = ".") const;
    // Print a summary of the run status.
    void PrintStatus() const;

    /// Run the simulation chain
    void Run(Int_t nofEvents);

    //virtual TVirtualMCApplication* CloneForWorker() const override;
    virtual void InitForWorker() const  override;
    virtual void ConstructGeometry() override;
    virtual void InitGeometry() override;
    virtual void GeneratePrimaries() override;
    virtual void BeginEvent() override;
    virtual void BeginPrimary() override;
    virtual void PreTrack() override;
    virtual void Stepping() override;
    virtual void PostTrack() override;
    virtual void FinishPrimary() override;
    virtual void FinishEvent() override;

    // methods for tests
    void SetOldGeometry(Bool_t oldGeometry = kTRUE);

  private:
    // methods
    void ConstructMaterials();
    void ConstructVolumes();

    // data members
    TVirtualMC*       fCurrentMCEngine;          ///< Pointer to current MC engine
    TVirtualMagField* fMagField;                 ///< The magnetic field
    TMCStackManager*  fMCStackManager;           ///< Store pointer to global TMCStackManager
    TVirtualMCStack*  fStack;                    ///< VMC stack to push tracks to
    Int_t             fImedAr;                   ///< The Argon gas medium Id
    Int_t             fImedAl;                   ///< The Aluminium medium Id
    Int_t             fImedPb;                   ///< The Lead medium Id
    Int_t             fNEventsProcessed;         ///< Number of processed events
    Int_t             fCurrTrackId;              ///< Id of track currently processed
    Int_t             fCurrGeoTrackId;           ///< Current TGeoTrack id
    Int_t             fStackSize;                ///< Size of the stack
    Int_t             fNTracks;                  ///< Number of tracks processed
    Int_t             fNSecondaries;             ///< All secondaries
    Int_t             fNGeneratePrimaries;       ///< Monitor number of primary generation
    Bool_t            fDryRun;                   ///< Having a dryrun
    // /TVirtualMC*       fCurrentMCEngine
    std::map<Int_t,Int_t> fTrackIdToGeoTrackId;  ///< Map VMC stack track number to TGeoTrack id used for drawing tracks with TGeoManager
    std::map<Int_t,Int_t> fSteps;                ///< Monitor the number of steps for a given track number


  ClassDef(CEMCSingleApplication,1)  //Interface to MonteCarlo application
};

// inline functions

inline CEMCSingleApplication* CEMCSingleApplication::Instance()
{
  /// \return The MC application instance
  return (CEMCSingleApplication*)(TVirtualMCApplication::Instance());
}

#endif //CE_MC_APPLICATION_H
