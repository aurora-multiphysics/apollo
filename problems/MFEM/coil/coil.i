[Mesh]
  type = MFEMMesh
  file = ./coil.gen
[]

[Problem]
  type = MFEMProblem
  formulation = Joule
  order = 1
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
    boundary = '1 2 3'
    function = p_bc
  [../]
  [./tangential_dEdt]
    type = VectorTangentialPenaltyDirichletBC
    variable = E
    boundary = '1 2 3 4'
  [../]
  [./thermal_flux]
    type = DirichletBC
    variable = F
    boundary = '1 3'
  []
[]

[Materials]
  [./coil]
    type = GenericConstantMaterial
    prop_names = 'electrical_conductivity thermal_conductivity heat_capacity'
    prop_values = '62.83185 0.01 1.0'
    block = 1
  [../]
  [./core]
    type = GenericConstantMaterial
    prop_names = 'electrical_conductivity thermal_conductivity heat_capacity'
    prop_values = '62.83185e-6 1e4 1.0'
    block = 2
  [../]
  [./air]
    type = GenericConstantMaterial
    prop_names = 'electrical_conductivity thermal_conductivity heat_capacity'
    prop_values = '62.83185e-6 1e4 1.0'
    block = 3
  [../]
[]

[Executioner]
  type = Steady
[]
