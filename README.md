# Jellyfish

## Introduction
Jellyfish is a new program for performing and analyzing electron dynamics calculations. Currently, the focus is on the time-dependent configuration interaction method, however, other methods will be implemented in the future. Jellyfish is developed user-friendly by employing a graphical user interface based on a NodeEditor. It also is developer-friendly as modules can be added without the need to edit the Jellyfish core.


![alt text](https://github.com/FabianLangkabel/Jellyfish/Examples/Screenshot_1.png?raw=true)
![alt text](https://github.com/FabianLangkabel/Jellyfish/Examples/Screenshot_2.png?raw=true)

### JellyfishGUI and JellyfishCMD
JellyfishGUI is the main program in the Program suit. With the help of the user interface, new calculations can be set up, edited, evaluated and executed. Alternatively, all calculations can be performed via a console through JellyfishCMD, for the usage in a HPC environments without graphical output.


### Plugins
JellyfishGUI and JellyfishCMD only provide the user interface, like the file format etc., but not the actual methods/nodes. These are bundled in the form of plugins, compiled externally and then loaded into Jellyfish. Currently, 4 plugins are available, with their code located in the "Plugins" folder.

| Plugin | Description |
| ------ | ------ |
| Basics | Contains routines to perform and evaluate time-independent and time-dependent configuration interaction calculations. |
| Visualization | Allows visualization of orbitals, electron densities, and more from stationary and time-dependent calculations. |
| OrcaInterface | Allows to import CIS and LR-TDDFT outputs from the ORCA program. An ORCA input to generate a compatible output is given in the Examples folder for a TDDFT calculation. |
| QuantumCompute |  Provides transformation, algorithms and quantum computer simulator to perform electrondynamics calculations with quantum computer simulators. |

More details about the methods implemented so far, the structure of Jellyfish as well as the usage can be found in the publications listed below.

## Compilation/Installation


### Required libraries, tools and compilers
C++20 compiler \
[Cmake](https://cmake.org/) \
[Qt](https://www.qt.io/) (Version 5 or 6) \
[zlib](https://www.zlib.net/) \
[libzip](https://libzip.org/) \
[eigen](https://eigen.tuxfamily.org/index.php) (Library for linear algebra; required for all existing plugins) \
[libcint](https://github.com/sunqm/libcint) (Library for calculating GTO integrals; required by Basic, Visualization and QuantumComputing plugin; requires installation of a BLAS library) \
[muparser](https://beltoforion.de/en/muparser/) (Library for parsing mathematical equations; required for Basic Plugin) \
[QuEST](https://quest.qtechtheory.org/) (Library for simulating quantum computers; required for the QuantumComputing plugin) \
[VTK](https://vtk.org/) (library for visualization; required by VisualizationPlugin; optional QT, OpenGL and Chemistry modules are also required)

Libraries for plugins only need to be installed if the corresponding plugins are to be compiled or executed.

In addition, the following modified libraries are directly included in the code:
[JSON for Modern C++](https://github.com/nlohmann/json),
[stduuid](https://github.com/mariusbancila/stduuid),
[Qt Node Editor](https://github.com/paceholder/nodeeditor),
[C++ Plugin framework](https://github.com/djurodrljaca/CppPluginFramework)

### Procedure for compiling
For compiling, all necessary libraries must be installed and referenced in the CmakeLists of the individual components or plugins. 

To compile JellyfishGUI and JellyfishCMD you need to compile first the JellyfishCore while the plugins only depend on header files of JellyfishCore.

Once Jellyfish and plugins have been compiled, a folder named "Plugins" must be created in the JellyfishGUI and JellyfishCMD folders and the compiled plugins must be copied into these folders. Furthermore, if standard quantum chemistry basis sets are needed, the "basissets" folder from this repository must also be copied to the correponding JellyfishGUI and JellyfishCMD folders.

A detailed installation guide and optimized CMakeLists file will follow.

## Usage and examples
The "Examples" folder contains the following examples of different types of calculations. The use of the program is quite self-explanatory. Otherwise, more information can be found in the publications and more tutorials will follow.

| Example | Description |
| ------ | ------ |
| Guanine | Shows the Orca input of a TDDFT calculation on the example of Guanine, how the output is imported into Jellyfish and a subsequent dynamics calculation and analysis. |

## Contribution
Issues and pull requests can be submitted via Github. If you develop new plugins we can link them here. For further questions you can write a mail to: fabian.langkabel@helmholtz-berlin.de

## Publications
If you use Jellyfish please cite the following paper:

```
F. Langkabel, P. Krause, A. Bande, submitted
```

This paper also describes the structure and functionality of Jellyfish and introduces the existing plugins.

In addition, Jellyfish was used in the following papers:
[Making optical excitations visible – An exciton wavefunction extension to the time-dependent configuration interaction method](https://doi.org/10.1016/j.chemphys.2022.111502)
[Quantum-Compute Algorithm for Exact Laser-Driven Electron Dynamics in Molecules](https://doi.org/10.1021/acs.jctc.2c00878)