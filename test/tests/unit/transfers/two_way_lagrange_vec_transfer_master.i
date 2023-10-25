[Mesh]
  type = CoupledMFEMMesh
  file = gold/mug-hex27.e
  dim = 3
[]

[Variables]
  [./moose_diffused]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[AuxVariables]
  [./test_variable_moose]
    family = LAGRANGE_VEC
    order = FIRST
  [../]

  [./test_variable_mfem]
    family = LAGRANGE_VEC
    order = FIRST
  [../]
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = moose_diffused
  []
[]

[Functions]
  [function]
    type = ParsedVectorFunction
    expression_x = t*x
    expression_y = t*y
  []
[]

[AuxKernels]
  [./test_variable_moose]
    type = VectorFunctionAux
    variable = test_variable_moose
    function = function
    execute_on = 'TIMESTEP_BEGIN'
  [../]
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
  [./l2_difference]
    type = ElementL2Difference
    variable = test_variable_mfem
    other_variable = test_variable_moose
  [../]
[]

[MultiApps]
  [sub_app]
    type = TransientMultiApp
    positions = '0 0 0'
    input_files = 'two_way_lagrange_vec_transfer_mfem.i'
    execute_on = timestep_begin
  []
[]

[Transfers]
  [push_test_variable]
    type = MultiAppNearestNodeTransfer

    # Transfer to the sub-app from this app.
    to_multi_app = sub_app

    # The name of the variable in this app to transfer.
    source_variable = test_variable_moose

    # The name of the auxiliary variable in the sub-app.
    variable = test_variable
  []

  [pull_test_variable]
    type = MultiAppNearestNodeTransfer

    # Transfer from the sub-app to this app.
    from_multi_app = sub_app

    # The name of the variable in the sub-app.
    source_variable = test_variable

    # The name of the auxiliary variable in this app.
    variable = test_variable_mfem
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
  exodus = true
[]
