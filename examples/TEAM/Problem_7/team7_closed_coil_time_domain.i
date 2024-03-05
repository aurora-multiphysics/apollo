[Mesh]
  type = ExclusiveMFEMMesh
  file = ./team7.g
[]

[Problem]
  type = MFEMProblem
  use_glvis = false
[]

[Formulation]
  type = AFormulation
  magnetic_vector_potential_name = magnetic_vector_potential
  magnetic_reluctivity_name = magnetic_reluctivity
  magnetic_permeability_name = magnetic_permeability
  electric_conductivity_name = electrical_conductivity

  magnetic_flux_density_name = magnetic_flux_density
  current_density_name = total_current_density
  external_current_density_name = source_current_density
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

[Functions]
  [current_magnitude]
    type = ParsedFunction
    value = Imax*cos(2.0*pi*freq*t)
    vars = 'freq Imax'
    vals = '200 2742.0'
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
  [source_current_density]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [source_electric_field]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [source_electric_potential]
    type = MFEMVariable
    fespace = H1FESpace
  []
  [total_current_density]
    type = MFEMVariable
    fespace = HDivFESpace
  []
[]

[Sources]
  [SourceCoil]
    type = MFEMClosedCoilSource
    total_current_coefficient = CurrentCoef
    source_current_density_gridfunction = source_current_density
    source_electric_field_gridfunction = source_electric_field
    hcurl_fespace = HCurlFESpace
    h1_fespace = H1FESpace
    coil_xsection_boundary = 7
    block = '3 4 5 6'
  []
[]

[Materials]
  [coil]
    type = MFEMConductor
    electrical_conductivity_coeff = PlateEConductivity
    electric_permittivity_coeff = PlatePermittivity
    magnetic_permeability_coeff = PlatePermeability
    block = '3 4 5 6'
  []
  [air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = '1'
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
  [CurrentCoef]
    type = MFEMFunctionCoefficient
    function = current_magnitude
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
