[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 4
    ny = 4
    nz = 1
    xmin = -1
    ymin = -1
    xmax = 1
    ymax = 1    
    zmin = -0.02
    zmax = 0.02
    elem_type = TET10
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
    type = H_CurlE
    variable = H
  [../]
  [./magnetic_field_time_derivative]
    type = VectorTimeDerivative
    variable = H
  [../]
[]

[BCs]
  [./bnd]
    type = VectorCurlPenaltyDirichletBC
    #boundary = 'top bottom rmax'   
    boundary = 'left right top bottom front back'         
    penalty = 1e10
    function_x = 'x_sln'
    function_y = 'y_sln'
    function_z = 'z_sln'    
    variable = H
  [../]  
[]

[Functions]
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
  [./H_exact_fcn]
    type = ParsedVectorFunction
    #value_z = 'if(sqrt(x^2 + y^2)<(1-(sin(0.1*pi*t))), 0, sin(0.1*pi*t) - (1-sqrt(x^2 + y^2)))'
    value_z = 'if(max(abs(x),abs(y))<(1-(sin(0.1*pi*t))), 
                  0, 
                  if(abs(x)>abs(y), 
                    sin(0.1*pi*t) - (1-abs(x)),
                    sin(0.1*pi*t) - (1-abs(y))) 
                  )'
  [../] 

[]

[Materials]
  [./block1]
    type = Superconductor
    block = 0
    nonlinearity_parameter = 200
    critical_current_density = 1
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
    order = CONSTANT
    family = MONOMIAL_VEC
  []
  [H_exact]
    family = NEDELEC_ONE
    order = FIRST
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
  [H_exact]
    type = VectorFunctionAux
    function = H_exact_fcn
    variable = H_exact
  [../]
  [current_density]
    type = CurrentDensity
    magnetic_field = H
    variable = current_density
    execute_on = timestep_end
  [../]
  [current_density_x]
    type = VectorVariableComponentAux
    variable = current_density_x
    component = x
    execute_on = timestep_end
    vector_variable = current_density
  [../]
  [current_density_y]
    type = VectorVariableComponentAux
    variable = current_density_y
    component = y
    execute_on = timestep_end
    vector_variable = current_density
  [../]
  [current_density_z]
    type = VectorVariableComponentAux
    variable = current_density_z
    component = z
    execute_on = timestep_end
    vector_variable = current_density
  [../]
  [magnetic_moment]
    type = MagneticMoment
    magnetic_field = H
    variable = magnetic_moment
    execute_on = timestep_end
  [../]  
  [magnetic_moment_z]
    type = VectorVariableComponentAux
    variable = magnetic_moment_z
    component = z
    execute_on = timestep_end
    vector_variable = magnetic_moment
  [../]
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
  [./H_applied]
    type = FunctionValuePostprocessor
    function = z_sln
    execute_on = timestep_end
  [../]
  [./magnetization_domain]
    type = BlockAveragePostprocessor
    block_average_userobject = block_magnetization
    block =  0
  [../]
  [./error]
    type = ElementNedelecL2Error
    variable = H
    function = H_exact_fcn   
    execute_on = timestep_end
  [../]
  [./h]
    type = AverageElementSize
  [../]
[]


[Executioner]
  type = Transient
  dt = 0.1
  num_steps = 50
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
