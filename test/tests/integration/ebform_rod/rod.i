[Mesh]
  type = ExclusiveMFEMMesh
  file = ./cylinder-hex-q2.gen
[]

[Problem]
  type = MFEMProblem
  use_glvis = true
[]

[Formulation]
  type = EBFormulation
  e_field_name = electric_field
  b_field_name = magnetic_flux_density
  magnetic_permeability_name = magnetic_permeability
  electric_conductivity_name = electrical_conductivity
[]

[AuxVariables]
  [./electric_potential]
    type = MFEMVariable
    fespace_name = _H1FESpace
    fespace_type = H1
    order = SECOND
  [../]
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

[Sources]
  [./SourcePotential]
    type = MFEMScalarPotentialSource
    potential = electric_potential
    conductivity = electrical_conductivity
    block = '1 2'
  [../]
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

[Coefficients]
  [./CopperEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185
  [../]
  [./CopperPermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]
  [./CopperPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  [../]

  [./AirEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185e-6
  [../]
  [./AirPermeability]
    type = MFEMConstantCoefficient
    value = 1.0
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
  end_time = 2.5

  l_tol = 1e-16
  l_max_its = 1000
[]

[Outputs]
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/EBFormRodVisIt
  []
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/EBFormRodParaView
    refinements = 1
    high_order_output = true
  []
[]
