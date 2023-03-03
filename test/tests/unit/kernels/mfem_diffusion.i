[Mesh]
  type = CoupledMFEMMesh
  file = mug.e
  dim = 3
[]

[Problem]
  type = MFEMProblem
  formulation = Custom
  order = 2
[]

[AuxVariables]
  [./diffused]
    type = MFEMVariable
    fespace = H1
    order = SECOND
  [../]
  [./dummy_moose_nodal]
    family = LAGRANGE
    order = FIRST
  [../]  
[../]

[ICs]
  [./dummy_moose_nodal_ic]
    type = FunctionIC
    variable = dummy_moose_nodal
    function = 2-x*x
  [../]  
[]
[UserObjects]
  [./one]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]
[]

[Kernels]
  [diff]
    type = MFEMDiffusionKernel
    variable = diffused
    coefficient = one
  []
[]

[Executioner]
  type = Transient
  dt = 1.0
  start_time = 0.0
  end_time = 1.0

  l_tol = 1e-16
  l_max_its = 1000  
[]

[Outputs]
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/Diffusion
  []
[]
