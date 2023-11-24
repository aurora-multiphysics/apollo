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
    family = LAGRANGE_VEC
    order = FIRST
  []

  [expected_vector]
    family = LAGRANGE_VEC
    order = FIRST
  []

  [received_vector_x]
    family = LAGRANGE
    order = FIRST
  []

  [received_vector_y]
    family = LAGRANGE
    order = FIRST
  []

  [received_vector_z]
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
    variable = expected_vector
    function = set_vector
  []
[]

[MultiApps]
  [sub_app]
    type = TransientMultiApp
    positions = '0 0 0'
    input_files = 'pull_lagrange_vector_nearest_location_transfer_subapp.i'
    execute_on = timestep_begin
  []
[]

[VectorTransfers]
  [pull]
    type = MultiAppGeneralFieldNearestLocationTransfer
    from_multi_app = sub_app
    source_type = 'variable_default variable_default variable_default'
    source_variable = sent_vector
    variable = received_vector
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