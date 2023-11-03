[Mesh]
  type = CoupledMFEMMesh
  file = ./gold/cylinder-hex-q2.e
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
[]

[FESpaces]
  [H1FESpace]
    type = MFEMFESpace
    fespace_type = H1
    order = SECOND
  []
  [HCurlFESpace]
    type = MFEMFESpace
    fespace_type = ND
    order = SECOND
  []
  [HDivFESpace]
    type = MFEMFESpace
    fespace_type = RT
    order = FIRST
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
  [electric_potential]
    type = MFEMVariable
    fespace = H1FESpace
  []
[]

[Functions]
  [potential_high]
    type = ParsedFunction
    value = cos(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  []
  [potential_low]
    type = ParsedFunction
    value = -cos(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  []
  [tangential_E]
    type = ParsedVectorFunction
    value_x = 0.0
    value_y = 0.0
    value_z = 0.0
  []
[]

[BCs]
  [tangential_E_bdr]
    type = MFEMVectorFunctionDirichletBC
    variable = electric_field
    boundary = '1 2 3'
    function = tangential_E
  []
  [high_terminal]
    type = MFEMScalarDirichletBC
    variable = electric_potential
    boundary = '1'
    coefficient = HighPotential
  []
  [low_terminal]
    type = MFEMScalarDirichletBC
    variable = electric_potential
    boundary = '2'
    coefficient = LowPotential
  []
[]

[Sources]
  [SourcePotential]
    type = MFEMScalarPotentialSource
    potential = electric_potential
    conductivity = electrical_conductivity
    h1_fespace = H1FESpace
    hcurl_fespace = HCurlFESpace
  []
[]

[AuxVariables]
  [temperature]
    family = LAGRANGE
    order = FIRST
  []
  [joule_heating]
    family = MONOMIAL
    order = CONSTANT
    initial_condition = 0.0
  []
[]

[AuxKernels]
  [joule_heating_aux]
    type = MFEMJouleHeatingAux
  []
[]

[Materials]
  [copper]
    type = MFEMConductor
    electrical_conductivity_coeff = CopperEConductivity
    electric_permittivity_coeff = CopperPermittivity
    magnetic_permeability_coeff = CopperPermeability
    block = 1
  []
  [air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = 2
  []
[]

[Coefficients]
  [CopperEConductivity]
    type = MFEMParsedCoefficient
    mfem_gridfunction_names = 'temperature'
    mfem_coefficient_names = ''
    constant_names = 'sigma0  alpha0   T0'
    constant_expressions = '5.96e7  4.29e-6  273'
    function = 'sigma0 / (1 + alpha0 * (temperature - T0))' #
  []
  [CopperPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  []
  [CopperPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []
  [AirEConductivity]
    type = MFEMConstantCoefficient
    value = 1.0
  []
  [AirPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  []
  [AirPermittivity]
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

[Executioner]
  type = Transient
  dt = 0.5
  start_time = 0.0
  end_time = 1.0

  l_tol = 1e-16
  l_max_its = 1000
[]

[Outputs]
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/NodalCoupledMFEMVarTest
  []
[]
