[Mesh]
  file = mug.e
[]

[Problem]
  type = MFEMProblem
  problem_type = rod
  input_mesh = ./cylinder-hex-q2.gen
[]

[BCs]
  [./curl_bc]
    type = VectorTangentialPenaltyDirichletBC
    variable = H
    boundary = '1 2 3'
  [../]
  [./thermal_bc]
    type = VectorTangentialPenaltyDirichletBC
    variable = H
    boundary = '1 2'
  []
  [./poisson_bc]
    type = VectorTangentialPenaltyDirichletBC
    variable = H
    boundary = '1 2'
  []
[]

[Materials]
  [./copper]
    type = GenericConstantMaterial
    prop_names = 'sigma InvTconductivity Tcapacity InvTcapacity'
    prop_values = '62.83185 100.0 1.0 1.0'
    block = 1
  [../]
  [./air]
    type = GenericConstantMaterial
    prop_names = 'sigma InvTconductivity Tcapacity InvTcapacity'
    prop_values = '62.83185e-6 1e-4 1.0 1.0'
    block = 2
  [../]
[]

[Executioner]
  type = Steady
[]
