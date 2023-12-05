[Mesh]
  type = CoupledMFEMMesh
  file = gold/simple-cube-tet10.e
  dim = 3
[]

[Problem]
  solve = false
[]

[AuxVariables]
  [test_variable_on_subapp_side]
    family = LAGRANGE
    order = SECOND
  []
[]

[Executioner]
  type = Transient
  dt = 1.0
  start_time = 0.0
  end_time = 1.0
[]