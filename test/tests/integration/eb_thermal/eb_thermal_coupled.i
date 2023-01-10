[Mesh]
  type = CoupledMFEMMesh
  file = ./gold/cylinder-hex-q2.e
  dim = 3
[]

[Problem]
  type = MFEMProblem
  formulation = EBForm
  order = 2
[]

[Functions]
  [./potential_high]
    type = ParsedFunction
    value = cos(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  [../]
  [./potential_low]
    type = ParsedFunction
    value = -cos(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  [../]
  [./tangential_E]
    type = ParsedVectorFunction
    value_x = 0.0
    value_y = 0.0
    value_z = 0.0
  [../]
[]

[BCs]
  [./tangential_E_bdr]
    type = MFEMVectorFunctionDirichletBC
    variable = electric_field
    boundary = '1 2 3'
    function = tangential_E
  [../]
  [./high_terminal]
    type = MFEMFunctionDirichletBC
    variable = electric_potential
    boundary = '1'
    function = potential_high
  [../]
  [./low_terminal]
    type = MFEMFunctionDirichletBC
    variable = electric_potential
    boundary = '2'
    function = potential_low
  [../]
[]

[AuxVariables]
  [temperature]
    family = LAGRANGE
    order = FIRST
  []
  [./joule_heating]
    family = MONOMIAL
    order = CONSTANT
    initial_condition = 0.0
  [../]  
[]

[AuxKernels]
  [joule_heating_aux]
    type = MFEMJouleHeatingAux
  []  
[]

[Materials]
  [./copper]
    type = MFEMConductor
    electrical_conductivity_coeff = CopperEConductivity
    electric_permittivity_coeff = CopperPermittivity
    magnetic_permeability_coeff = CopperPermeability
    block = 1
  [../]
  [./air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = 2
  [../]
[]

[UserObjects]
  [./CopperEConductivity]
    type = MFEMParsedCoefficient
    mfem_gridfunction_names = 'temperature'
    mfem_coefficient_names = ''
    constant_names        = 'sigma0  alpha0   T0'
    constant_expressions  = '5.96e7  4.29e-6  273'
    function = 'sigma0 / (1 + alpha0 * (temperature - T0))' #
  [../]
  [./CopperPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  [../]
  [./CopperPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  [../]

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
