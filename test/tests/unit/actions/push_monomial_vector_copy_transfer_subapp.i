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
  [received_vector]
    family = MONOMIAL_VEC
    order = CONSTANT
  []

  [expected_vector]
    family = MONOMIAL_VEC
    order = CONSTANT
  []

  [received_vector_x]
    family = MONOMIAL
    order = CONSTANT
  []

  [received_vector_y]
    family = MONOMIAL
    order = CONSTANT
  []

  [received_vector_z]
    family = MONOMIAL
    order = CONSTANT
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
    variable = expected_vector
    function = set_vector
  []
[]

[Postprocessors]
  [vector_l2_difference]
    type = ElementVectorL2Difference
    variable = received_vector
    other_variable = expected_vector
  []
[]

[Executioner]
  type = Transient
  dt = 1.0
  start_time = 0.0
  end_time = 1.0
[]

[Outputs]
  csv = true
[]