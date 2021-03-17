[Mesh]
  [gen]
    type = FileMeshGenerator
    file = gold/mesh_irises_hex.e
  [../]
  [./scale]
    type = TransformGenerator
    input = gen
    transform = SCALE
    vector_value ='1e-3 1e-3 1e-3'
  []
  uniform_refine=1
[]

[UserObjects]
  [./Waveguide]
    type = WaveguideProperties
    port_length = 22.86e-3
    port_width = 10.16e-3
    frequency = 9.3e9
  [../]      
[]

[Materials]
  [./Vacuum]
    type = ComplexConductor
    block = '1'
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
[]

[AuxKernels]
  [E_field]
    type = ComplexMagnitude
    vector_re = E_real
    vector_im = E_imag
    variable = E
    execute_on = 'timestep_end'
  [../]
[]
[Kernels]
  [./Real]
    type = HarmonicMaxwellReal
    variable = E_real
    v = E_imag
    waveguide_properties = Waveguide
  [../]
  [./Imag]
    type = HarmonicMaxwellImag
    variable = E_imag
    v = E_real
    waveguide_properties = Waveguide
  [../]
[]

[BCs]
  [./wallReal]
    type = VectorCurlPenaltyDirichletBC
    boundary = '1'
    penalty = 1.75e30
    variable = E_real
    v = E_imag 
  [../]
  [./wallImag]
    type = VectorCurlPenaltyDirichletImagBC
    boundary = '1'
    penalty = 1.75e30
    variable = E_imag
    v = E_real
  [../]
  [./inputPortReal]
    type = RobinTE10RealBC
    boundary = '2'
    variable = E_real
    v = E_imag    
    waveguide_properties = Waveguide
    input_port = true
  [../]
  [./inputPortImag]
    type = RobinTE10ImagBC
    boundary = '2'
    waveguide_properties = Waveguide
    variable = E_imag
    v = E_real    
    input_port = true
  [../]
  [./outputPortReal]
    type = RobinTE10RealBC
    boundary = '3'
    waveguide_properties = Waveguide
    variable = E_real
    v = E_imag    
    input_port = false
  [../]
  [./outputPortImag]
    type = RobinTE10ImagBC
    boundary = '3'
    waveguide_properties = Waveguide
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
