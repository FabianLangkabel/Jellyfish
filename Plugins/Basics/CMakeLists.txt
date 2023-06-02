cmake_minimum_required(VERSION 2.6)

project(JellyfishPluginBasics CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/scratch/mmz/Libraries/QT/5.15.2/gcc_64/")

include_directories(
  "${CMAKE_SOURCE_DIR}/Libraries/eigen"
  "${CMAKE_SOURCE_DIR}/Libraries/libcint/include"
  "${CMAKE_SOURCE_DIR}/Libraries/muparser/include"
  "${CMAKE_SOURCE_DIR}/Libraries/OpenBLAS/include" 
  "${CMAKE_SOURCE_DIR}/Libraries/VTK/include"
  "${CMAKE_SOURCE_DIR}/Libraries/libzip/include" 
)

link_directories(
  "${CMAKE_SOURCE_DIR}/Libraries/libcint/lib64"
  "${CMAKE_SOURCE_DIR}/Libraries/muparser/lib64"
  "${CMAKE_SOURCE_DIR}/Libraries/OpenBLAS/lib64"
  "${CMAKE_SOURCE_DIR}/Libraries/VTK/lib64"
  "${CMAKE_SOURCE_DIR}/Libraries/libzip/lib64"
)

find_package(Qt6
  COMPONENTS
   Core
   Widgets
   Gui
   OpenGL
   Charts
)

if (NOT Qt6_FOUND)
  find_package(Qt5 5.13
    COMPONENTS
     Core
     Widgets
     Gui
     OpenGL
     Charts
  )
endif()

if (NOT (Qt6_FOUND OR Qt5_FOUND))
  message(FATAL_ERRROR "Qt libraries were not found.")
endif()

find_package(OpenMP REQUIRED)

if(WIN32)
  set(CXXFLAGS -O2 -s -Wall -Wextra ${OpenMP_CXX_FLAGS})
  set(LDLIBS -s pthread stdc++fs ole32 oleaut32 psapi advapi32)
else()
  set(CXXFLAGS -O2 -s -Wall -Wextra ${OpenMP_CXX_FLAGS})
  set(LDLIBS -s dl pthread stdc++fs)
endif()

if(MAX_PLUGINS_ARGS)
  add_definitions(-DMAX_PLUGINS_ARGS=${MAX_PLUGINS_ARGS})
endif()

if(NDEBUG)
  add_definitions(-DNDEBUG)
endif()

set(SOURCE_FILES
"plugin.cxx"
"save_load.cpp"

"Lib/src/gaussian_basisfunction.cpp"
"Lib/src/integrator_gaussian_potential_gaussian.cpp" 
"Lib/src/integrator_libcint_interface_1e.cpp"
"Lib/src/integrator_libcint_interface_2e.cpp"
"Lib/src/hartree_fock_closed_shell.cpp"
"Lib/src/transformation_integrals_basisfunction_to_hforbitals.cpp"
"Lib/src/transformation_integrals_hforbitals_to_spinorbitals.cpp"
"Lib/src/configuration_interaction.cpp"
"Lib/src/td_hamiltonian_molecular.cpp"
"Lib/src/propagator_matrixexponential.cpp"
"Lib/src/read_molecule.cpp"
"Lib/src/pointcharge_energy.cpp"
"Lib/src/CIS.cpp"
"Lib/src/configuration_interaction_dipole_moment.cpp"
"Lib/src/hartree_fock_closed_shell_dipole_moment.cpp"
"Lib/src/laser.cpp"
"Lib/src/nuclear_dipole_moment.cpp"
"Lib/src/propagator.cpp"
"Lib/src/configuration_interaction_one_electron_operator.cpp"
"Lib/src/operator_function.cpp"
"Lib/src/integrator_operator_function_numerical.cpp"
"Lib/src/one_electron_system.cpp"
"Lib/src/one_electron_system_dipole_moment.cpp"
"Lib/src/single_particle_transition_density_matrix.cpp"

"Nodes/ReadMoleculeNode/ReadMoleculeNode.cpp"
"Nodes/OneElectronIntegralsNode/OneElectronIntegralsNode.cpp"
"Nodes/OneElectronPotentialIntegralsNode/OneElectronPotentialIntegralsNode.cpp"
"Nodes/TwoElectronIntegralsNode/TwoElectronIntegralsNode.cpp"
"Nodes/CIEnergyCAPNode/CIEnergyCAPNode.cpp"
"Nodes/ConfigurationInteractionNode/ConfigurationInteractionNode.cpp"
"Nodes/RestrictedHartreeFockNode/RestrictedHartreeFockNode.cpp"
"Nodes/CombineOneElectronIntegralsNode/CombineOneElectronIntegralsNode.cpp"
"Nodes/CombineBasissetsNode/CombineBasissetsNode.cpp"
"Nodes/CreateEvenTemperedBasissetNode/CreateEvenTemperedBasissetNode.cpp"
"Nodes/CreateGaussPotentialNode/CreateGaussPotentialNode.cpp"
"Nodes/EvaluateNTONode/EvaluateNTONode.cpp"
"Nodes/GaussPotentialIntegralsNode/GaussPotentialIntegralsNode.cpp"
"Nodes/InputElectronNumberNode/InputElectronNumberNode.cpp"
"Nodes/IntegralsTransformationToHFNode/IntegralsTransformationToHFNode.cpp"
"Nodes/IntegralsTransformationToSpinNode/IntegralsTransformationToSpinNode.cpp"
"Nodes/PointchargeEnergyNode/PointchargeEnergyNode.cpp"
"Nodes/PropagatorNode/PropagatorNode.cpp"
"Nodes/PulsedLaserNode/PulsedLaserNode.cpp"
"Nodes/SolveOneElectronSystemNode/SolveOneElectronSystemNode.cpp"
"Nodes/SpartialCAPNode/SpartialCAPNode.cpp"
"Nodes/TransitionDipoleMomentsCINode/TransitionDipoleMomentsCINode.cpp"
"Nodes/TransitionDipoleMomentsOneElectronNode/TransitionDipoleMomentsOneElectronNode.cpp"

"Nodes/EvaluateNTONode/evaluate_ntos_window.cpp"
"Nodes/PropagatorNode/td_wf_analysis.cpp"
"Nodes/PulsedLaserNode/plot_laser.cpp"
"Nodes/TransitionDipoleMomentsCINode/get_transitions.cpp"
)

add_library(Basics SHARED ${SOURCE_FILES})
target_compile_options(Basics PUBLIC ${CXXFLAGS})
target_link_libraries(Basics ${LDLIBS})

target_link_libraries(Basics
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt::OpenGL
    Qt::Charts
)

target_link_libraries(Basics openblas)
target_link_libraries(Basics cint)

target_link_libraries(Basics zip)
target_link_libraries(Basics muparser)
target_link_libraries(Basics OpenMP::OpenMP_CXX)