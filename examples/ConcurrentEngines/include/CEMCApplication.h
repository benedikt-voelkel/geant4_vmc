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
class TMCStackManager;
class TVirtualMC;

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
    void InitMCs();
    void RunMCs(Int_t nofEvents);
    // Now used for cleaning up (especially TGeant4) after running single events
    void TerminateRun();
    // Export the geometry for further inspection
    void ExportGeometry(const char* path = ".") const;
    // Print a summary of the run status.
    void PrintStatus() const;
    // Populate the master stack
    void GeneratePrimariesMaster();
    // Initialise stacks by passing primaries from master stack to respective engine stacks
    void InitializeStacks();

    virtual TVirtualMCApplication* CloneForWorker() const;
    virtual void InitForWorker() const;
    virtual void ConstructGeometry();
    virtual void InitGeometry();
    virtual void GeneratePrimaries();
    virtual void BeginEvent();
    virtual void BeginPrimary();
    virtual void PreTrack();
    virtual void UserStepping();
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
    // Initialize SelectionCriteria ==> extracting volume ids
    void InitializeSelectionCriteria();

    // data members
    TVirtualMC*       fCurrentMCEngine;          ///< Pointer to current MC engine
    TVirtualMagField* fMagField;                 ///< The magnetic field
    TMCStackManager*  fMCStackManager;           ///< Store pointer to global TMCStackManager
    Int_t             fImedAr;                   ///< The Argon gas medium Id
    Int_t             fImedAl;                   ///< The Aluminium medium Id
    Int_t             fImedPb;                   ///< The Lead medium Id
    Int_t             fPreviousMed;              ///< The medium of the previous step
    Int_t             fTopID;                    ///< ID of top volume
    Int_t             fTrackerTubeID;            ///< volume ID fo the tracker tube
    Int_t             fNEventsProcessed;         ///< Number of processed events
    Int_t             fCurrTrackId;              ///< Id of track currently processed
    Int_t             fStackSize;                ///< Size of the stack
    Int_t             fNMovedTracks;             ///< Number of tracks moved from TGeant4 stack to TGeant3TGeo stack
    Int_t             fNTracksG3;                ///< Number of tracks processed by TGeant3TGeo
    Int_t             fNTracksG4;                ///< Number of tracks processed by TGeant4
    Int_t             fNSecondariesG3;           ///< Number of secondaries produced in TGeant3TGeo transport
    Int_t             fNSecondariesG4;           ///< Number of secondaries produced in TGeant4 transport
    Int_t             fNGeneratePrimaries;       ///< Monitor number of primary generation
    Bool_t            fDryRun;                   ///< Having a dryrun
    Bool_t            fChooseEngineForTopVolume; ///< false: Just keep the current engine, true: change engine
    // /TVirtualMC*       fCurrentMCEngine


  ClassDef(CEMCApplication,1)  //Interface to MonteCarlo application
};

// inline functions

inline CEMCApplication* CEMCApplication::Instance()
{
  /// \return The MC application instance
  return (CEMCApplication*)(TVirtualMCApplication::Instance());
}

#endif //CE_MC_APPLICATION_H
