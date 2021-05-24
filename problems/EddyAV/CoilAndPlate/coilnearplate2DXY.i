[Mesh]
  [gen]
    type = FileMeshGenerator
    file = gold/coil_plate_2d.e
  [../]
  [./scale]
    type = TransformGenerator
    input = gen
    transform = SCALE
    vector_value ='1e-2 1e-2 1e-2'
  []
  second_order = true
[]

[Variables]
  [./Az]
    family = LAGRANGE
    order = SECOND
  [../]
[]

[BCs]
  [./PEC]
    # Dirichlet conditions on Az
    type = DirichletBC
    variable = Az
    boundary = 'top_edge bottom_edge left_edge right_edge'
    value = 0
  [../]
[]

[Kernels]
  [./CurlCurlA] # looks just like boundary form but solves slower
    type = MatCoefDiffusion
    variable = Az
    conductivity = 'reluctance'
  [../]
  [./CoefTimeDerivative] # looks just like boundary form but solves slower
    type = MatTimeDerivative
    variable = Az
    material_property =  electrical_conductivity
  [../]

  [./body_force] # looks just like boundary form but solves slower
    type = BodyForce
    variable = Az
    function = coil_current_density
    block = 'coil'
  [../]
  # [./body_force] # looks just like boundary form but solves slower
  #   type = CoupledForce
  #   variable = Az
  #   v = J_source
  #   block = 'coil'
  # [../]
[]

[Materials]
  [./vacuum]
    type = Conductor
    electrical_conductivity = 1e-10 # [S/m]
    block = 'vacuum coil'
  [../]
  [./aluminium]
    type = Conductor
    electrical_conductivity = 1e6 #[S/m]
    block = 'plate'
  [../]
[]

[Functions]
  [./dts]
    type = PiecewiseConstant
    x = '10.0 10.02 10.04 10.1'
    y = '0.5 2.5e-4 5.0e-4 1e-3'
    direction = right
  [../]
  [./coil_current_density]
    type = ParsedFunction
    value ='J0*if(t<Deltat, t/Deltat,
                 if(t>t2, if(t<(t2+Deltat),1-(t-t2)/Deltat,0),
                    1))'
    vars = 'J0 Deltat t2'
    vals = '1e6 40e-3 10'
  [../]
[]

[AuxVariables]
  # Cannot use Lagrange, as components may be discontinuous
  # at material boundaries.
  # [E]
  #   family = LAGRANGE_VEC
  #   order = FIRST
  # []
  # [J]
  #   order = FIRST
  #   family = LAGRANGE
  # []
  [F]
    order = FIRST
    family = MONOMIAL_VEC
  []
  [J_source]
    # J defined on faces - RT elements preferable
    order = SECOND
    family = MONOMIAL
  []
  [J_eddy]
    # J defined on faces - RT elements preferable
    order = SECOND
    family = MONOMIAL
  []
  [B]
    # B defined on edges
    family = NEDELEC_ONE
    order = FIRST
  []
[]

[AuxKernels]
  [J_source]
    type = FunctionAux
    function = coil_current_density
    variable = J_source
  [../]
  [J_eddy]
   type = A2DCurrentDensity
    magnetic_vector_potential = Az
    variable = J_eddy
    execute_on = timestep_end
  [../]
  [Lorentz_Force_Density]
   type = A2DLorentzForce
    current_density = J_eddy
    magnetic_flux_density = B
    variable = F
    execute_on = timestep_end
  [../]
  # [E_field]
  #   type = ElectricField
  #   magnetic_vector_potential = magnetic_vector_potential
  #   electric_scalar_potential = electric_scalar_potential
  #   variable = E
  #   execute_on = timestep_end
  # []
  [B_field]
    type = A2DMagneticField
    magnetic_vector_potential = Az
    variable = B
    execute_on = timestep_end
  [../]
[]
[Preconditioning]
  [./pre]
    type = SMP
    full = true
    petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
    petsc_options_value = 'lu mumps'
    petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  [../]
[]

[Executioner]
  type = Transient
  automatic_scaling = true
  solve_type = 'NEWTON'
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-8
  num_steps = 200
  end_time = 10.1
  [./TimeStepper]
    type = FunctionDT
    function = dts
  [../]
[]

[Outputs]
  exodus = true
  csv = true

  # perf_graph = true
[]
