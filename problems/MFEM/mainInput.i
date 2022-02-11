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

[Executioner]
  type = Steady
[]
