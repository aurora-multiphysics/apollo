[Mesh]
  type = ExclusiveMFEMMesh
  file = ./team7_small.g
[]

[Problem]
  type = MFEMProblem
  formulation = AForm
  order = 2
[]

[UserObjects]
  [./SourceCoil]
    type = MFEMDivFreeVolumetricSource
    function = RacetrackCoilCurrentFunction
    block = '3 4 5 6'
  [../]
[]

[Functions]
  [./RacetrackCoilCurrentFunction]
    type = RacetrackCoilCurrentDensity
    coil_axis_x = 194e-3 
    coil_axis_y = 100e-3
    coil_thickness = 50e-3
    coil_current_magnitude = 2742.0
    coil_xsection_area = 2.5e-3
    frequency = 200.0
  [../]
[]

[Materials]
  [./air]
    type = MFEMConductor
    electrical_conductivity = 1.0
    magnetic_permeability = 12.5663706144e-7
    block = '1 3 4 5 6'
  [../]
  [./plate]
    type = MFEMConductor
    electrical_conductivity = 3.526e7
    magnetic_permeability = 12.5663706144e-7
    block = 2
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.001
  start_time = 0.0
  end_time = 0.002
[]

[Outputs]
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/TEAM7VisIt
  []
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/TEAM7ParaView
    refinements = 1
    high_order_output = true
  []
[]
