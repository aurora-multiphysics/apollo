[Mesh]
  type = CoupledMFEMMesh
  file = gold/mug-hex27.e
  dim = 3
[]

[Variables]
  [moose_diffused]
    family = LAGRANGE
    order = FIRST
  []
[]

[AuxVariables]
  [moose_lagrange_vector_component_x]
    family = LAGRANGE 
    order = FIRST
  []

  [moose_lagrange_vector_component_y]
    family = LAGRANGE 
    order = FIRST
  []

  [moose_lagrange_vector_component_z]
    family = LAGRANGE 
    order = FIRST
  []

  [mfem_lagrange_vector_component_x]
    family = LAGRANGE 
    order = FIRST
  []

  [mfem_lagrange_vector_component_y]
    family = LAGRANGE 
    order = FIRST
  []

  [mfem_lagrange_vector_component_z]
    family = LAGRANGE 
    order = FIRST
  []
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = moose_diffused
  []
[]

[Functions]
  [function_x]
    type = ParsedFunction
    expression = '100*x'
  []

  [function_y]
    type = ParsedFunction
    expression = '100*y'
  []

  [function_z]
    type = ParsedFunction
    expression = '100*z'
  []
[]

[ICs]
  [set_moose_x]
    type = FunctionIC
    variable = moose_lagrange_vector_component_x
    function = function_x
  []

  [set_moose_y]
    type = FunctionIC
    variable = moose_lagrange_vector_component_y
    function = function_y
  []

  [set_moose_z]
    type = FunctionIC
    variable = moose_lagrange_vector_component_z
    function = function_z
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
  [max_x_difference]
    type = ElementL2Difference
    variable = mfem_lagrange_vector_component_x
    other_variable = moose_lagrange_vector_component_x
  []

  [max_y_difference]
    type = ElementL2Difference
    variable = mfem_lagrange_vector_component_y
    other_variable = moose_lagrange_vector_component_y
  []

  [max_z_difference]
    type = ElementL2Difference
    variable = mfem_lagrange_vector_component_z
    other_variable = moose_lagrange_vector_component_z
  []
[]

[MultiApps]
  [sub_app]
    type = TransientMultiApp
    positions = '0 0 0'
    input_files = 'two_way_lagrange_vector_transfer_mfem.i'
    execute_on = timestep_begin
  []
[]

[Transfers]
  [push]
    type = MultiAppGeneralFieldNearestLocationTransfer
    to_multi_app = sub_app
    source_type = 'variable_default variable_default variable_default'
    source_variable = 'moose_lagrange_vector_component_x moose_lagrange_vector_component_y moose_lagrange_vector_component_z'
    variable = 'mfem_lagrange_vector_component_x_in mfem_lagrange_vector_component_y_in mfem_lagrange_vector_component_z_in'
  []

  [pull]
    type = MultiAppGeneralFieldNearestLocationTransfer
    from_multi_app = sub_app
    source_type = 'variable_default variable_default variable_default'
    source_variable = 'mfem_lagrange_vector_component_x_out mfem_lagrange_vector_component_y_out mfem_lagrange_vector_component_z_out'
    variable = 'mfem_lagrange_vector_component_x mfem_lagrange_vector_component_y mfem_lagrange_vector_component_z'
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
