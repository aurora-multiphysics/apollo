[Mesh]
  type = ExclusiveMFEMMesh
  file = ./cylinder-hex-q2.gen
[]

[Problem]
  type = MFEMProblem
  formulation = Joule
  order = 2
  dt = 0.5
  end_time = 2.5
[]

[Functions]
  [./p_bc]
    type = ParsedFunction
    value = (z/abs(z+1e-50))*cos(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  [../]
[]

[BCs]
  [./tangential_dEdt]
    type = MFEMBoundaryCondition
    variable = tangential_dEdt
    boundary = '1 2 3'
  [../]
  [./thermal_flux]
    type = MFEMBoundaryCondition
    variable = thermal_flux
    boundary = '1 2'
  []
  [./electric_potential]
    type = MFEMFunctionDirichletBC
    variable = electric_potential
    boundary = '1 2'
    function = p_bc
  [../]
[]

[Materials]
  [./copper]
    type = MFEMConductor
    electrical_conductivity_coeff = CopperEConductivity
    electric_permittivity_coeff = CopperPermittivity
    magnetic_permeability_coeff = CopperPermeability
    block = 1
  [../]
  [./copper_th]
    type = MFEMThermalMaterial
    thermal_conductivity_coeff = CopperTConductivity
    heat_capacity_coeff = CopperHeatCapacity
    block = 1
  [../]  
  [./air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = 2
  [../]
  [./air_th]
    type = MFEMThermalMaterial
    thermal_conductivity_coeff = AirTConductivity
    heat_capacity_coeff = AirHeatCapacity
    block = 2
  [../]

[]

[UserObjects]
  [./CopperEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185
  [../]
  [./CopperPermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]
  [./CopperPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  [../]
  [./CopperTConductivity]
    type = MFEMConstantCoefficient
    value = 0.01
  [../]
  [./CopperHeatCapacity]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]

  [./AirEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185e-6
  [../]
  [./AirPermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]
  [./AirPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  [../]
  [./AirTConductivity]
    type = MFEMConstantCoefficient
    value = 1e4
  [../]
  [./AirHeatCapacity]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/JouleRodVisIt
  []
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/JouleRodParaView
    refinements = 1
    high_order_output = true
  []
[]
