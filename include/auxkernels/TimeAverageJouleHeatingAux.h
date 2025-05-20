#pragma once

#include "MFEMAuxSolver.h"

class TimeAverageJouleHeatingCoefficient : public hephaestus::CoupledCoefficient
{
private:
  mfem::Coefficient * sigma{nullptr};
  mfem::ParGridFunction * joule_heating_gf{nullptr};
  mfem::ParGridFunction * avg_joule_heating_gf{nullptr};

  std::string conductivity_coef_name;
  
  double time_step;
  double skip;

  double current_time{0.0};
  double start_time{0.0};
  

public:
  TimeAverageJouleHeatingCoefficient(const hephaestus::InputParameters & params)
    : hephaestus::CoupledCoefficient(params),
      conductivity_coef_name(params.GetParam<std::string>("ConductivityCoefName")),
      time_step(params.GetParam<double>("time_step")),
      skip(params.GetParam<double>("skip"))

  {}

  void Init(const hephaestus::GridFunctions & variables, hephaestus::Coefficients & coefficients)
  {
    std::cout << "Intialising TimeAverageJouleHeating";
    hephaestus::CoupledCoefficient::Init(variables, coefficients);
    sigma = coefficients._scalars.Get(conductivity_coef_name);

    joule_heating_gf = variables.Get("joule_heating");
    avg_joule_heating_gf = variables.Get("average_joule_heating");
   
  }

  virtual double Eval(mfem::ElementTransformation & T, const mfem::IntegrationPoint & ip)
  {
    mfem::Vector E;
    _gf->GetVectorValue(T, ip, E);
    double thisSigma = sigma->Eval(T, ip);

    double joule_heat = thisSigma * (E * E);
    //  return thisSigma * (E * E);

    double previous_avg = avg_joule_heating_gf->GetValue(T, ip);
    double weight = (current_time > skip) ? (time_step / (current_time - skip)) : 0.0;
    double avg_joule_heat = (1.0 - weight) * previous_avg + weight * joule_heat;

    // return avg_joule_heat;
    return avg_joule_heat;
  }

  void Solve(double t)
  {
    current_time = t;
    this->SetTime(t);
    avg_joule_heating_gf->ProjectCoefficient(*this);
  }
};

class MFEMTimeAverageJouleHeatingAux : public MFEMAuxSolver
{
public:
  static InputParameters validParams();

  MFEMTimeAverageJouleHeatingAux(const InputParameters & parameters);
  virtual ~MFEMTimeAverageJouleHeatingAux();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  inline std::shared_ptr<hephaestus::AuxSolver> getAuxSolver() const override
  {
    return avg_joule_heating_aux;
  }

  virtual void storeCoefficients(hephaestus::Coefficients & coefficients) override;

protected:
  hephaestus::InputParameters avg_joule_heating_params;
  std::shared_ptr<TimeAverageJouleHeatingCoefficient> avg_joule_heating_aux{nullptr};
};
