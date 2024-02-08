# EMSIM

EMSim is a library that computes different electro-magnetic effects
like LPF and VSD.

## Features

EMSim provides the following functionality:

In order to compute LFP or VSD, there are a few common initial steps:

1. Load the circuit data from a circuit file
2. Create all the static morphology geometry
3. Apply current and voltage report on the geometry
4. Finally, specific calculations are done to compute LFP or VSD

The LFP part of EMSim computes the electric field in the space between neurons, using the following method:

1. Load and place all segments in 3D space
2. Load the current report for each segment and for every simulation frame
3. Create a volume with the specified resolution (# of voxels)
4. Compute the value for each voxel, using the current point model (one can also compute the values of some sample points instead of the full volume)

The VSD part of EMSim simulates the processes of a voltage sensitive dye, using the following method:

1. Load and place all segments in 3D space
2. Load the voltage report for each segment and for every simulation frame
3. Create a volume with the specified resolution (# of voxels)
4. For each voxel, the value is accumulated
5. Create a projection of the volume into a 2D surface

## Installation

With a proper installation of [Brion](https://github.com/BlueBrain/brion), installation can be done with CMake:

```
$ mkdir build
$ cd build
$ cmake .. -GNinja
$ ninja
```

See the [CI plan](https://github.com/BlueBrain/EMSim/blob/master/.github/workflows/run-tests.yml) for more details.

## Usage

### emsim
The `emsim` program has the following options:

```
  -i [ --input ] arg    Path to Blueconfig file.
  -o [ --output ] arg   Path for the output file.
  --target arg          The circuit's target.
  --report arg          The name of the report.
  --start-time arg      The start time
  --end-time arg        The end time
  --fraction arg        Specify the fraction [0.0 1.0] of gids to be used
                        during the computation. Default is 1.0.
  --export-volume       Will export a floating point volume for each time
                        steps.
  --voxel-size arg      The size in each dimension of a voxel in circuit units.
                        Default is 4.0,4.0,4.0. Must be written in the form:
                        --voxel-size rx,ry,rz
  --volume-extent arg   Specify an additional 3d extent for the volume in
                        micrometers. Default is 0.0,0.0,0.0. Must be written in
                        the form: --volume-extent ex,ey,ez
  --sample-point arg    The x y z positions of a sample point. Must be written
                        in the form: --sample-point x,y,z
```

So, for example, to compute the LFP values on 2 probes:
```
    emsim                        \
        -i blueconfigFile        \
        -o outputFileName        \
        --target cuirtcuitTarget \
        --report currentReport   \
        --sample-point 12,34,32  \
        --sample-point 43,56,43  \
```

### emsimVSD

The `emsim` program has the following options:

```
  -i [ --input ] arg                   Path to Blueconfig file.
  -o [ --output ] arg                  Name of the output volume (if
                                       'export-volume' specified) and 2D image
                                       files
  --target arg                         The circuit's target.
  --report-voltage arg                 The name of the voltage report in the BlueConfig.
  --report-area arg                    The name of the area report in the BlueConfig.
  --sensor-res arg (=512)              Number of pixels per side of the square
                                       sensor.
  --sensor-dim arg (=1000)             Length of side of the square sensor in
                                       micrometers.
  --curve arg                          Path to the dye curve file (default: no
                                       attenuation)
  --start-time arg                     The start time of the simulation
  --end-time arg                       The end time of the simulation
  --time-step arg                      The time between frames, in milliseconds
  --fraction arg                       Specify the fraction [0.0 1.0] of gids
                                       to be used during the computation.
                                       Default is 1.0.
  --export-volume                      Will export a floating point volume for
                                       each time steps.
  --depth arg (=2081.7561)             Depth of the attenuation curve area of
                                       influence. It also defines the
                                       Y-coordinate at which it starts being
                                       applied down until y=0 (default:
                                       2081.756 micrometers).
  --interpolate-attenuation            Will interpolate the attenuation curve.
  --sigma arg (=0.00449999981)         Absorption + scattering coefficient
                                       (units per micrometer) in the
                                       Beer-Lambert law. Must be a positive
                                       value (default: 0.0045).
  --v0 arg (=-65)                      Resting potential (default: -65 mV).
  --g0 arg (=0)                        Multiplier for surface area in
                                       background fluorescence term.
  --ap-threshold arg (=3.40282347e+38) Action potential threshold in
                                       millivolts.
  --soma-pixels                        Produce a text file containing the GIDs
                                       loaded and their corresponding 3D
                                       positions and indices in the resulting
                                       2D image.
```

For example, to compute the VSD with 1000um sensor size with a 512 resolution:

```
    emsimVSD                           \
        -i blueconfigFile              \
        -o outputFileName              \
        --target cuirtcuitTarget       \
        --report-voltage voltageReport \
        --report-area areaReport       \
        --sensor-dim 1000              \
        --sensor-res 512
```

## Input Formats

###  VSD Curve File

When using the `--curve` option, a file must be supplied.
This is a depth-dependent dye attenuation curve file that contains a column of floating point values, one for every micrometer specified by the `--depth` argument


## Output Format

### LFP

#### export-volume
Triggered by the `export-volume` option.

#### sample-point

This writes a file called `$output$_sample_points` where `$output$` is the value of the `--output` argument.
Lines in this file starting with `#` are comments;
For each frame in the report a line is generated with the following format:

```
timestamp samplePoint1 samplePoint2 ... samplePointN
```

Where the `samplePointN` is the voltage at that point.

#### export-volume
Triggered by the `export-volume` option.

This option writes two files (where `$output$` is the value of the `--output` argument):
* a `raw` file called: `$output$_volume_floats$timestamp$.raw` for each frame, containing the LFP voxel values for each voxel
* a txt file called `$output$_volume_info_$timestamp$.txt` for each frame.

### VSD

* `$output$_image_floats_$timestamp$.raw`, containing the raw binary data of the 2 dimensional viewport.
* an `MHD` file for each frame called `$output$_image_floats_$timestamp$.mhd`, containg the header information for the above.

#### volume-info
This option writes two files (where `$output$` is the value of the `--output` argument):
* `$output$_volume_floats_$timestamp$.raw`, much like the one from LFP.
* an `MHD` file for each frame called `$output$_volume_floats_$timestamp$.mhd`, containing the header imformation for the above.

## Acknowledgement & Funding

This project was supported by funding to the Blue Brain Project, a research
center of the École polytechnique fédérale de Lausanne (EPFL), from the Swiss
government's ETH Board of the Swiss Federal Institutes of Technology.

Copyright (c) 2017-2024 Blue Brain Project/EPFL
