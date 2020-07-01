# This example reproduces the libmesh vector_fe example 3 results

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
  xmin = -1
  ymin = -1
  elem_type = QUAD9
[]

[Variables]
  [./H]
    family = NEDELEC_ONE
    order = FIRST
  [../]
[]

[Kernels]
  [./curl_curl_H]
    type = CurlE
    variable = H
  [../]
  [./magnetic_field_time_derivative]
  type = VectorTimeDerivative
  variable = H
  [../]
[]

[BCs]
  [./bnd]
    type = VectorCurlBC
    boundary = 'left right top bottom'    
    curl_value = field
    variable = H
  [../]  
[]

[Functions]
  [./field]
    type = ParsedVectorFunction
    curl_z = '0.1'
  [../]
[]

[Preconditioning]
  [./pre]
    type = SMP
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.1
  num_steps = 10
  start_time = 0.0
  solve_type = 'LINEAR'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
[]

[Outputs]
  exodus = true
[]
