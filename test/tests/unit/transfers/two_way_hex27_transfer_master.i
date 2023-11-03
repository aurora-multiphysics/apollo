[Mesh]
  type = CoupledMFEMMesh
  file = gold/simple-cube-hex27.e
  dim = 3
[]

[Variables]
  [moose_diffused]
    family = LAGRANGE
    order = SECOND
  []
[]

[AuxVariables]
  [test_variable_sent]
    family = LAGRANGE
    order = SECOND
  []

  [test_variable_received]
    family = LAGRANGE
    order = SECOND
  []
[]

[Kernels]
  [diffusion]
    type = Diffusion
    variable = moose_diffused
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

[ICs]
  [set_test_variable_sent]
    type = FunctionIC
    variable = test_variable_sent
    function = 42.0+1000.0*x*x
  []

  [zero_test_variable_received]
    type = ConstantIC
    variable = test_variable_received
    value = 0.0
  []
[]

[Postprocessors]
  [l2_difference]
    type = ElementL2Difference
    variable = test_variable_sent
    other_variable = test_variable_received
  []
[]

[MultiApps]
  [sub_app]
    type = TransientMultiApp
    positions = '0 0 0'
    input_files = 'two_way_hex27_transfer_mfem.i'
    execute_on = timestep_begin
  []
[]

[Transfers]
  [push_test_variable]
    type = MultiAppNearestNodeTransfer

    # Transfer to the sub-app from this app.
    to_multi_app = sub_app

    # The name of the variable in this app to transfer.
    source_variable = test_variable_sent

    # The name of the auxiliary variable in the sub-app.
    variable = test_variable_on_mfem_side
  []

  [pull_test_variable]
    type = MultiAppNearestNodeTransfer

    # Transfer from the sub-app to this app.
    from_multi_app = sub_app

    # The name of the variable in the sub-app.
    source_variable = test_variable_on_mfem_side

    # The name of the auxiliary variable in this app.
    variable = test_variable_received
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
