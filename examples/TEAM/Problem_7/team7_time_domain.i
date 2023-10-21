[Mesh]
  type = ExclusiveMFEMMesh
  file = ./team7.g
[]

[Problem]
  type = MFEMProblem
[]

[Formulation]
  type = AFormulation
  magnetic_vector_potential_name = magnetic_vector_potential
  magnetic_reluctivity_name = magnetic_reluctivity
  magnetic_permeability_name = magnetic_permeability
  electric_conductivity_name = electrical_conductivity
[]

[FESpaces]
  [H1FESpace]
    type = MFEMFESpace
    fespace_type = H1
    order = FIRST
  []
  [HCurlFESpace]
    type = MFEMFESpace
    fespace_type = ND
    order = FIRST
  []
  [HDivFESpace]
    type = MFEMFESpace
    fespace_type = RT
    order = CONSTANT
  []
[]

[AuxVariables]
  [magnetic_vector_potential]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [magnetic_flux_density]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [current_density]
    type = MFEMVariable
    fespace = HDivFESpace
  []
[]

[Sources]
  [SourceCoil]
    type = MFEMDivFreeVolumetricSource
    function = RacetrackCoilCurrentFunction
    hcurl_fespace = HCurlFESpace
    h1_fespace = H1FESpace
    block = '3 4 5 6'
  []
[]

[Functions]
  [RacetrackCoilCurrentFunction]
    type = RacetrackCoilCurrentDensity
    coil_axis_x = 194e-3 # m
    coil_axis_y = 100e-3 # m
    coil_thickness = 50e-3 # m
    coil_current_magnitude = 2742.0 # Ampere-turns
    coil_xsection_area = 2.5e-3 # m^2
    frequency = 200.0 # Hz
  []
[]

[Materials]
  [air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = '1 3 4 5 6'
  []
  [plate]
    type = MFEMConductor
    electrical_conductivity_coeff = PlateEConductivity
    electric_permittivity_coeff = PlatePermittivity
    magnetic_permeability_coeff = PlatePermeability
    block = 2
  []
[]

[Coefficients]
  [AirEConductivity]
    type = MFEMConstantCoefficient
    value = 1.0 # S/m
  []
  [AirPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6 # T m/A
  []
  [AirPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0 # (dummy value for A form)
  []
  [PlateEConductivity]
    type = MFEMConstantCoefficient
    value = 3.526e7 # S/m
  []
  [PlatePermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6 # T m/A
  []
  [PlatePermittivity]
    type = MFEMConstantCoefficient
    value = 0.0 # (dummy value for A form)
  []
[]

[Executioner]
  type = Transient
  dt = 0.001 # s
  start_time = 0.0 # s
  end_time = 0.02 # s

  l_tol = 1e-16
  l_max_its = 1000
[]

[Outputs]
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/TEAM7ParaViewTimeDomain
  []
[]
