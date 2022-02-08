[Mesh]
  file = mug.e
[]

[Problem]
  type = MFEMProblem
  problem_type = rod
  input_mesh = ./cylinder-hex-q2.gen
[]

[BCs]
  [./bnd]
    type = VectorTangentialPenaltyDirichletBC
    boundary = 'left right top bottom front back'
    variable = H
  [../]
[]

[Executioner]
  type = Steady
[]
