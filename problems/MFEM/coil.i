[Mesh]
  file = mug.e
[]

[Problem]
  type = MFEMProblem
  input_mesh = ./coil.gen
  order = 2
[]

[BCs]
  [./curl_bc]
    type = VectorTangentialPenaltyDirichletBC
    variable = H
    boundary = '1 2 3 4'
  [../]
  [./thermal_bc]
    type = VectorTangentialPenaltyDirichletBC
    variable = H
    boundary = '1 3'
  []
  [./poisson_bc]
    type = VectorTangentialPenaltyDirichletBC
    variable = H
    boundary = '1 2 3'
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
