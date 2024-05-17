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
    family = MONOMIAL_VEC
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
    variable = sent_vector
    function = set_vector
  []
[]

[MultiApps]
  [sub_app]
    type = TransientMultiApp
    positions = '0 0 0'
    input_files = 'push_monomial_vector_nearest_location_transfer_subapp.i'
    execute_on = timestep_begin
  []
[]

[VectorTransfers]
  [push]
    type = MultiAppGeneralFieldNearestLocationTransfer
    to_multi_app = sub_app
    source_variable = sent_vector
    variable = received_vector_sub_app
  []
[]

[Executioner]
  type = Transient
  dt = 1.0
  start_time = 0.0
  end_time = 1.0
[]