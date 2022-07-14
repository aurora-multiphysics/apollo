[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 10
  nz=1
  xmax = 5
  ymax = 5
  zmax = 1
[]

[Materials]
  [./k]
    type = GenericConstantMaterial
    prop_names = 'thermal_conductivity'
    prop_values = '397.48' #copper in W/(m K)
    block = 0
  [../]
  [./cp]
    type = GenericConstantMaterial
    prop_names = 'specific_heat'
    prop_values = '385.0' #copper in J/(kg K)
    block = 0
  [../]
  [./rho]
    type = GenericConstantMaterial
    prop_names = 'density'
    prop_values = '8920.0' #copper in kg/(m^3)
    block = 0
  [../]
  [./sigma] #copper is default material
    type = ElectricalConductivity
    temperature = T
  [../]
  [./nu]
    type = Dielectric
    block = 0
  [../]
[]

[Variables]
  [./T]
    initial_condition = 293.0 #in K
  [../]
  [./A]
    family = LAGRANGE_VEC
    order = FIRST
  [../]
  [./V]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[Functions]
  [./V_low]
    type = ParsedFunction
    value = '0'
  [../]
  [./V_high]
    type = ParsedFunction
    value = '1'
  [../]

[]

[Kernels]
  [./Faraday]
    type = EddyAVFaraday
    variable = A
    scalar_potential = V
  [../]
  [./Gauge]
    type = EddyAVGauss
    variable = V
    vector_potential = A
  [../]
  [./HeatDiff]
    type = HeatConduction
    variable = T
  [../]
  [./HeatTdot]
    type = HeatConductionTimeDerivative
    variable = T
  [../]
  [./HeatSrc]
    type = JouleHeatingSource
    variable = T
    elec = V
  [../]
[]


[BCs]
  [./lefttemp]
    type = DirichletBC
    boundary = left
    variable = T
    value = 293 #in K
  [../]
  [./bnd1]
    type = FunctionDirichletBC
    variable = V
    boundary = 'left'
    function = 'V_high'
  [../]
  [./bnd2]
    type = FunctionDirichletBC
    variable = V
    boundary = 'right'
    function = 'V_low'
  [../]
  [./bnd3]
    type = VectorTangentialPenaltyDirichletBC
    boundary = 'left right top bottom front back'
    penalty = 1e20
    function_z = '0'
    #function_z = 'z_sln'
    variable = A
  [../]
[]

# [AuxVariables]
#   [E]
#     # Cannot use Lagrange, as components may be discontinuous
#     # at material boundaries.
#     family = MONOMIAL_VEC
#     order = FIRST
#   []
#   [V_applied]
#     family = MONOMIAL
#     order = FIRST
#   []
#   [B_x]
#     order = CONSTANT
#     family = MONOMIAL
#   []
#   [B_y]
#     order = CONSTANT
#     family = MONOMIAL
#   []
#   [B_z]
#     order = CONSTANT
#     family = MONOMIAL
#   []
#   [B]
#     order = CONSTANT
#     family = MONOMIAL_VEC
#   []
# []

# [AuxKernels]
#   [E_field]
#     type = MagneticField
#     electric_vector_potential = A
#     # magnetic_scalar_potential = V
#     variable = E
#     execute_on = timestep_end
#   []
#   [B_field]
#     type = CurrentDensity
#     magnetic_field = A
#     variable = B
#     execute_on = timestep_end
#   [../]
# []

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  scheme = bdf2
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -ksp_grmres_restart -sub_ksp_type -sub_pc_type -pc_asm_overlap'
  petsc_options_value = 'asm         101   preonly   ilu      1'
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-10
  l_tol = 1e-4
  dt = 1
  end_time = 5
  automatic_scaling = true
[]

[Outputs]
  exodus = true
  perf_graph = true
[]
