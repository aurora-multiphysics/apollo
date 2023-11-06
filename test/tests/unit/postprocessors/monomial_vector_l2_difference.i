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
  [primary_monomial_vector]
    family = MONOMIAL_VEC
    order = FIRST
  []

  [secondary_monomial_vector]
    family = MONOMIAL_VEC
    order = FIRST
  []

  [tertiary_monomial_vector]
    family = MONOMIAL_VEC
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
    variable = primary_monomial_vector
    function = primary_function
  []

  [secondary_ic]
    type = VectorFunctionIC
    variable = secondary_monomial_vector
    function = secondary_function  
  []

  [tertiary_ic]
    type = VectorFunctionIC
    variable = tertiary_monomial_vector
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
    var = primary_monomial_vector
    other_var = secondary_monomial_vector
  []

  [primary_tertiary_difference]
    type = ElementVectorL2Difference
    var = primary_monomial_vector
    other_var = tertiary_monomial_vector
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
