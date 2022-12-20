[Mesh]
  type = ExclusiveMFEMMesh
  file = ./coil.gen
[]

[Problem]
  type = MFEMProblem
  formulation = Joule
  order = 1
  dt = 0.5
  end_time = 100.0
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
    boundary = '1 2 3 4'
  [../]
  [./thermal_flux]
    type = MFEMBoundaryCondition
    variable = thermal_flux
    boundary = '1 3'
  []
  [./electric_potential]
    type = MFEMFunctionDirichletBC
    variable = electric_potential
    boundary = '1 2 3'
    function = p_bc
  [../]
[]

[Materials]
  [./coil]
    type = MFEMConductor
    electrical_conductivity_coeff = CoilEConductivity
    electric_permittivity_coeff = CoilPermittivity
    magnetic_permeability_coeff = CoilPermeability
    block = 1
  [../]
  [./coil_th]
    type = MFEMThermalMaterial
    thermal_conductivity_coeff = CoilTConductivity
    heat_capacity_coeff = CoilHeatCapacity
    block = 1
  [../] 
  [./core]
    type = MFEMConductor
    electrical_conductivity_coeff = CoreEConductivity
    electric_permittivity_coeff = CorePermittivity
    magnetic_permeability_coeff = CorePermeability
    block = 2
  [../]
  [./core_th]
    type = MFEMThermalMaterial
    thermal_conductivity_coeff = CoreTConductivity
    heat_capacity_coeff = CoreHeatCapacity
    block = 2
  [../]  
  [./air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = 3
  [../]
  [./air_th]
    type = MFEMThermalMaterial
    thermal_conductivity_coeff = AirTConductivity
    heat_capacity_coeff = AirHeatCapacity
    block = 3
  [../]  
[]

[UserObjects]
  [./CoilEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185
  [../]
  [./CoilPermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]
  [./CoilPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  [../]
  [./CoilTConductivity]
    type = MFEMConstantCoefficient
    value = 0.01
  [../]
  [./CoilHeatCapacity]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]

  [./CoreEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185e-6
  [../]
  [./CorePermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]
  [./CorePermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  [../]
  [./CoreTConductivity]
    type = MFEMConstantCoefficient
    value = 1e4
  [../]
  [./CoreHeatCapacity]
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
    file_base = OutputData/JouleCoilVisIt
  []
[]
