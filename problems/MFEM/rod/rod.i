[Mesh]
  type = MFEMMesh
  file = ./cylinder-hex-q2.gen
[]

[Problem]
  type = MFEMProblem
  formulation = Joule
  order = 2
  dt = 0.5
  end_time = 100.0
[]

[Functions]
  [./p_bc]
    type = ParsedFunction
    value = (z/abs(z))*cos(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  [../]
[]

[BCs]
  [./electric_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = '1 2'
    function = p_bc
  [../]
  [./tangential_dEdt]
    type = VectorTangentialPenaltyDirichletBC
    variable = E
    boundary = '1 2 3'
  [../]
  [./thermal_flux]
    type = VectorTangentialPenaltyDirichletBC
    variable = T
    boundary = '1 2'
  []
[]

[Materials]
  [./copper]
    type = GenericConstantMaterial
    prop_names = 'electrical_conductivity thermal_conductivity heat_capacity'
    prop_values = '62.83185 0.01 1.0'
    block = 1
  [../]
  [./air]
    type = GenericConstantMaterial
    prop_names = 'electrical_conductivity thermal_conductivity heat_capacity'
    prop_values = '62.83185e-6 1e4 1.0'
    block = 2
  [../]
[]

[Executioner]
  type = Steady
[]
