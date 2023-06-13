[Mesh]
  type = ExclusiveMFEMMesh
  file = ./team7.g
[]

[Problem]
  type = MFEMProblem
  formulation = AForm
  use_glvis = true
[]

[AuxVariables]
  [./magnetic_vector_potential]
    type = MFEMVariable
    fespace_name = _HCurlFESpace
    fespace_type = ND
    order = first
  [../]
  [./magnetic_flux_density]
    type = MFEMVariable
    fespace_name = _HDivFESpace
    fespace_type = RT
    order = first
  [../]
  [./electric_potential]
    type = MFEMVariable
    fespace_name = _H1FESpace
    fespace_type = H1
    order = second
  [../]
[]

[UserObjects]
  [./SourceCoil]
    type = MFEMDivFreeVolumetricSource
    function = RacetrackCoilCurrentFunction
    block = '3 4 5 6'
  [../]
[]

[Functions]
  [./RacetrackCoilCurrentFunction]
    type = RacetrackCoilCurrentDensity
    coil_axis_x = 194e-3 
    coil_axis_y = 100e-3
    coil_thickness = 50e-3
    coil_current_magnitude = 2742.0
    coil_xsection_area = 2.5e-3
    frequency = 200.0
  [../]
[]

[Materials]
  [./air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = '1 3 4 5 6'
  [../]
  [./plate]
    type = MFEMConductor
    electrical_conductivity_coeff = PlateEConductivity
    electric_permittivity_coeff = PlatePermittivity
    magnetic_permeability_coeff = PlatePermeability
    block = 2
  [../]
[]

[UserObjects]
  [./AirEConductivity]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]
  [./AirPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  [../]
  [./AirPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  [../]
  [./PlateEConductivity]
    type = MFEMConstantCoefficient
    value = 3.526e7
  [../]
  [./PlatePermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  [../]
  [./PlatePermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.001
  start_time = 0.0
  end_time = 0.02

  l_tol = 1e-16
  l_max_its = 1000
[]

[Outputs]
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/TEAM7VisIt
  []
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/TEAM7ParaView
    refinements = 1
    high_order_output = true
  []
[]
