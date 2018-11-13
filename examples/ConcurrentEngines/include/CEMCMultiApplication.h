#ifndef CE_MC_MULTLI_APPLICATION_H
#define CE_MC_MULTLI_APPLICATION_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file CEMCMultiApplication.h
/// \brief Definition of the CEMCMultiApplication class
///
/// Adapting example E01 to demonstrate the usage of GEANT3 and GEANT4 running concurrently.
///
/// \author B. Volkel; Heidelberg University

#include <map> // For testing/providing mapping between VMC stack track and TGeoTrack

#include <TVirtualMCMultiApplication.h>

class TVirtualMagField;
class CEMCStack;

/// \ingroup MultiEngines
/// \brief Implementation of the TVirtualMCApplication
///
/// \date 02/08/2018
/// \author B. Volkel; Heidelberg University

class CEMCMultiApplication : public TVirtualMCMultiApplication
{
  public:
    CEMCMultiApplication(const char *name, const char *title);
    CEMCMultiApplication();
    virtual ~CEMCMultiApplication();

    // static access method
    static CEMCMultiApplication* Instance();

    // Export the geometry for further inspection
    void ExportGeometry(const char* path = ".") const;
    // Print a summary of the run status.
    void PrintStatus() const;

    /// Run the simulation chain
    void Run(Int_t nofEvents);

    //virtual TVirtualMCApplication* CloneForWorker() const override;
    virtual void InitForWorker() const  override;
    virtual void ConstructGeometry() override;
    virtual void InitGeometryMulti() override;
    virtual void GeneratePrimariesMulti() override;
    virtual void BeginEventMulti() override;
    virtual void BeginPrimaryMulti() override;
    virtual void PreTrackMulti() override;
    virtual void SteppingMulti() override;
    virtual void PostTrackMulti() override;
    virtual void FinishPrimaryMulti() override;
    virtual void FinishEventMulti() override;

  private:
    // methods
    void ConstructMaterials();
    void ConstructVolumes();

    // data members
    TVirtualMagField* fMagField;                 ///< The magnetic field
    CEMCStack*        fStack;                    ///< The stack
    Int_t             fImedAr;                   ///< The Argon gas medium Id
    Int_t             fImedAl;                   ///< The Aluminium medium Id
    Int_t             fImedPb;                   ///< The Lead medium Id
    Int_t             fNEventsProcessed;         ///< Number of processed events
    Int_t             fCurrTrackId;              ///< Id of track currently processed
    Int_t             fCurrGeoTrackId;           ///< Current TGeoTrack id
    Int_t             fStackSize;                ///< Size of the stack
    Int_t             fNMovedTracks;             ///< Number of tracks moved from TGeant4 stack to TGeant3TGeo stack
    Int_t             fNTracksG3;                ///< Number of tracks processed by TGeant3TGeo
    Int_t             fNTracksG4;                ///< Number of tracks processed by TGeant4
    Int_t             fNSecondariesG3;           ///< Number of secondaries produced in TGeant3TGeo transport
    Int_t             fNSecondariesG4;           ///< Number of secondaries produced in TGeant4 transport
    Int_t             fNSecondaries;             ///< All secondaries
    Int_t             fNGeneratePrimaries;       ///< Monitor number of primary generation
    Bool_t            fDryRun;                   ///< Having a dryrun
    // /TVirtualMC*       fCurrentMCEngine
    std::map<Int_t,Int_t> fTrackIdToGeoTrackId;  ///< Map VMC stack track number to TGeoTrack id used for drawing tracks with TGeoManager
    std::map<Int_t,Int_t> fSteps;                ///< Monitor the number of steps for a given track number


  ClassDefOverride(CEMCMultiApplication,1)  //Interface to MonteCarlo application
};

// inline functions

inline CEMCMultiApplication* CEMCMultiApplication::Instance()
{
  /// \return The MC application instance
  return (CEMCMultiApplication*)(TVirtualMCMultiApplication::Instance());
}

#endif //CE_MC_MULTLI_APPLICATION_H
