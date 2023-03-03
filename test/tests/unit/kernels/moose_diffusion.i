[Mesh]
  file = mug.e
[]

[Variables]
  [convected]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = convected
  []
[]

[BCs]
  [bottom]
    type = DirichletBC
    variable = convected
    boundary = 'bottom'
    value = 1
  []

  [top]
    type = DirichletBC
    variable = convected
    boundary = 'top'
    value = 0
  []
[]

[Executioner]
  type = Steady
  solve_type = 'PJFNK'
[]

[Outputs]
  execute_on = 'timestep_end'
  exodus = true
[]
