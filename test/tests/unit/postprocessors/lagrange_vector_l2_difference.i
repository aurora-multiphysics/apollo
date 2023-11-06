[Mesh]
  type = CoupledMFEMMesh
  file = gold/mug.e
  dim = 3
[]

[Variables]
  [moose_diffused]
    family = LAGRANGE
    order = FIRST
  []
[]

[AuxVariables]
  [primary_lagrange_vector]
    family = LAGRANGE_VEC
    order = FIRST
  []

  [secondary_lagrange_vector]
    family = LAGRANGE_VEC
    order = FIRST
  []

  [tertiary_lagrange_vector]
    family = LAGRANGE_VEC
    order = FIRST
  []
[]

[Kernels]
  [diffusion]
    type = Diffusion
    variable = moose_diffused
  []
[]

[Functions]
  [primary_function]
    type = ParsedVectorFunction
    expression_x = '100*x'
    expression_y = '100*y'
    expression_z = '100*z'
  []

  [secondary_function]
    type = ParsedVectorFunction
    expression_x = '0'
    expression_y = '0'
    expression_z = '0'
  []

  [tertiary_function]
    type = ParsedVectorFunction
    expression_x = '100*x'
    expression_y = '100*y'
    expression_z = '100*z'
  []
[]

[ICs]
  [primary_ic]
    type = VectorFunctionIC
    variable = primary_lagrange_vector
    function = primary_function
  []

  [secondary_ic]
    type = VectorFunctionIC
    variable = secondary_lagrange_vector
    function = secondary_function  
  []

  [tertiary_ic]
    type = VectorFunctionIC
    variable = tertiary_lagrange_vector
    function = tertiary_function  
  []
[]

[BCs]
  [bottom]
    type = DirichletBC
    variable = moose_diffused
    boundary = 'bottom'
    value = 1
  []

  [top]
    type = DirichletBC
    variable = moose_diffused
    boundary = 'top'
    value = 0
  []
[]

[Postprocessors]
  [primary_secondary_difference]
    type = ElementVectorL2Difference
    var = primary_lagrange_vector
    other_var = secondary_lagrange_vector
  []

  [primary_tertiary_difference]
    type = ElementVectorL2Difference
    var = primary_lagrange_vector
    other_var = tertiary_lagrange_vector
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
  exodus = false
[]
