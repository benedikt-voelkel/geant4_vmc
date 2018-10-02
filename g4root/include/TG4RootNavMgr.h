// @(#)root/g4root:$Id$
// Author: Andrei Gheata   07/08/06

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

/// \file TG4RootNavMgr.h
/// \brief Definition of the TG4RootNavMgr class
///
/// \author A. Gheata; CERN

#ifndef ROOT_TG4RootNavMgr
#define ROOT_TG4RootNavMgr

#ifndef ROOT_TObject
#include "TObject.h"
#endif

#include "G4Threading.hh"

class TGeoManager;
class TG4RootNavigator;
class TG4RootDetectorConstruction;
class TVirtualUserPostDetConstruction;

class G4TrackingManager;

/// \brief Manager class creating a G4Navigator based on a ROOT geometry.
///
/// \author A. Gheata; CERN

class TG4RootNavMgr : public TObject {

protected:
   TGeoManager          *fGeometry;   ///< Pointer to TGeo geometry
   TG4RootNavigator     *fNavigator;  ///< G4 navigator working with TGeo
   TG4RootDetectorConstruction *fDetConstruction; ///< G4 geometry built based on ROOT one
   TVirtualUserPostDetConstruction* fPostDetDetConstruction; ///< User defined initialization
   Bool_t                fConnected;  ///< Flags connection to G4

   TG4RootNavMgr();
   TG4RootNavMgr(TGeoManager *geom, TG4RootDetectorConstruction *detConstruction=0);

private:
   static G4ThreadLocal TG4RootNavMgr *fRootNavMgr; ///< Static pointer to singleton
   static TG4RootNavMgr *fgMasterInstance;

public:
   static TG4RootNavMgr *GetInstance(TGeoManager *geom=0);
   static TG4RootNavMgr *GetInstance(const TG4RootNavMgr& navMgr);
   static TG4RootNavMgr *GetMasterInstance();
   virtual ~TG4RootNavMgr();

   Bool_t                ConnectToG4();
   void                  Initialize(TVirtualUserPostDetConstruction *sdinit=0, Int_t nthreads=1);
   void                  LocateGlobalPointAndSetup(Double_t *pt, Double_t *dir=0);

   //Test utilities
   void                  PrintG4State() const;
   void                  SetVerboseLevel(Int_t level);

   void                  SetNavigator(TG4RootNavigator *nav);
                         /// Return the G4 navigator working with TGeo
   TG4RootNavigator     *GetNavigator() const {return fNavigator;}
                         /// Return the G4 geometry built based on ROOT one
   TG4RootDetectorConstruction *GetDetConstruction() const {return fDetConstruction;}

   /// Push current geometry status of the TGeoNavigator assuming it belongs to
   /// the G4Track currently processed.
   /// Wrapper around the TG4RootNavigator::SaveGeometryStatus()
   void SaveGeometryStatus();
   /// Notify the TG4RootNvMgr and the therefore the TG4RootNavigator about
   /// a geometry status which has been pushed to the TGeoNavigator passing
   /// the index returned by the TGeoNavigator and the corresponging G4Track
   /// object.
   /// Wrapper around the TG4RootNavigator::SaveGeometryStatus(Int_t G4TrackId, Int_t geoStateIndex)
   void SaveGeometryStatus(Int_t G4TrackId, Int_t geoStateIndex);
   /// Initialisation steps which are only possible when the G4EventManager
   /// and the G4TrackingManager have been instantiated.
   void SetG4TrackingManager(G4TrackingManager* trackingManager);
   /// Notify on new event
   void PrepareNewEvent();

   //ClassDef(TG4RootNavMgr,0)  // Class crreating a G4Navigator based on ROOT geometry
};
#endif
