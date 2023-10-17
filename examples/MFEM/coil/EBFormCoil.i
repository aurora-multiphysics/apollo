[Mesh]
  type = ExclusiveMFEMMesh
  file = ./coil.gen
  dim = 3
[]

[Problem]
  type = MFEMProblem
[]

[Formulation]
  type = EBFormulation
  e_field_name = electric_field
  b_field_name = magnetic_flux_density
  magnetic_reluctivity_name = magnetic_reluctivity
  magnetic_permeability_name = magnetic_permeability
  electric_conductivity_name = electrical_conductivity
[]

[AuxVariables]
  [./electric_field]
    type = MFEMVariable
    fespace_name = _HCurlFESpace
    fespace_type = ND
    order = first
  [../]
  [./magnetic_flux_density]
    type = MFEMVariable
    fespace_name = _HDivFESpace
    fespace_type = RT
    order = constant
  [../]
  [./electric_potential]
    type = MFEMVariable
    fespace_name = _H1FESpace
    fespace_type = H1
    order = first
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
  [./tangential_E_bc]
    type = MFEMVectorFunctionDirichletBC
    variable = electric_field
    function = tangential_E
    boundary = '1 2 3 4'
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

[Materials]
  [./coil]
    type = MFEMConductor
    electrical_conductivity_coeff = CoilEConductivity
    electric_permittivity_coeff = CoilPermittivity
    magnetic_permeability_coeff = CoilPermeability
    block = 1
  [../]  
  [./air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = 2
  [../]  
  [./core]
    type = MFEMConductor
    electrical_conductivity_coeff = CoreEConductivity
    electric_permittivity_coeff = CorePermittivity
    magnetic_permeability_coeff = CorePermeability
    block = 3
  [../]
[]

[Coefficients]
  [./CoilEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185
  [../]
  [./CoilPermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]
  [./CoilPermittivity]
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

  [./CoreEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185e-6
  [../]
  [./CorePermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]
  [./CorePermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  [../]

  [./OneCoef]
    type = MFEMConstantCoefficient
    value = 1.0e12
  [../]
[]

[Sources]
  [./SourcePotential]
    type = MFEMScalarPotentialSource
    potential = electric_potential
    conductivity = electrical_conductivity
    solver_max_its = 1000
    block = 1
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
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/ParaViewCoil
  []
[]
