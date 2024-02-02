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
  type = AFormulation
  magnetic_vector_potential_name = magnetic_vector_potential
  magnetic_reluctivity_name = magnetic_reluctivity
  magnetic_permeability_name = magnetic_permeability
  electric_conductivity_name = electrical_conductivity
  magnetic_flux_density_name = magnetic_flux_density

  current_density_name = current_density
  electric_field_name = electric_field
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
  [source_electric_field]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [source_grad_potential]
    type = MFEMVariable
    fespace = H1FESpace
  []
  [current_density]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [electric_field]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
[]

[Functions]
  [potential_high]
    type = ParsedFunction
    expression = 1000*cos(2.0*pi*freq*t)
    symbol_names = 'freq'
    symbol_values = '0.01666667'
  []
  [potential_low]
    type = ParsedFunction
    expression = -1000*cos(2.0*pi*freq*t)
    symbol_names = 'freq'
    symbol_values = '0.01666667'
  []
[]

[BCs]
  [tangential_E_bdr]
    type = MFEMVectorDirichletBC
    variable = dmagnetic_vector_potential_dt
    boundary = '1 2 4'
    vector_coefficient = TangentialECoef
  []
  [high_terminal]
    type = MFEMScalarDirichletBC
    variable = source_electric_potential
    boundary = '1'
    coefficient = HighPotential
  []
  [low_terminal]
    type = MFEMScalarDirichletBC
    variable = source_electric_potential
    boundary = '2'
    coefficient = LowPotential
  []
[]

[Materials]
  [air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = '1 2'
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
    value = 62.83185
  []
  [CorePermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  []
  [CorePermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []

  [HighPotential]
    type = MFEMFunctionCoefficient
    function = potential_high
  []
  [LowPotential]
    type = MFEMFunctionCoefficient
    function = potential_low
  []
[]

[Sources]
  [SourcePotential]
    type = MFEMScalarPotentialSource
    source_grad_potential_gridfunction = source_grad_potential
    source_potential_gridfunction = source_electric_potential
    conductivity_coef = CoilEConductivity
    h1_fespace = H1FESpace
    hcurl_fespace = HCurlFESpace
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
