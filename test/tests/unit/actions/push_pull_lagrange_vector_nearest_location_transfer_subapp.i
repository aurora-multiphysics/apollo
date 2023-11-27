[Mesh]
  type  = GeneratedMesh
  dim   = 3
  nx    = 5
  ny    = 5
  nz    = 5
[]

[Problem]
  solve = false
[]

[AuxVariables]
  [received_vector_subapp]
    family  = LAGRANGE_VEC
    order   = FIRST
  []

  [received_vector_subapp_x]
    family  = LAGRANGE
    order   = FIRST
  []

  [received_vector_subapp_y]
    family  = LAGRANGE
    order   = FIRST
  []

  [received_vector_subapp_z]
    family  = LAGRANGE
    order   = FIRST
  []
[]

[Executioner]
  type        = Transient
  dt          = 1.0
  start_time  = 0.0
  end_time    = 1.0
[]

[Outputs]
  csv = false
[]