------------------------------------------------
 The Virtual Monte Carlo examples
 Copyright (C) 2007 - 2014 benedikt Volkel
 All rights reserved.

 For the licensing terms see geant4_vmc/LICENSE.
 Contact: root-vmc@cern.ch
-------------------------------------------------

# Concurrent Engines [WIP]
  Adapting example E01 to demonstrate sharing the transport among `GEANT3` and `GEANT4`.

  Demonstrates:
    Running `GEANT3` and `GEANT4` sharing particle transport. Each of them is responsible for different parts of the geometry.

  Important note: This is work in progress.

# Sharing the transport based on geometry boundaries
The geometry consists of a dummy tracker tube and a sampling calorimeter. If two engines are running the first part up to the tracker tube is done by `engine1`. The track is stopped at the boundary and transferred to `engine2` being responsible for the tracker. Afterwards, the track is given back to `engine1`. If there are particles reflected after the tracker tube entering it again, these tracks are transported by `engine2`. So any track entering the tracker tube is simulated by `engine2` and anything else is taken care of by `engine1`.

![Shared geometry figure](misc/share_geometry.png?raw=true "Shared geometry during particle transport")


# Building and running the example

## Prerequisites
* ROOT at https://github.com/benedikt-voelkel/root/tree/v6-14-06-multi-engines-wip
* GEANT4_VMC at https://github.com/benedikt-voelkel/geant4_vmc/tree/multi-engines (this)
* GEANT3 at https://github.com/benedikt-voelkel/geant3/tree/multi-engines

## Build GEANT4_VMC

### Using aliBuild
You can install everything using the [ALICE build system aliBuild](https://github.com/alisw/alibuild). The required recipe and the default file can be found at https://github.com/benedikt-voelkel/alidist/tree/multi-engines. Then run
```bash
aliBuild build multi-engines --defaults multi-engines
```
This also takes care of disabling the GEANT4 multithreading (see [below](#Comments)).

### Manually
Follow the normal build procedure of all three packages, however, `GEANT4` multithreading needs to be disabled and not used as default.

## Build the example
From the build directory run
```bash
cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR -DPythia6_DIR=$PYTHIA6_ROOT/ -DGeant3_DIR=$GEANT3_ROOT/lib64/Geant3-2.5.0 -DGeant4VMC_DIR=$GEANT4_VMC_ROOT/lib/Geant4VMC-3.5.0 [-DVGM_DIR=$VGM_ROOT/lib/VGM-4.4.0] -DVMC_MULTI=ON $GEANT4_VMC_EXAMPLE_DIR
```
`VMG` will be needed in case `GEANT4` was built with `VGM` support. The option `VMC_MULTI` is required to steer the built with both `TGeant3` and `TGeant4` since by default the examples are build either with `TGeant3` or `TGeant4`.

## Run the example
Export the required shared library paths together with `$INSTALL_DIR/lib64` and then run
```bash
$> $INSTALLDIR/vmc_exampleCE [ <numberOfEvents>  [ <engine1> [<engine2>] ] ]
```
`<engine1>` and `<engine2>` directly correspond to the naming in the above sketch. If only `<engine1>` is specified it is a normal run with just engine. If `<engine1>==<engine2>` it is basically the same as running only one engine, however, there is a little overhead since the routine for transferring tracks is executed but skipped as soon it realizes the engine it should change to is the same which is running. This is done this way to show that there will be no logical bug in case of running one engine in a multi-run.
If no arguments are specified, the defaults are 2 events and `TGeant3` as first and `TGeant4` as second engine, respectively. If only the number of events is given, `TGeant3` is taken as first and `TGeant4` as second engine, respectively.

# Comments

## Disable multithreading for GEANT4
Multithreading must be disabled for `GEANT4` since in case of a multi-run that would mean to have `TGeant3` on multiple threads together with `TGeant4` which is not possible. Keeping `TGeant3` on one worker thread requires a more complex VMC stack management. But because TVirtualMCMultiStack is responsible to distribute tracks to multiple engines that needs further development.

## Neutrinos not skipped by GEANT4
There is apparently an issue such that neutrinos are not skipped although that is specified in [](examples/MultipleEngines/exampleME.cxx). In the stepping you might hence see PDG IDs according to |PDG_ID| in \{12,14,16\} and transport might take significantly longer. That is currently investigated.
