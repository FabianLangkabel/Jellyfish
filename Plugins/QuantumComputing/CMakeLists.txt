cmake_minimum_required(VERSION 2.6)

project(JellyfishPluginQuantumComputing CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/scratch/mmz/Libraries/QT/5.15.2/gcc_64/")

include_directories(
  "/usr/local/eigen"
  "/usr/local/libcint/include"
  "/usr/local/QuEST/include"
)

link_directories(
  "/usr/local/libcint/lib"
  "/usr/local/QuEST/lib"
)

find_package(Qt6
  COMPONENTS
   Core
   Widgets
   Gui
   OpenGL
   Charts
)

set(CXXFLAGS -O2 -s -Wall -Wextra)
set(LDLIBS -s dl pthread stdc++fs)

set(SOURCE_FILES
"plugin.cxx"
"save_load.cpp"

"Lib/src/laser.cpp"
"Lib/src/integrator_libcint_interface_1e.cpp"
"Lib/src/transformation_integrals_basisfunction_to_hforbitals.cpp"

"Lib/src/basics.cpp"
"Lib/src/hadamard_test_pauli_sum_quest.cpp"
"Lib/src/jordan_wigner.cpp"
"Lib/src/QC_advanced_operations.cpp"
"Lib/src/QC_QuEST_build_in.cpp"
"Lib/src/QC_Simulator_QuEST.cpp"
"Lib/src/quantum_phase_estimation_quest.cpp"
"Lib/src/quantum_phase_estimation.cpp"
"Lib/src/quantum_propagation_laser_cap_quest.cpp"
"Lib/src/quantum_propagation_laser_cap.cpp"

"Nodes/CreateStateFromCINode/CreateStateFromCINode.cpp"
"Nodes/JWEnergyCAPNode/JWEnergyCAPNode.cpp"
"Nodes/JWLaserNode/JWLaserNode.cpp"
"Nodes/JWTransformationNode/JWTransformationNode.cpp"
"Nodes/QPENode/QPENode.cpp"
"Nodes/QPEQuESTNode/QPEQuESTNode.cpp"
"Nodes/SimulatorQuESTNode/SimulatorQuESTNode.cpp"
"Nodes/TDExpHadamardTestQuESTNode/TDExpHadamardTestQuESTNode.cpp"
"Nodes/TDStatevectorToTDCIWFNode/TDStatevectorToTDCIWFNode.cpp"
"Nodes/TrotterPropagationNode/TrotterPropagationNode.cpp"
"Nodes/TrotterPropagationQuESTNode/TrotterPropagationQuESTNode.cpp"
)

add_library(QuantumComputing SHARED ${SOURCE_FILES})
target_compile_options(QuantumComputing PUBLIC ${CXXFLAGS})
target_link_libraries(QuantumComputing ${LDLIBS})

target_link_libraries(QuantumComputing
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt::OpenGL
    Qt::Charts
)
target_link_libraries(QuantumComputing openblas)
target_link_libraries(QuantumComputing cint)
target_link_libraries(QuantumComputing zip)
target_link_libraries(QuantumComputing QuEST)