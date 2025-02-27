#pragma once

#include "MFEMAuxSolver.h"

/*
  class TimeAverageJouleHeatingCoefficient : public hephaestus::CoupledCoefficient
{
private:
  mfem::Coefficient * sigma{nullptr};
  mfem::ParGridFunction * joule_heating_gf{nullptr};
  mfem::ParGridFunction * electric_field_gf{nullptr};

  std::string conductivity_coef_name;
  std::string electric_field_name;

  double skip{0.0};    // Time interval to start averaging
  bool average{true};  // Flag to control averaging

public:
  TimeAverageJouleHeatingCoefficient(const hephaestus::InputParameters & params)
    : hephaestus::CoupledCoefficient(params),
      conductivity_coef_name(params.GetParam<std::string>("ConductivityCoefName")),
      electric_field_name(params.GetParam<std::string>("ElectricFieldName")),
      skip(params.GetParam<double>("Skip")),
      average(params.GetParam<bool>("Average"))
  {
  }

  void Init(const hephaestus::GridFunctions & variables, hephaestus::Coefficients & coefficients)
  {
    hephaestus::CoupledCoefficient::Init(variables, coefficients);

    sigma = coefficients._scalars.Get(conductivity_coef_name);
    electric_field_gf = variables.Get(electric_field_name);
    joule_heating_gf = variables.Get("avg_joule_heating");
  }

  virtual double Eval(mfem::ElementTransformation & T, const mfem::IntegrationPoint & ip)
  {
    mfem::Vector E;
    double thisSigma;

    electric_field_gf->GetVectorValue(T, ip, E);
    thisSigma = sigma->Eval(T, ip);

    double joule_heat = thisSigma * (E * E);

    // Time-averaging logic
    double weight = (_t > skip) ? (average ? _dt / (_t - skip) : 1) : 0;
    double previous_avg = joule_heating_gf->GetValue(T.ElementNo);
    double avg_joule_heat = (1 - weight) * previous_avg + weight * joule_heat;

    return avg_joule_heat;
  }

  void Solve(double t)
  {
    this->SetTime(t);
    joule_heating_gf->ProjectCoefficient(*this);
  }
};
*/
//constructor initialising the class and inherits from Hephaestus::CoupledCoefficient 
class TimeAverageJouleHeatingCoefficient : public hephaestus::CoupledCoefficient
{
private:
  mfem::Coefficient * sigma{nullptr};
  mfem::ParGridFunction * joule_heating_gf{nullptr};
  mfem::ParGridFunction * electric_field_gf{nullptr}; // Direct input

  std::string conductivity_coef_name;
  std::string electric_field_name;

  double skip{0.0};    // Time interval to start averaging
  bool average{true};  // Flag to control averaging

  // Store time and time step explicitly
  double current_time{0.0};
  double time_step{0.0};

public:
  TimeAverageJouleHeatingCoefficient(const hephaestus::InputParameters & params)
    : hephaestus::CoupledCoefficient(params),
      conductivity_coef_name(params.GetParam<std::string>("ConductivityCoefName")),
      electric_field_name(params.GetParam<std::string>("ElectricFieldName")),
      skip(params.GetParam<double>("Skip")),
      average(params.GetParam<bool>("Average"))
  {
  }

  void Init(const hephaestus::GridFunctions & variables, hephaestus::Coefficients & coefficients)
  {
    hephaestus::CoupledCoefficient::Init(variables, coefficients);

    // Get conductivity from Hephaestus's coefficient registry
    sigma = coefficients._scalars.Get(conductivity_coef_name);

    // Access electric field directly from MFEM
    electric_field_gf = variables.Get(electric_field_name);

    // Initialize Joule Heating GridFunction
    joule_heating_gf = variables.Get("avg_joule_heating");
  }

  virtual double Eval(mfem::ElementTransformation & T, const mfem::IntegrationPoint & ip)
  {
    mfem::Vector E;
    double thisSigma;

    // Get the electric field value at the integration point (From MFEM library function stored in Hephastus)
    electric_field_gf->GetVectorValue(T, ip, E);
    thisSigma = sigma->Eval(T, ip); //Conductivity at the integration point (for anisotropic material)

    // Calculate instantaneous Joule heating
    double joule_heat = thisSigma * (E * E); //(E*E) is the dot product of the electric field vector with itself
                                             // since E is a vector overloaded with the * operator from MFEM

    // Time-averaging logic, calculates the weights for the averaging, ensuring it starts only after the skip time
    // Tenary operator ?(if true) : (else if false) - compact form of if-else statement
    double weight = (current_time > skip) ? (average ? time_step / (current_time - skip) : 1) : 0;

    // Get the previous average value at the integration point
    double previous_avg = joule_heating_gf->GetValue(T, ip);

    // Calculate the new time-averaged Joule heating
    double avg_joule_heat = (1 - weight) * previous_avg + weight * joule_heat;

    return avg_joule_heat;
  }

  void Solve(double t, double dt) //called once per time step in MOOSE (Apollo)
  {
    // Store time and time step explicitly for the Aux Kernel
    current_time = t;
    time_step = dt;
    //triger the projections of the coefficient
    this->SetTime(t);
    joule_heating_gf->ProjectCoefficient(*this); //loops over all elements and integration points and calls Eval
                                                //and stores the result in the grid function "joule_heating_gf"
  }
};

class TimeAverageJouleHeatingAux : public MFEMAuxSolver
{
public:
  static InputParameters validParams();

  TimeAverageJouleHeatingAux(const InputParameters & parameters);
  virtual ~TimeAverageJouleHeatingAux();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  inline std::shared_ptr<hephaestus::AuxSolver> getAuxSolver() const override
  {
    return time_avg_joule_heating_aux;
  }

  virtual void storeCoefficients(hephaestus::Coefficients & coefficients) override;

protected:
  hephaestus::InputParameters time_avg_joule_heating_params;
  std::shared_ptr<TimeAverageJouleHeatingCoefficient> time_avg_joule_heating_aux{nullptr};
};



