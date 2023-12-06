#
# NB: Currently MFEMProblem requires a dummy auxvariable and kernel.
#

[Mesh]
  type = CoupledMFEMMesh
  file = gold/simple-cube-pyramid5.e
  dim = 3
[]

[Problem]
  type = MFEMProblem
[]

[Formulation]
  type = CustomFormulation
[]

[AuxVariables]
  [dummy]
    family = LAGRANGE
    order = FIRST
  []

  [sent_variable]
    family = LAGRANGE
    order = FIRST
  []
[]

[ICs]
  [set_variable]
    type = FunctionIC
    variable = sent_variable
    function = set_variable
  []
[]  

[Kernels]
  [dummy_kernel]
    type = MFEMDiffusionKernel
    variable = dummy
    coefficient = one_coefficient
  []
[]

[Functions]
  [set_variable]
    type = ParsedFunction
    expression = '42 + 100*x*x'
  []

  [value_top]
    type = ParsedFunction
    expression = 0.0
  []

  [value_bottom]
    type = ParsedFunction
    expression = 1.0
  []
[]

[BCs]
  [top]
    type = MFEMScalarDirichletBC
    variable = dummy
    boundary = '1'
    coefficient = top_coefficient
  []
  
  [bottom]
    type = MFEMScalarDirichletBC
    variable = dummy
    boundary = '2'
    coefficient = bottom_coefficient
  []
[]

[Coefficients]
  [one_coefficient]
    type = MFEMConstantCoefficient
    value = 1.0
  []

  [top_coefficient]
    type = MFEMFunctionCoefficient
    function = value_top
  []

  [bottom_coefficient]
    type = MFEMFunctionCoefficient
    function = value_bottom
  []
[]

[Executioner]
  type = Transient
  dt = 1.0
  start_time = 0.0
  end_time = 1.0
[]