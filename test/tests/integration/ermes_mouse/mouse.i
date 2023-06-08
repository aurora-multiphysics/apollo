[Mesh]
  type = ExclusiveMFEMMesh
  file = ./ermes_mouse_coarse.g
[]

[Problem]
  type = MFEMProblem
  formulation = ComplexEForm
  use_glvis = true
[]

[Functions]
  [./tangential_E]
    type = ParsedVectorFunction
    value_x = 0.0
    value_y = 0.0
    value_z = 0.0
  [../]
[]

[BCs]
  [./tangential_E_bdr]
    type = MFEMComplexVectorFunctionDirichletBC
    variable = electric_field
    boundary = '2 3 4'
    real_function = tangential_E
    imag_function = tangential_E
  [../]
  [./waveguide_port_in]
    type = MFEMRWTE10PortRBC
    variable = electric_field
    boundary = '5'
    port_length_vector = '24.76e-2 0 0'
    port_width_vector = '0 12.38e-2 0'    
    frequency = 900e6
    input_port = true
  [../]
  [./waveguide_port_out]
    type = MFEMRWTE10PortRBC
    variable = electric_field
    boundary = '6'
    port_length_vector = '24.76e-2 0 0'
    port_width_vector = '0 12.38e-2 0'    
    frequency = 900e6
    input_port = false
  [../]
[]

[Materials]
  [./mouse]
    type = MFEMConductor
    electrical_conductivity_coeff = MouseEConductivity
    electric_permittivity_coeff = MousePermittivity
    magnetic_permeability_coeff = MousePermeability
    block = 1
  [../]
  [./air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = 2
  [../]  
[]

[UserObjects]
  [./frequency]
    type = MFEMConstantCoefficient
    value = 900e6
  [../]  
  [./MouseEConductivity]
    type = MFEMConstantCoefficient
    value = 0.97
  [../]
  [./MousePermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  [../]
  [./MousePermittivity]
    type = MFEMConstantCoefficient
    value = 3.807300762e-10
  [../]
  [./AirEConductivity]
    type = MFEMConstantCoefficient
    value = 0.0
  [../]
  [./AirPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  [../]
  [./AirPermittivity]
    type = MFEMConstantCoefficient
    value = 8.8541878176e-12
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/ERMESMouseParaView
    refinements = 1
    high_order_output = true
  []
[]
