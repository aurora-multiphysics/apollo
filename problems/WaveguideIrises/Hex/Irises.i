[Mesh]
  [gen]
    type = FileMeshGenerator
    file = Hex_Six_Inductive_Irises_MW.e
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
  [./Metal]
    type = ComplexConductor
    real_conductivity = 0.0 #rel
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
  [./E_mag]
    family = MONOMIAL
    order = FIRST  
  [../]
[]

[AuxKernels]
  [./calc_E_mag]
    type = VectorVariableComplexMagnitudeAux
    variable = E_mag # the auxvariable to compute
    real_vector = E_real # vector variable to compute from
    imag_vector = E_imag # vector variable to compute from
    execute_on = 'initial timestep_end'
  [../]
[]
[Kernels]
  # [./Real]
  #   type = VectorFEWave
  #   variable = E_real
  # [../]
  # [./Imag]
  #   type = VectorFEWave
  #   variable = E_imag
  # [../]
  # [./Real]
  #   type = VectorTimeDerivative
  #   variable = E_real
  # [../]
  # [./Imag]
  #   type = VectorFEWave
  #   variable = E_imag
  # [../]
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
  # [./wallReal]
  #   type = VectorPenaltyDirichletBC
  #   boundary = '1'
  #   variable = E_real
  #   penalty = 1e7
  #   # The third entry is to satisfy RealVectorValue
  #   values = '0 0 0'
  # [../]
  # [./wallImag]
  #   type = VectorPenaltyDirichletBC
  #   boundary = '1'
  #   variable = E_imag
  #   penalty = 1e7
  #   # The third entry is to satisfy RealVectorValue
  #   values = '0 0 0'
  # [../]

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
# [AuxVariables]
#   [E] 
#     family = NEDELEC_ONE
#     order = FIRST
#   []
# []

# [AuxKernels]
#   [E_field]
#     type = ComplexMagnitude
#     vector_re = E_real
#     vector_im = E_imag
#     variable = E
#   [../]
# []

[Preconditioning]
  [./pre]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Steady
  automatic_scaling = false
  # petsc_options_iname = '-pc_type -pc_hypre_type'
  # petsc_options_value = 'hypre boomeramg'

  # solve_type = 'NEWTON'
  # nl_rel_tol = 1e-5
  # solve_type = JFNK  
  solve_type = LINEAR
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu mumps'
  #   petsc_options_value = 'lu mumps'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
[]

[Outputs]
  exodus = true
  csv = true
[]
