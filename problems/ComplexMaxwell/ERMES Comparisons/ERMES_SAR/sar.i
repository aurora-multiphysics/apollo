[Mesh]
  [gen]
    type = FileMeshGenerator
    file = gold/mesh_sar_tet.e
  [../]
[]

[UserObjects]
  [./WaveguideProps]
    type = WaveguideProperties
    port_length_vector = '24.76e-2 0 0'
    port_width_vector = '0 12.38e-2 0'
    frequency = 900e6
  [../]
[]

[Materials]
  [./Mouse]
    type = ComplexConductor
    real_conductivity = 0.97
    real_rel_permittivity = 43
    block = 'mouse'
  [../]
  [./Waveguide]
    type = ComplexConductor
    block = 'waveguide'
  [../]
[]

[Variables]
  [./E_real]
    family = NEDELEC_ONE
    order = FIRST
  [../]
  [./E_imag]
    family = NEDELEC_ONE
    order = FIRST
  [../]
[]
[AuxVariables]
  [./E]
    family = NEDELEC_ONE
    order = FIRST
  [../]
  [./q]
    family = MONOMIAL
    order = FIRST
  [../]
[]

[AuxKernels]
  [E_field]
    type = ComplexMagnitude
    vector_re = E_real
    vector_im = E_imag
    variable = E
    execute_on = 'timestep_end'
  [../]
  [heat_source]
    type = JouleHeating
    electric_field = E
    variable = q
    execute_on = 'timestep_end'
  [../]
[]
[Kernels]
  [./Real]
    type = ComplexMaxwellReal
    variable = E_real
    v = E_imag
    waveguide_properties = WaveguideProps
  [../]
  [./Imag]
    type = ComplexMaxwellImag
    variable = E_imag
    v = E_real
    waveguide_properties = WaveguideProps
  [../]
[]


[BCs]

  [./wallReal]
    type = VectorTangentialPenaltyDirichletRealBC
    boundary = 'left top bottom'
    penalty = 1.75e30
    variable = E_real
    v = E_imag
  [../]
  [./wallImag]
    type = VectorTangentialPenaltyDirichletImagBC
    boundary = 'left top bottom'
    penalty = 1.75e30
    variable = E_imag
    v = E_real
  [../]
  [./inputPortReal]
    type = RobinTE10RealBC
    boundary = 'back'
    variable = E_real
    v = E_imag
    waveguide_properties = WaveguideProps
    input_port = true
  [../]
  [./inputPortImag]
    type = RobinTE10ImagBC
    boundary = 'back'
    waveguide_properties = WaveguideProps
    variable = E_imag
    v = E_real
    input_port = true
  [../]
  [./outputPortReal]
    type = RobinTE10RealBC
    boundary = 'front'
    waveguide_properties = WaveguideProps
    variable = E_real
    v = E_imag
    input_port = false
  [../]
  [./outputPortImag]
    type = RobinTE10ImagBC
    boundary = 'front'
    waveguide_properties = WaveguideProps
    variable = E_imag
    v = E_real
    input_port = false
  [../]
[]

[Preconditioning]
  [pre]
    type = SMP
    full = true
    petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
    petsc_options_value = 'lu mumps'
    # petsc_options_iname = '-ksp_type -pc_type -sub_pc_type -snes_max_it -sub_pc_factor_shift_type -pc_asm_overlap -snes_atol -snes_rtol '
    # petsc_options_value = 'gmres asm lu 100 NONZERO 2 1E-14 1E-12'
  []
[]

[Executioner]
  type = Steady
  automatic_scaling = false
  solve_type = LINEAR

  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
[]

[Outputs]
  exodus = true
  csv = true
[]
