[Mesh]
  [gen]
    type = FileMeshGenerator
    file = team7.e
  [../]
  [./scale]
    type = TransformGenerator
    input = gen
    transform = SCALE
    vector_value ='1e-3 1e-3 1e-3'
    []
[]

[Modules]
  [AVFormulation]
    vector_family = LAGRANGE_VEC
    vector_order = FIRST

    scalar_family = LAGRANGE
    scalar_order = FIRST

    zero_flux_boundaries = 'top bottom left right front back'
    zero_flux_penalty = 1e30
  []
[]

[Materials]
  [./vacuum]
    type = Conductor
    electrical_conductivity = 1 # [S/m]
    block = 'vacuum coilFL coilFR coilBL coilBR'
  [../]
  [./aluminium]
    type = Conductor
    electrical_conductivity = 3.526e7 #[S/m]
    block = 'plate'
  [../]
[]

[Functions]
  [./coil_current_density]
    type = ParsedVectorFunction
    value_x = '(I0/S)*sin(2*pi*f*t)*if(abs(x-x0)<a,
                  -((y-y0)/abs(y-y0)),
                  if(abs(y-y0)<a,
                    0.0,
                    -(y-(y0+a*((y-y0)/abs(y-y0))))/hypot(x-(x0+a*((x-x0)/abs(x-x0))),y-(y0+a*((y-y0)/abs(y-y0)))))
                  )'
    value_y = '(I0/S)*sin(2*pi*f*t)*if(abs(y-y0)<a,
                  ((x-x0)/abs(x-x0)),
                  if(abs(x-x0)<a,
                    0.0,
                    (x-(x0+a*((x-x0)/abs(x-x0))))/hypot(x-(x0+a*((x-x0)/abs(x-x0))),y-(y0+a*((y-y0)/abs(y-y0)))))
                  )'
    value_z ='0.0'
    vars = 'x0 y0 a I0 S f'
    vals = '194e-3 100e-3 50e-3 2742 2.5e-3 50'
  [../]
  [./zv]
    type = ParsedVectorFunction
    value_x = '0.0'
    value_y = '0.0'
    value_z ='0.0'
  [../]
[]

[Kernels]
  [./body_force] # looks just like boundary form but solves slower
    type = VectorBodyForce
    variable = magnetic_vector_potential
    function = coil_current_density
    block = 'coilFL coilFR coilBL coilBR'
  [../]
[]

[AuxVariables]
  # Cannot use Lagrange, as components may be discontinuous
  # at material boundaries.
  [E]
    family = MONOMIAL_VEC
    order = FIRST
  []
  [J]
    order = FIRST
    family = MONOMIAL_VEC
  []
  [B]
    family = MONOMIAL_VEC
    order = FIRST
  []
[]

[AuxKernels]
  [J_source]
    type = VectorFunctionAux
    function = coil_current_density
    variable = J
  [../]
  [E_field]
    type = ElectricField
    magnetic_vector_potential = magnetic_vector_potential
    electric_scalar_potential = electric_scalar_potential
    variable = E
    execute_on = timestep_end
  []
  [B_field]
    type = CurrentDensity
    magnetic_field = magnetic_vector_potential
    variable = B
    execute_on = timestep_end
  [../]
[]

[Preconditioning]
  [./pre]
    type = SMP
    full = true
    # petsc_options = '-ksp_diagonal_scale -ksp_diagonal_scale_fix'
    # petsc_options_iname = '-pc_type -sub_pc_type -sub_pc_factor_shift_type -pc_asm_overlap'
    # petsc_options_value = ' asm      lu           NONZERO                   2'
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.001
  start_time = 0.0
  end_time = 0.04
  automatic_scaling = true
  scheme=BDF2
  # petsc_options_iname = '-pc_type -pc_hypre_type'
  # petsc_options_value = 'hypre boomeramg'

  solve_type = 'LINEAR'
  # nl_rel_tol = 1e-5
  # solve_type = JFNK
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu mumps'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
[]

[Outputs]
  exodus = true
  csv = true
[]
