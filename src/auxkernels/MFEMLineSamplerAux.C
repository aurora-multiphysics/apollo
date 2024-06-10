#include "MFEMLineSamplerAux.h"

registerMooseObject("ApolloApp", MFEMLineSamplerAux);

InputParameters
MFEMLineSamplerAux::validParams()
{
  InputParameters params = MFEMAuxSolver::validParams();
  params.addRequiredParam<Point>("start_point", "The beginning of the line");
  params.addRequiredParam<Point>("end_point", "The end of the line");
  params.addRequiredParam<unsigned int>("num_points",
                                        "The number of points to sample along the line");
  params.addRequiredParam<UserObjectName>(
      "variable", "The FESpace associated with the test variable the source will be applied to.");
  params.addRequiredParam<std::string>("header", "Header for the output CSV.");
  params.addRequiredParam<std::string>("filename", "Name of the output CSV.");
  return params;
}

MFEMLineSamplerAux::MFEMLineSamplerAux(const InputParameters & parameters)
  : MFEMAuxSolver(parameters),
    _mfem_variable(getUserObject<MFEMVariable>("variable")),
    _start_point(getParam<Point>("start_point")),
    _end_point(getParam<Point>("end_point")),
    _start_vec({_start_point(0), _start_point(1), _start_point(2)}),
    _end_vec({_end_point(0), _end_point(1), _end_point(2)}),
    _num_points(getParam<unsigned int>("num_points")),
    _line_sampler_aux{
        std::make_shared<hephaestus::LineSamplerAux>(_mfem_variable.name(),
                                                     _start_vec,
                                                     _end_vec,
                                                     _num_points,
                                                     getParam<std::string>("filename"),
                                                     getParam<std::string>("header"))}
{
  _line_sampler_aux->SetPriority(10);
}

MFEMLineSamplerAux::~MFEMLineSamplerAux() {}
