# This example reproduces the libmesh vector_fe example 3 results

[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 40
    ny = 40
    nz = 1
    xmin = -1
    ymin = -1
    zmin = -0.2
    zmax = 0.2
    elem_type = HEX27
  []
  [./bottom]
    input = gen
    type = SubdomainBoundingBoxGenerator
    location = inside
    bottom_left = '-0.4 -0.2 -0.2'
    top_right = '0.4 0.2 0.2'
    block_id = 1
  [../]
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
    curl_z = '0.0'
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
    resistivity = 1e8
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
[AuxVariables]
  [current_density_x]
    order = CONSTANT
    family = MONOMIAL
  []
  [current_density_y]
    order = CONSTANT
    family = MONOMIAL
  []
  [current_density_z]
    order = CONSTANT
    family = MONOMIAL
  []
  [current_density]
    order = CONSTANT
    family = MONOMIAL_VEC
  []
[]

[AuxKernels]
  [current_density]
    type = CurrentDensity
    magnetic_field = H
    variable = current_density
    execute_on = timestep_end
  []
  [current_density_x]
    type = VectorVariableComponentAux
    magnetic_field = H
    variable = current_density_x
    component = x
    execute_on = timestep_end
    vector_variable = current_density
  []
  [current_density_y]
    type = VectorVariableComponentAux
    magnetic_field = H
    variable = current_density_y
    component = y
    execute_on = timestep_end
    vector_variable = current_density
  []
  [current_density_z]
    type = VectorVariableComponentAux
    magnetic_field = H
    variable = current_density_z
    component = z
    execute_on = timestep_end
    vector_variable = current_density
  []
[]
[Postprocessors]
  [magnetization]
    type = MagnetizationIntegral
    magnetic_field = H
    component = y
  []
  [./H_applied]
  type = FunctionValuePostprocessor
  function = y_sln
  execute_on = timestep_end
[../]

[]

[Executioner]
  type = Transient
  dt = 0.1
  num_steps = 40
  start_time = 0.0
  solve_type = 'LINEAR'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
[]

[Outputs]
  exodus = true
  csv = true
  # output_material_properties = true  
[]
