[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 5
  ny = 5
  nz = 5
[]

[Problem]
  solve = false
[]

[AuxVariables]
  [sent_vector]
    family = LAGRANGE_VEC
    order = FIRST
  []

  [sent_vector_x]
    family = LAGRANGE
    order = FIRST
  []

  [sent_vector_y]
    family = LAGRANGE
    order = FIRST
  []

  [sent_vector_z]
    family = LAGRANGE
    order = FIRST
  []

  [dummy]
    family = LAGRANGE
    order = FIRST
  []
[]


[Functions]
  [set_vector]
    type = ParsedVectorFunction
    expression_x = '100*x*x'
    expression_y = '100*y*y'
    expression_z = '100*z*z'
  []
[]

[ICs]
  [set_vector]
    type = VectorFunctionIC
    variable = sent_vector
    function = set_vector
  []
[]

[Executioner]
  type = Transient
  dt = 1.0
  start_time = 0.0
  end_time = 1.0
[]
