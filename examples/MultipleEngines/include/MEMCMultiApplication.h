#ifndef ME_MC_MULTLI_APPLICATION_H
#define ME_MC_MULTLI_APPLICATION_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file MEMCMultiApplication.h
/// \brief Definition of the MEMCMultiApplication class
///
/// Adapting example E01 to demonstrate the usage of GEANT3 and GEANT4 running
/// sharing the transport of an event.
///
/// \author B. Volkel; University Heidelberg

#include <map> // For testing/providing mapping between VMC stack track
               // and TGeoTrack

#include <TVirtualMCMultiApplication.h>

class TVirtualMagField;
class MEMCMultiStack;

/// \ingroup ME
/// \brief Implementation of the TVirtualMCMultiApplication
///
/// \date 10/12/2018
/// \author B. Volkel; University Heidelberg


class MEMCMultiApplication : public TVirtualMCMultiApplication
{
  public:
    MEMCMultiApplication(const char *name, const char *title);
    MEMCMultiApplication();
    virtual ~MEMCMultiApplication();

    // static access method
    static MEMCMultiApplication* Instance();

    // Export the geometry for further inspection
    void ExportGeometry(const char* path = ".") const;
    // Print a summary of the run status.
    void PrintStatus() const;

    virtual TVirtualMCApplication* CloneForWorker() const override;

    //virtual TVirtualMCApplication* CloneForWorker() const override;
    virtual void InitForWorker() const  override;
    virtual void ConstructGeometryMulti() override;
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
    MEMCMultiApplication(const MEMCMultiApplication& rhs);

    // data members
    TVirtualMagField* fMagField;                 ///< The magnetic field
    MEMCMultiStack*   fStack;                    ///< The stack
    Int_t             fImedAr;                   ///< The Argon gas medium Id
    Int_t             fImedAl;                   ///< The Aluminium medium Id
    Int_t             fImedPb;                   ///< The Lead medium Id
    Int_t             fStackSize;                ///< Initial stack size
    Int_t             fNEventsProcessed;         ///< Number of processed events
    Int_t             fCurrTrackId;              ///< Id of track currently processed
    Int_t             fNTracks;                  ///< Count tracks
    Int_t             fNSteps;                   ///< Count steps
    Int_t             fCurrNTracks;              ///< Count tracks per event
    Int_t             fCurrNSteps;               ///< Count steps per event

  ClassDefOverride(MEMCMultiApplication,1)  //Interface to MonteCarlo application
};

// inline functions

inline MEMCMultiApplication* MEMCMultiApplication::Instance()
{
  /// \return The MC application instance
  return (MEMCMultiApplication*)(TVirtualMCMultiApplication::Instance());
}

#endif // ME_MC_MULTLI_APPLICATION_H
