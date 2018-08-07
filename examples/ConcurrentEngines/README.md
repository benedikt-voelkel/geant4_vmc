------------------------------------------------
 The Virtual Monte Carlo examples
 Copyright (C) 2007 - 2014 benedikt Volkel
 All rights reserved.

 For the licensing terms see geant4_vmc/LICENSE.
 Contact: root-vmc@cern.ch
-------------------------------------------------

# Concurrent Engines [WIP]
  Adapting example E01 to demonstrate the usage of GEANT3 and GEANT4 running concurrently.

  Demonstrates:
    Concurrently running GEANT3 and GEANT4. Each of them is responsible for different parts of the geometry.

  Important note: This is work in progress and things might not run as smoothly as expected.

# Running the example
Note, that macros as used in the other examples are disabled at the moment and the only way to run the example is by building and running the executable. Both `-DVMC_Geant4_DIR=$GEANT4_VMC_INSTALL_ROOT/lib64/Geant4VMC-<version>` and `-DGeant3_DIR=$GEANT3_INSTALL_ROOT/lib64/Geant3-<version>` need to be set when building with `cmake`. Further, the path to `Pythia6` might be required by `GEANT3` which is set via `-DPtyhia6_DIR`. If `GEANT4` was build with `VGM`, the path to these libraries needs to be given as well via `-DVGM_DIR=$VGM_INSTALL_ROOT/lib/VGM-<version>`.
Finally, the flag `-DVMC_CONCURRENT=ON` is required. Note, that this can be used concurrently with the option `-DVMC_WITH_Geant[3|4]=ON` in which case the ConcurrentEngines example is build and all other examples are built for `GEANT[3|4]`.
