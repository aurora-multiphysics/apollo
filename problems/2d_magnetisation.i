# This example reproduces the libmesh vector_fe example 3 results

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 40
  ny = 40
  xmin = -1
  ymin = -1
  elem_type = QUAD9
[]

[MeshModifiers]
  [bottom]
    type = SubdomainBoundingBox
    location = inside
    bottom_left = '-0.4 -0.2 0'
    top_right = '0.4 0.2 0'
    block_id = 1
  []
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
#   [./bnd]
#     type = VectorCurlBC
#     boundary = 'left right top bottom'    
#     curl_value = field
#     variable = H
#   [../]
  [./bnd]
    type = VectorCurlPenaltyDirichletBC
    boundary = 'left right top bottom'
    penalty = 1e10
    function_x = 'x_sln'
    function_y = 'y_sln'
    variable = H
  [../]  
[]

[Functions]
  [./field]
    type = ParsedVectorFunction
    curl_z = '0.1'
  [../]
  [./x_sln]
    type = ParsedFunction
    value = '0'
  [../]
  [./y_sln]
    type = ParsedFunction
    value = 'sin(pi*t)'
  [../]  
[]

[Materials]
  [./block0]
    type = Conductor
    block = 0
    resistivity = 1e7
  [../]

  [./block1]
    type = Conductor
    block = 1
    resistivity = 0.1
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
  num_steps = 100
  start_time = 0.0
  solve_type = 'LINEAR'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
[]

[Outputs]
  exodus = true
  output_material_properties = true  
[]
