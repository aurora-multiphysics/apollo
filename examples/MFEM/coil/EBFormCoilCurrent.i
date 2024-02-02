[Mesh]
  type = ExclusiveMFEMMesh
  file = ./coil.gen
  dim = 3
[]

[Problem]
  type = MFEMProblem
  use_glvis = false
[]

[Formulation]
  type = EBFormulation
  e_field_name = electric_field
  b_field_name = magnetic_flux_density
  magnetic_reluctivity_name = magnetic_reluctivity
  magnetic_permeability_name = magnetic_permeability
  electric_conductivity_name = electrical_conductivity

  current_density_name = total_current_density
  # external_current_density_name = source_current_density
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
  [electric_field]
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

[Functions]
  [current_magnitude]
    type = ParsedFunction
    expression = cos(2.0*pi*freq*t)
    symbol_names = 'freq'
    symbol_values = '0.01666667'
  []
[]

[BCs]
  [tangential_E_bc]
    type = MFEMVectorDirichletBC
    variable = electric_field
    vector_coefficient = TangentialECoef
    boundary = '1 2 4'
  []
[]

[Materials]
  [coil]
    type = MFEMConductor
    electrical_conductivity_coeff = CoilEConductivity
    electric_permittivity_coeff = CoilPermittivity
    magnetic_permeability_coeff = CoilPermeability
    block = 1
  []
  [air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = 2
  []
  [core]
    type = MFEMConductor
    electrical_conductivity_coeff = CoreEConductivity
    electric_permittivity_coeff = CorePermittivity
    magnetic_permeability_coeff = CorePermeability
    block = 3
  []
[]

[VectorCoefficients]
  [TangentialECoef]
    type = MFEMVectorConstantCoefficient
    value_x = 0.0
    value_y = 0.0
    value_z = 0.0
  []
[]

[Coefficients]
  [CoilEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185
  []
  [CoilPermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  []
  [CoilPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []

  [AirEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185e-6
  []
  [AirPermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  []
  [AirPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []

  [CoreEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185e-6
  []
  [CorePermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  []
  [CorePermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []

  [CurrentCoef]
    type = MFEMFunctionCoefficient
    function = current_magnitude
  []
[]

[Sources]
  [SourcePotential]
    type = MFEMOpenCoilSource
    total_current_coef = CurrentCoef
    source_current_density_gridfunction = source_current_density
    source_electric_field_gridfunction = source_electric_field
    source_potential_gridfunction = source_electric_potential
    coil_in_boundary = 1
    coil_out_boundary = 2
    block = 1
  []
[]

[Executioner]
  type = Transient
  dt = 0.5
  start_time = 0.0
  end_time = 1.0

  l_tol = 1e-16
  l_max_its = 1000
[]

[Outputs]
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/ParaViewCoil
  []
[]
