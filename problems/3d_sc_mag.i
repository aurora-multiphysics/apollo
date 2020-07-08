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
    xmax = 1
    ymax = 1    
    zmin = -0.2
    zmax = 0.2
    elem_type = HEX27
  []
  [./bottom]
    input = gen
    type = SubdomainBoundingBoxGenerator
    location = inside
    bottom_left = '-0.5 -0.5 -0.2'
    top_right = '0.5 0.5 0.2'
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
    boundary = 'left right top bottom front back'
    penalty = 1e10
    function_x = 'x_sln'
    function_y = 'y_sln'
    function_z = 'z_sln'    
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
    value = '0'
  [../]
  [./z_sln]
    type = ParsedFunction
    value = 'sin(0.1*pi*t)'

  [../]  
[]

[Materials]
  [./block0]
    type = Conductor
    block = 0
    resistivity = 1e8
  [../]

  [./block1]
    type = Superconductor
    block = 1
    nonlinearity_parameter = 50
    critical_current_density = 2
    critical_electric_field = 0.1
    magnetic_field = H
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
    # family = NEDELEC_ONE
    # order = FIRST
    order = CONSTANT
    family = MONOMIAL_VEC
  []
  [magnetic_moment]
    order = CONSTANT
    family = MONOMIAL_VEC
  []
  [magnetic_moment_z]
    order = CONSTANT
    family = MONOMIAL
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
    variable = current_density_x
    component = x
    execute_on = timestep_end
    vector_variable = current_density
  []
  [current_density_y]
    type = VectorVariableComponentAux
    variable = current_density_y
    component = y
    execute_on = timestep_end
    vector_variable = current_density
  []
  [current_density_z]
    type = VectorVariableComponentAux
    variable = current_density_z
    component = z
    execute_on = timestep_end
    vector_variable = current_density
  []
  [magnetic_moment]
    type = MagneticMoment
    magnetic_field = H
    variable = magnetic_moment
    execute_on = timestep_end
  []  
  [magnetic_moment_z]
    type = VectorVariableComponentAux
    variable = magnetic_moment_z
    component = z
    execute_on = timestep_end
    vector_variable = magnetic_moment
  []
[]

[UserObjects]
  [./block_magnetization]
    type = BlockAverageValue
    variable = magnetic_moment_z
    execute_on = timestep_end
    outputs = none
  [../]
[]

[Postprocessors]
  # [./magnetization]
  #   type = MagnetizationIntegral
  #   magnetic_field = H
  #   component = y
  # [../]
  [./H_applied]
  type = FunctionValuePostprocessor
  function = z_sln
  execute_on = timestep_end
  [../]
  # [./magnetization_auxvar]
  #   type = ElementIntegralVariablePostprocessor
  #   variable = magnetic_moment_y
  #   block =1
  # [../]
  [./magnetization_domain]
    type = BlockAveragePostprocessor
    block_average_userobject = block_magnetization
    block = 1
  [../]

[]


[Executioner]
  type = Transient
  dt = 0.1
  num_steps = 400
  start_time = 0.0
  solve_type = 'NEWTON'
  automatic_scaling = true
  #solve_type = PJFNK
  # petsc_options_iname = '-pc_type -pc_hypre_type'
  # petsc_options_value = 'hypre boomeramg'

  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  # [TimeStepper]
  #   type = IterationAdaptiveDT
  #   optimal_iterations = 1
  #   linear_iteration_ratio = 1
  #   dt = 0.1
  # []  
[]

[Outputs]
  exodus = true
  csv = true
[]
