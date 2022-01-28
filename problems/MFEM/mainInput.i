[Mesh]
  file = mug.e
[]

[Problem]
  type = MFEMProblem
  problem_type = rod
  input_mesh = ./cylinder-hex-q2.gen
[]

[Executioner]
  type = Steady
[]
