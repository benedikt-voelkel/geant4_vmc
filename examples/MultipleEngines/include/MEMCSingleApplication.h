#ifndef ME_MC_SINGLE_APPLICATION_H
#define ME_MC_SINGLE_APPLICATION_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file MEMCSingleApplication.h
/// \brief Definition of the MEMCSingleApplication class
///
/// Adapting example E01 to demonstrate the usage of GEANT3 and GEANT4 running concurrently.
///
/// \author B. Volkel; University Heidelberg

#include <map> // For testing/providing mapping between VMC stack track and TGeoTrack
#include <functional> // customInit in InitTransport
#include <TVirtualMCApplication.h>

class TVirtualMagField;
class MEMCSingleStack;

/// \ingroup ME
/// \brief Implementation of the TVirtualMCApplication
///
/// \date 10/12/2018
/// \author B. Volkel; University Heidelberg

class MEMCSingleApplication : public TVirtualMCApplication
{
  public:
    MEMCSingleApplication(const char *name, const char *title);
    MEMCSingleApplication();
    virtual ~MEMCSingleApplication();

    // static access method
    static MEMCSingleApplication* Instance();

    // Export the geometry for further inspection
    void ExportGeometry(const char* path = ".") const;
    // Print a summary of the run status.
    void PrintStatus() const;

    /// Init the VMC and simulation
    void InitTransport(std::function<void(TVirtualMC*)> customInit =
                                                            [](TVirtualMC*){});
    /// Run the simulation chain
    void RunTransport(Int_t nofEvents);

    virtual TVirtualMCApplication* CloneForWorker() const override;

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


  private:
    // methods
    void ConstructMaterials();
    void ConstructVolumes();
    MEMCSingleApplication(const MEMCSingleApplication& rhs);

    // data members
    TVirtualMagField* fMagField;                 ///< The magnetic field
    MEMCSingleStack*  fStack;                    ///< The stack
    Int_t             fImedAr;                   ///< The Argon gas medium Id
    Int_t             fImedAl;                   ///< The Aluminium medium Id
    Int_t             fImedPb;                   ///< The Lead medium Id
    Int_t             fNEventsProcessed;         ///< Number of processed events
    Int_t             fCurrTrackId;              ///< Id of track currently processed
    Int_t             fStackSize;                ///< Size of the stack
    Int_t             fNTracks;                  ///< Count tracks
    Int_t             fNSteps;                   ///< Count steps
    Int_t             fCurrNTracks;              ///< Count tracks per event
    Int_t             fCurrNSteps;               ///< Count steps per event

  ClassDefOverride(MEMCSingleApplication,1)  //Interface to MonteCarlo application
};

// inline functions

inline MEMCSingleApplication* MEMCSingleApplication::Instance()
{
  /// \return The MC application instance
  return (MEMCSingleApplication*)(TVirtualMCApplication::Instance());
}


#endif // ME_MC_APPLICATION_H
