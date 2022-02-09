#pragma once

#include "MooseObject.h"


class MFEMInputs : public MooseObject
{
public:
  static InputParameters validParams();

  MFEMInputs(const InputParameters & parameters);
  virtual ~MFEMInputs();

  std::string _problem_type;
  std::string _input_mesh;
  std::vector<std::vector<BoundaryName>> _bcs;
  std::vector<std::vector<SubdomainName>> _mats;

protected:

};
