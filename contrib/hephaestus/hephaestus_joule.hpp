// Copyright (c) 2010-2021, Lawrence Livermore National Security, LLC. Produced
// at the Lawrence Livermore National Laboratory. All Rights reserved. See files
// LICENSE and NOTICE for details. LLNL-CODE-806117.
//
// This file is part of the MFEM library. For more information and source code
// availability visit https://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the BSD-3 license. We welcome feedback and contributions, see file
// CONTRIBUTING.md for details.

#ifndef MFEM_JOULE_SOLVER
#define MFEM_JOULE_SOLVER

#include "../common/pfem_extras.hpp"
#include "../common/mesh_extras.hpp"
#include "electromagnetics.hpp"
// #ifdef MFEM_USE_MPI

#include <memory>
#include <iostream>
#include <fstream>

namespace mfem
{

namespace electromagnetics
{

// Some global variable for convenience
const double       SOLVER_TOL = 1.0e-9;
const int       SOLVER_MAX_IT = 1000;
// Initialized in joule.cpp and used in joule_solver.cpp:
extern int SOLVER_PRINT_LEVEL;
extern int        STATIC_COND;

// These are defined in joule.cpp
void edot_bc(const Vector &x, Vector &E);
void e_exact(const Vector &x, double t, Vector &E);
void b_exact(const Vector &x, double t, Vector &B);
double p_bc(const Vector &x, double t);
double t_exact(const Vector &x);

// A Coefficient is an object with a function Eval that returns a double.  A
// MeshDependentCoefficient returns a different value depending upon the given
// mesh attribute, i.e. a "material property".
// Somewhat inefficiently, this is achieved using a GridFunction.
class MeshDependentCoefficient: public Coefficient
{
private:
   std::map<int, double> *materialMap;
   double scaleFactor;
public:
   MeshDependentCoefficient(const std::map<int, double> &inputMap,
                            double scale = 1.0);
   MeshDependentCoefficient(const MeshDependentCoefficient &cloneMe);
   virtual double Eval(ElementTransformation &T, const IntegrationPoint &ip);
   void SetScaleFactor(const double &scale) { scaleFactor = scale; }
   virtual ~MeshDependentCoefficient()
   {
      if (materialMap != NULL) { delete materialMap; }
   }
};

// This Coefficient is a product of a GridFunction and MeshDependentCoefficient
// for example if T (temperature) is a GridFunction and c (heat capacity) is a
// MeshDependentCoefficient, this function can compute c*T.
class ScaledGFCoefficient: public GridFunctionCoefficient
{
private:
   MeshDependentCoefficient mdc;
public:
   ScaledGFCoefficient(GridFunction *gf, MeshDependentCoefficient &input_mdc);
   virtual double Eval(ElementTransformation &T, const IntegrationPoint &ip);
   void SetMDC(const MeshDependentCoefficient &input_mdc) { mdc = input_mdc; }
   virtual ~ScaledGFCoefficient() {}
};

/**
   After spatial discretization, the magnetic diffusion equation can be written
   as a system of ODEs:

      S0(sigma) P = 0
      dE/dt       = - (M1(sigma) + dt S1(1/mu))^{-1}*(S1(1/mu)*E + sigma Grad P)
      dB/dt       = - Curl(E)
      dF/dt       = (M2(c/k) + dt S2(1))^{-1} (-S2(1) F + Div J)
      dcT/dt      = -Div F + J

   where

     P is the 0-form electrostatic potential,
     E is the 1-form electric field,
     B is the 2-form magnetic flux,
     F is the 2-form thermal flux,
     T is the 3-form temperature.
     M is the mass matrix,
     S is the stiffness matrix,
     Curl is the curl matrix,
     Div is the divergence matrix,
     sigma is the electrical conductivity,
     k is the thermal conductivity,
     c is the heat capacity,
     J is a function of the Joule heating sigma (E dot E).

   Class MagneticDiffusionEOperator represents the right-hand side of the above
   system of ODEs.
*/
class MagneticDiffusionEOperator : public TimeDependentOperator
{
protected:
   // These ParFiniteElementSpace objects provide degree-of-freedom mappings.
   // To create these you must provide the mesh and the definition of the FE
   // space. These objects are used to create hypre vectors to store the DOFs,
   // they are used to create grid functions to perform FEM interpolation, and
   // they are used by bilinear forms.
   ParFiniteElementSpace &L2FESpace;
   ParFiniteElementSpace &HCurlFESpace;
   ParFiniteElementSpace &HDivFESpace;
   ParFiniteElementSpace &HGradFESpace;

   // ParBilinearForms are used to create sparse matrices representing discrete
   // linear operators.
   ParBilinearForm *a0, *a1, *a2, *m1, *m2, *m3, *s1, *s2;
   ParDiscreteLinearOperator *grad, *curl;
   ParMixedBilinearForm *weakDiv, *weakDivC, *weakCurl;

   // Hypre matrices and vectors for 1-form systems A1 X1 = B1 and 2-form
   // systems A2 = X2 = B2
   HypreParMatrix *A0, *A1, *A2, *M1, *M2, *M3;
   Vector *X0, *X1, *X2, *B0, *B1, *B2, *B3;

   // temporary work vectors
   ParGridFunction *v0, *v1, *v2;

   // HypreSolver is derived from Solver, which is derived from Operator. So a
   // HypreSolver object has a Mult() operator, which is actually the solver
   // operation y = A^-1 x i.e. multiplication by A^-1.
   // HyprePCG is a wrapper for hypre's preconditioned conjugate gradient.
   mutable HypreSolver * amg_a0;
   mutable HyprePCG    * pcg_a0;
   mutable HypreSolver * ads_a2;
   mutable HyprePCG    * pcg_a2;
   mutable HypreSolver * ams_a1;
   mutable HyprePCG    * pcg_a1;
   mutable HypreSolver * dsp_m3;
   mutable HyprePCG    * pcg_m3;
   mutable HypreSolver * dsp_m1;
   mutable HyprePCG    * pcg_m1;
   mutable HypreSolver * dsp_m2;
   mutable HyprePCG    * pcg_m2;

   mutable Array<int> ess_bdr;
   mutable Array<int> ess_bdr_vdofs;
   mutable Array<int> thermal_ess_bdr;
   mutable Array<int> thermal_ess_bdr_vdofs;
   mutable Array<int> poisson_ess_bdr;
   mutable Array<int> poisson_ess_bdr_vdofs;

   MeshDependentCoefficient *sigma, *Tcapacity, *InvTcap, *InvTcond;
   double mu, dt_A1, dt_A2;

   // The method builA2 creates the ParBilinearForm a2, the HypreParMatrix A2,
   // and the solver and preconditioner pcg_a2 and amg_a2. The other build
   // functions do similar things.
   void buildA0(MeshDependentCoefficient &sigma);
   void buildA1(double muInv, MeshDependentCoefficient &sigma, double dt);
   void buildA2(MeshDependentCoefficient &InvTcond,
                MeshDependentCoefficient &InvTcap, double dt);
   void buildM1(MeshDependentCoefficient &sigma);
   void buildM2(MeshDependentCoefficient &alpha);
   void buildM3(MeshDependentCoefficient &Tcap);
   void buildS1(double muInv);
   void buildS2(MeshDependentCoefficient &alpha);
   void buildGrad();
   void buildCurl(double muInv);
   void buildDiv( MeshDependentCoefficient &InvTcap);

public:
   MagneticDiffusionEOperator(int len,
                              ParFiniteElementSpace &L2FES,
                              ParFiniteElementSpace &HCurlFES,
                              ParFiniteElementSpace &HDivFES,
                              ParFiniteElementSpace &HGradFES,
                              Array<int> &ess_bdr,
                              Array<int> &thermal_ess_bdr,
                              Array<int> &poisson_ess_bdr,
                              double mu,
                              std::map<int, double> sigmaAttMap,
                              std::map<int, double> TcapacityAttMap,
                              std::map<int, double> InvTcapAttMap,
                              std::map<int, double> InvTcondAttMap
                             );

   // Initialize the fields. This is where restart would go to.
   void Init(Vector &vx);

   // class TimeDependentOperator is derived from Operator, and class Operator
   // has the virtual function Mult(x,y) which computes y = A x for some matrix
   // A, or more generally a nonlinear operator y = A(x).
   virtual void Mult(const Vector &vx, Vector &dvx_dt) const;

   // Solve the Backward-Euler equation: k = f(x + dt*k, t), for the unknown
   // slope k. This is the only requirement for high-order SDIRK implicit
   // integration. This is a virtual function of class TimeDependentOperator.
   virtual void ImplicitSolve(const double dt, const Vector &x, Vector &k);

   // Compute B^T M2 B, where M2 is the HDiv mass matrix with permeability
   // coefficient.
   // double MagneticEnergy(ParGridFunction &B_gf) const;

   // Compute E^T M1 E, where M1 is the HCurl mass matrix with conductivity
   // coefficient.
   double ElectricLosses(ParGridFunction &E_gf) const;

   // E is the input, w is the output which is L2 heating.
   void GetJouleHeating(ParGridFunction &E_gf, ParGridFunction &w_gf) const;

   void SetTime(const double t_);

   // Write all the hypre matrices and vectors to disk.
   void Debug(const char *basefilename, double time);

   virtual ~MagneticDiffusionEOperator();
};

// A Coefficient is an object with a function Eval that returns a double. The
// JouleHeatingCoefficient object will contain a reference to the electric field
// grid function, and the conductivity sigma, and returns sigma E dot E at a
// point.
class JouleHeatingCoefficient: public Coefficient
{
private:
   ParGridFunction &E_gf;
   MeshDependentCoefficient sigma;
public:
   JouleHeatingCoefficient(const MeshDependentCoefficient &sigma_,
                           ParGridFunction &E_gf_)
      : E_gf(E_gf_), sigma(sigma_) {}
   virtual double Eval(ElementTransformation &T, const IntegrationPoint &ip);
   virtual ~JouleHeatingCoefficient() {}
};

} // namespace electromagnetics

} // namespace mfem

// #endif // MFEM_USE_MPI

#endif // MFEM_JOULE_SOLVER

// Copyright (c) 2010-2021, Lawrence Livermore National Security, LLC. Produced
// at the Lawrence Livermore National Laboratory. All Rights reserved. See files
// LICENSE and NOTICE for details. LLNL-CODE-806117.
//
// This file is part of the MFEM library. For more information and source code
// availability visit https://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the BSD-3 license. We welcome feedback and contributions, see file
// CONTRIBUTING.md for details.

#include "joule_solver.hpp"

// #ifdef MFEM_USE_MPI

using namespace std;

namespace mfem
{

using namespace common;

namespace electromagnetics
{

MagneticDiffusionEOperator::MagneticDiffusionEOperator(
   int stateVectorLen,
   ParFiniteElementSpace &L2FES,
   ParFiniteElementSpace &HCurlFES,
   ParFiniteElementSpace &HDivFES,
   ParFiniteElementSpace &HGradFES,
   Array<int> &ess_bdr_arg,
   Array<int> &thermal_ess_bdr_arg,
   Array<int> &poisson_ess_bdr_arg,
   double mu_coef,
   std::map<int, double> sigmaAttMap,
   std::map<int, double> TcapacityAttMap,
   std::map<int, double> InvTcapAttMap,
   std::map<int, double> InvTcondAttMap)

   : TimeDependentOperator(stateVectorLen, 0.0),
     L2FESpace(L2FES), HCurlFESpace(HCurlFES), HDivFESpace(HDivFES),
     HGradFESpace(HGradFES),
     a0(NULL), a1(NULL), a2(NULL), m1(NULL), m2(NULL), m3(NULL),
     s1(NULL), s2(NULL), grad(NULL), curl(NULL), weakDiv(NULL), weakDivC(NULL),
     weakCurl(NULL),
     A0(NULL), A1(NULL), A2(NULL), M1(NULL), M2(NULL), M3(NULL),
     X0(NULL), X1(NULL), X2(NULL), B0(NULL), B1(NULL), B2(NULL), B3(NULL),
     v0(NULL), v1(NULL), v2(NULL),
     amg_a0(NULL), pcg_a0(NULL), ads_a2(NULL), pcg_a2(NULL), ams_a1(NULL),
     pcg_a1(NULL), dsp_m3(NULL),pcg_m3(NULL),
     dsp_m1(NULL), pcg_m1(NULL), dsp_m2(NULL), pcg_m2(NULL),
     mu(mu_coef), dt_A1(-1.0), dt_A2(-1.0)
{
   ess_bdr.SetSize(ess_bdr_arg.Size());
   for (int i=0; i<ess_bdr_arg.Size(); i++)
   {
      ess_bdr[i] = ess_bdr_arg[i];
   }
   thermal_ess_bdr.SetSize(thermal_ess_bdr_arg.Size());
   for (int i=0; i<thermal_ess_bdr_arg.Size(); i++)
   {
      thermal_ess_bdr[i] = thermal_ess_bdr_arg[i];
   }
   poisson_ess_bdr.SetSize(poisson_ess_bdr_arg.Size());
   for (int i=0; i<poisson_ess_bdr_arg.Size(); i++)
   {
      poisson_ess_bdr[i] = poisson_ess_bdr_arg[i];
   }

   sigma     = new MeshDependentCoefficient(sigmaAttMap);
   Tcapacity = new MeshDependentCoefficient(TcapacityAttMap);
   InvTcap   = new MeshDependentCoefficient(InvTcapAttMap);
   InvTcond  = new MeshDependentCoefficient(InvTcondAttMap);

   this->buildA0(*sigma);
   this->buildM3(*Tcapacity);
   this->buildM1(*sigma);
   this->buildM2(*InvTcond);
   this->buildS2(*InvTcap);
   this->buildS1(1.0/mu);
   this->buildCurl(1.0/mu);
   this->buildDiv(*InvTcap);
   this->buildGrad();

   v0 = new ParGridFunction(&HGradFESpace);
   v1 = new ParGridFunction(&HCurlFESpace);
   v2 = new ParGridFunction(&HDivFESpace);
   A0 = new HypreParMatrix;
   A1 = new HypreParMatrix;
   A2 = new HypreParMatrix;
   X0 = new Vector;
   X1 = new Vector;
   X2 = new Vector;
   B0 = new Vector;
   B1 = new Vector;
   B2 = new Vector;
   B3 = new Vector;
}

void MagneticDiffusionEOperator::Init(Vector &X)
{
   Vector zero_vec(3); zero_vec = 0.0;
   VectorConstantCoefficient Zero_vec(zero_vec);
   ConstantCoefficient Zero(0.0);

   // The big BlockVector stores the fields as follows:
   //    Temperature
   //    Temperature Flux
   //    P field
   //    E field
   //    B field
   //    Joule Heating

   int Vsize_l2 = L2FESpace.GetVSize();
   int Vsize_nd = HCurlFESpace.GetVSize();
   int Vsize_rt = HDivFESpace.GetVSize();
   int Vsize_h1 = HGradFESpace.GetVSize();

   Array<int> true_offset(7);
   true_offset[0] = 0;
   true_offset[1] = true_offset[0] + Vsize_l2;
   true_offset[2] = true_offset[1] + Vsize_rt;
   true_offset[3] = true_offset[2] + Vsize_h1;
   true_offset[4] = true_offset[3] + Vsize_nd;
   true_offset[5] = true_offset[4] + Vsize_rt;
   true_offset[6] = true_offset[5] + Vsize_l2;

   Vector* xptr = (Vector*) &X;
   ParGridFunction E, B, T, F, W, P;
   T.MakeRef(&L2FESpace,   *xptr,true_offset[0]);
   F.MakeRef(&HDivFESpace, *xptr,true_offset[1]);
   P.MakeRef(&HGradFESpace,*xptr,true_offset[2]);
   E.MakeRef(&HCurlFESpace,*xptr,true_offset[3]);
   B.MakeRef(&HDivFESpace, *xptr,true_offset[4]);
   W.MakeRef(&L2FESpace,   *xptr,true_offset[5]);

   E.ProjectCoefficient(Zero_vec);
   B.ProjectCoefficient(Zero_vec);
   F.ProjectCoefficient(Zero_vec);
   T.ProjectCoefficient(Zero);
   P.ProjectCoefficient(Zero);
   W.ProjectCoefficient(Zero);
}

/*
This is an experimental Mult() method for explicit integration.
Not recommended for actual use.

S0 P  = 0
M1 E  = WeakCurl^T B + Grad P
   dB = -Curl E
M2 F  = WeakDiv^T T
M3 dT = WeakDiv F + W

where W is the Joule heating.

Boundary conditions are applied to E.  No boundary conditions are applied to B.
Since we are using Hdiv, zero flux is an essential BC on F.  P is given by Div
sigma Grad P = 0 with appropriate BC's.
*/
void MagneticDiffusionEOperator::Mult(const Vector &X, Vector &dX_dt) const
{
   dX_dt = 0.0;

   // The big BlockVector stores the fields as follows:
   //    Temperature
   //    Temperature Flux
   //    P field
   //    E field
   //    B field
   //    Joule Heating

   int Vsize_l2 = L2FESpace.GetVSize();
   int Vsize_nd = HCurlFESpace.GetVSize();
   int Vsize_rt = HDivFESpace.GetVSize();
   int Vsize_h1 = HGradFESpace.GetVSize();

   Array<int> true_offset(7);
   true_offset[0] = 0;
   true_offset[1] = true_offset[0] + Vsize_l2;
   true_offset[2] = true_offset[1] + Vsize_rt;
   true_offset[3] = true_offset[2] + Vsize_h1;
   true_offset[4] = true_offset[3] + Vsize_nd;
   true_offset[5] = true_offset[4] + Vsize_rt;
   true_offset[6] = true_offset[5] + Vsize_l2;

   Vector* xptr = (Vector*) &X;
   ParGridFunction E, B, T, F, W, P;
   T.MakeRef(&L2FESpace,   *xptr,true_offset[0]);
   F.MakeRef(&HDivFESpace, *xptr,true_offset[1]);
   P.MakeRef(&HGradFESpace,*xptr,true_offset[2]);
   E.MakeRef(&HCurlFESpace,*xptr,true_offset[3]);
   B.MakeRef(&HDivFESpace, *xptr,true_offset[4]);
   W.MakeRef(&L2FESpace,   *xptr,true_offset[5]);

   ParGridFunction dE, dB, dT, dF, dW, dP;
   dT.MakeRef(&L2FESpace,   dX_dt,true_offset[0]);
   dF.MakeRef(&HDivFESpace, dX_dt,true_offset[1]);
   dP.MakeRef(&HGradFESpace,dX_dt,true_offset[2]);
   dE.MakeRef(&HCurlFESpace,dX_dt,true_offset[3]);
   dB.MakeRef(&HDivFESpace, dX_dt,true_offset[4]);
   dW.MakeRef(&L2FESpace,   dX_dt,true_offset[5]);

   // db = - Curl E
   curl->Mult(E, dB);
   dB *= -1.0;

   // form the Laplacian and solve it
   ParGridFunction Phi_gf(&HGradFESpace);

   // p_bc is given function defining electrostatic potential on surface
   FunctionCoefficient voltage(p_bc);
   voltage.SetTime(this->GetTime());
   Phi_gf = 0.0;

   // the line below is currently not fully supported on AMR meshes
   // Phi_gf.ProjectBdrCoefficient(voltage,poisson_ess_bdr);

   // this is a hack to get around the above issue
   Phi_gf.ProjectCoefficient(voltage);
   // end of hack

   // apply essential BC's and apply static condensation, the new system to
   // solve is A0 X0 = B0
   Array<int> poisson_ess_tdof_list;
   HGradFESpace.GetEssentialTrueDofs(poisson_ess_bdr, poisson_ess_tdof_list);

   *v0 = 0.0;
   a0->FormLinearSystem(poisson_ess_tdof_list,Phi_gf,*v0,*A0,*X0,*B0);

   if (amg_a0 == NULL) { amg_a0 = new HypreBoomerAMG(*A0); }
   if (pcg_a0 == NULL)
   {
      pcg_a0 = new HyprePCG(*A0);
      pcg_a0->SetTol(SOLVER_TOL);
      pcg_a0->SetMaxIter(SOLVER_MAX_IT);
      pcg_a0->SetPrintLevel(SOLVER_PRINT_LEVEL);
      pcg_a0->SetPreconditioner(*amg_a0);
   }
   // pcg "Mult" operation is a solve
   // X0 = A0^-1 * B0
   pcg_a0->Mult(*B0, *X0);

   // "undo" the static condensation using dP as a temporary variable, dP stores
   // Pnew
   a0->RecoverFEMSolution(*X0,*v0,P);
   dP = 0.0;

   // v1 = <1/mu v, curl u> B
   // B is a grid function but weakCurl is not parallel assembled so is OK
   weakCurl->MultTranspose(B, *v1);

   // now add Grad dPhi/dt term
   // use E as a temporary, E = Grad P
   // v1 = curl 1/mu B + M1 * Grad P
   // note: these two steps could be replaced by one step if we have the
   // bilinear form <sigma gradP, E>
   grad->Mult(P,E);
   m1->AddMult(E,*v1,1.0);

   // OK now v1 is the right hand side, just need to add essential BC's

   ParGridFunction J_gf(&HCurlFESpace);

   // edot_bc is time-derivative E-field on a boundary surface and then it is
   // used as a Dirichlet BC.

   VectorFunctionCoefficient Jdot(3, edot_bc);
   J_gf = 0.0;
   J_gf.ProjectBdrCoefficientTangent(Jdot,ess_bdr);

   // apply essential BC's and apply static condensation
   // the new system to solve is M1 X1 = B1
   Array<int> ess_tdof_list;
   HCurlFESpace.GetEssentialTrueDofs(ess_bdr, ess_tdof_list);

   m1->FormLinearSystem(ess_tdof_list,J_gf,*v1,*A1,*X1,*B1);

   if (dsp_m1 == NULL) { dsp_m1 = new HypreDiagScale(*A1); }
   if (pcg_m1 == NULL)
   {
      pcg_m1 = new HyprePCG(*A1);
      pcg_m1->SetTol(SOLVER_TOL);
      pcg_m1->SetMaxIter(SOLVER_MAX_IT);
      pcg_m1->SetPrintLevel(SOLVER_PRINT_LEVEL);
      pcg_m1->SetPreconditioner(*dsp_m1);
   }
   // pcg "Mult" operation is a solve
   // X1 = M1^-1 * B1 = M1^-1 (-S1 E)
   pcg_m1->Mult(*B1, *X1);

   // "undo" the static condensation and fill in grid function dE
   m1->RecoverFEMSolution(*X1,*v1,E);
   dE = 0.0;

   // the total field is E_tot = E_ind - Grad Phi
   // so we need to subtract out Grad Phi
   // E = E - grad (P)
   grad->AddMult(P,E,-1.0);

   // Compute Joule heating using the previous value of E
   this->GetJouleHeating(E,W);
   dW = 0.0;

   // Mult(x,y,alpha=1,beta=0)
   // y = alpha*A*x + beta*y
   // giving
   // v2 = <v, div u> * T
   weakDiv->Mult(T, *v2);

   // apply the thermal BC
   // recall for Hdiv formulation the essential BC is on the flux
   Vector zero_vec(3); zero_vec = 0.0;
   VectorConstantCoefficient Zero_vec(zero_vec);
   ParGridFunction F_gf(&HDivFESpace);
   F_gf = 0.0;
   F_gf.ProjectBdrCoefficientNormal(Zero_vec,thermal_ess_bdr);

   // apply essential BC's and apply static condensation
   // the new system to solve is M2 X2 = B2
   Array<int> thermal_ess_tdof_list;
   HDivFESpace.GetEssentialTrueDofs(thermal_ess_bdr, thermal_ess_tdof_list);

   m2->FormLinearSystem(thermal_ess_tdof_list,F_gf,*v2,*A2,*X2,*B2);

   if (dsp_m2 == NULL) { dsp_m2 = new HypreDiagScale(*A2); }
   if (pcg_m2 == NULL)
   {
      pcg_m2 = new HyprePCG(*A2);
      pcg_m2->SetTol(SOLVER_TOL);
      pcg_m2->SetMaxIter(SOLVER_MAX_IT);
      pcg_m2->SetPrintLevel(SOLVER_PRINT_LEVEL);
      pcg_m2->SetPreconditioner(*dsp_m2);
   }
   // X2 = m2^-1 * B2
   pcg_m2->Mult(*B2, *X2);

   // "undo" the static condensation and fill in grid function dF
   m2->RecoverFEMSolution(*X2,*v2,F);

   // Compute dT using previous value of flux
   // dT = [w - div F]
   //
   // <u,u> dT = <1/c W,u> - <1/c div v,u> F
   //
   // where W is Joule heating and F is the flux that we just computed
   //
   // note: if div is a BilinearForm, then W should be converted to a LoadVector

   GridFunctionCoefficient Wcoeff(&W);
   ParLinearForm temp_lf(&L2FESpace);

   // compute load vector < W, u>
   temp_lf.AddDomainIntegrator(new DomainLFIntegrator(Wcoeff));
   temp_lf.Assemble();
   // lf = lf - div F
   weakDiv->AddMult(F, temp_lf, -1.0);

   // if div is a BilinearForm, need to perform mass matrix solve to convert
   // energy cT to temperature T

   if (dsp_m3 == NULL) { dsp_m3 = new HypreDiagScale(*M3); }
   if (pcg_m3 == NULL)
   {
      pcg_m3 = new HyprePCG(*M3);
      pcg_m3->SetTol(SOLVER_TOL);
      pcg_m3->SetMaxIter(SOLVER_MAX_IT);
      pcg_m3->SetPrintLevel(SOLVER_PRINT_LEVEL);
      pcg_m3->SetPreconditioner(*dsp_m3);
   }
   // solve for dT from M3 dT = lf
   // no boundary conditions on this solve
   pcg_m3->Mult(temp_lf, dT);
}

/*
This is the main computational code that computes dX/dt implicitly
where X is the state vector containing P, E, B, F, T, and W

        S0 P = 0
(M1+dt S1) E = WeakCurl^T B + Grad P
          dB = -Curl E
(M2+dt S2) F = WeakDiv^T T
       M3 dT = WeakDiv F + W

where W is the Joule heating.

Boundary conditions are applied to E.  Boundary conditions are applied to F.  No
boundary conditions are applied to B or T.

The W term in the left hand side is the Joule heating which is a nonlinear
(quadratic) function of E.

P is solution of Div sigma Grad dP = 0.

The total E-field is given by E_tot = E_ind - Grad P, the big equation for E
above is really for E_ind (the induced, or solenoidal, component) and this is
corrected for.
*/
void MagneticDiffusionEOperator::ImplicitSolve(const double dt,
                                               const Vector &X, Vector &dX_dt)
{
   if ( A2 == NULL || fabs(dt-dt_A2) > 1.0e-12*dt )
   {
      this->buildA2(*InvTcond, *InvTcap, dt);
   }
   if ( A1 == NULL || fabs(dt-dt_A1) > 1.0e-12*dt )
   {
      this->buildA1(1.0/mu, *sigma, dt);
   }

   dX_dt = 0.0;

   // The big BlockVector stores the fields as follows:
   //    Temperature
   //    Temperature Flux
   //    P field
   //    E field
   //    B field
   //    Joule Heating

   int Vsize_l2 = L2FESpace.GetVSize();
   int Vsize_nd = HCurlFESpace.GetVSize();
   int Vsize_rt = HDivFESpace.GetVSize();
   int Vsize_h1 = HGradFESpace.GetVSize();

   Array<int> true_offset(7);
   true_offset[0] = 0;
   true_offset[1] = true_offset[0] + Vsize_l2;
   true_offset[2] = true_offset[1] + Vsize_rt;
   true_offset[3] = true_offset[2] + Vsize_h1;
   true_offset[4] = true_offset[3] + Vsize_nd;
   true_offset[5] = true_offset[4] + Vsize_rt;
   true_offset[6] = true_offset[5] + Vsize_l2;

   Vector* xptr  = (Vector*) &X;
   ParGridFunction E, B, T, F, W, P;
   T.MakeRef(&L2FESpace,   *xptr,true_offset[0]);
   F.MakeRef(&HDivFESpace, *xptr,true_offset[1]);
   P.MakeRef(&HGradFESpace,*xptr,true_offset[2]);
   E.MakeRef(&HCurlFESpace,*xptr,true_offset[3]);
   B.MakeRef(&HDivFESpace, *xptr,true_offset[4]);
   W.MakeRef(&L2FESpace,   *xptr,true_offset[5]);

   ParGridFunction dE, dB, dT, dF, dW, dP;
   dT.MakeRef(&L2FESpace,   dX_dt,true_offset[0]);
   dF.MakeRef(&HDivFESpace, dX_dt,true_offset[1]);
   dP.MakeRef(&HGradFESpace,dX_dt,true_offset[2]);
   dE.MakeRef(&HCurlFESpace,dX_dt,true_offset[3]);
   dB.MakeRef(&HDivFESpace, dX_dt,true_offset[4]);
   dW.MakeRef(&L2FESpace,   dX_dt,true_offset[5]);

   // form the Laplacian and solve it
   ParGridFunction Phi_gf(&HGradFESpace);

   // p_bc is given function defining electrostatic potential on surface
   FunctionCoefficient voltage(p_bc);
   voltage.SetTime(this->GetTime());
   Phi_gf = 0.0;

   // the function below is currently not fully supported on AMR meshes
   // Phi_gf.ProjectBdrCoefficient(voltage,poisson_ess_bdr);

   // this is a hack to get around the above issue
   Phi_gf.ProjectCoefficient(voltage);
   // end of hack

   // apply essential BC's and apply static condensation, the new system to
   // solve is A0 X0 = B0
   Array<int> poisson_ess_tdof_list;
   HGradFESpace.GetEssentialTrueDofs(poisson_ess_bdr, poisson_ess_tdof_list);

   *v0 = 0.0;
   a0->FormLinearSystem(poisson_ess_tdof_list,Phi_gf,*v0,*A0,*X0,*B0);

   if (amg_a0 == NULL) { amg_a0 = new HypreBoomerAMG(*A0); }
   if (pcg_a0 == NULL)
   {
      pcg_a0 = new HyprePCG(*A0);
      pcg_a0->SetTol(SOLVER_TOL);
      pcg_a0->SetMaxIter(SOLVER_MAX_IT);
      pcg_a0->SetPrintLevel(SOLVER_PRINT_LEVEL);
      pcg_a0->SetPreconditioner(*amg_a0);
   }
   // pcg "Mult" operation is a solve
   // X0 = A0^-1 * B0
   pcg_a0->Mult(*B0, *X0);

   // "undo" the static condensation saving result in grid function dP
   a0->RecoverFEMSolution(*X0,*v0,P);
   dP = 0.0;

   // v1 = <1/mu v, curl u> B
   // B is a grid function but weakCurl is not parallel assembled so is OK
   weakCurl->MultTranspose(B, *v1);

   // now add Grad dPhi/dt term
   // use E as a temporary, E = Grad P
   // v1 = curl 1/mu B + M1 * Grad P
   grad->Mult(P,E);
   m1->AddMult(E,*v1,1.0);

   ParGridFunction J_gf(&HCurlFESpace);

   // edot_bc is time-derivative E-field on a boundary surface
   // and then it is used as a Dirichlet BC
   // the vector v1 will be modified by the values Jtmp and
   // the part of the matrix m1 that hs been eliminated (but stored).
   VectorFunctionCoefficient Jdot(3, edot_bc);
   J_gf = 0.0;
   J_gf.ProjectBdrCoefficientTangent(Jdot,ess_bdr);

   // form the linear system, including eliminating essential BC's and applying
   // static condensation. The system to solve is A1 X1 = B1
   Array<int> ess_tdof_list;
   HCurlFESpace.GetEssentialTrueDofs(ess_bdr, ess_tdof_list);

   a1->FormLinearSystem(ess_tdof_list,J_gf,*v1,*A1,*X1,*B1);

   // We only need to create the solver and preconditioner once
   if ( ams_a1 == NULL )
   {
      ParFiniteElementSpace *prec_fespace =
         (a1->StaticCondensationIsEnabled() ? a1->SCParFESpace() : &HCurlFESpace);
      ams_a1 = new HypreAMS(*A1, prec_fespace);
   }
   if ( pcg_a1 == NULL )
   {
      pcg_a1 = new HyprePCG(*A1);
      pcg_a1->SetTol(SOLVER_TOL);
      pcg_a1->SetMaxIter(SOLVER_MAX_IT);
      pcg_a1->SetPrintLevel(SOLVER_PRINT_LEVEL);
      pcg_a1->SetPreconditioner(*ams_a1);
   }
   // solve the system
   // dE = (A1)^-1 [-S1 E]
   pcg_a1->Mult(*B1, *X1);

   // this is required because of static condensation, E is a grid function
   a1->RecoverFEMSolution(*X1,*v1,E);
   dE = 0.0;

   // the total field is E_tot = E_ind - Grad Phi
   // so we need to subtract out Grad Phi
   // E = E - grad (P)
   // note grad maps GF to GF
   grad->AddMult(P,E,-1.0);

   // Compute dB/dt = -Curl(E_{n+1})
   // note curl maps GF to GF
   curl->Mult(E, dB);
   dB *= -1.0;

   // Compute Energy Deposition
   this->GetJouleHeating(E,W);

   // v2 = Div^T * W, where W is the Joule heating computed above, and
   // Div is the matrix <div u, v>
   weakDivC->MultTranspose(W, *v2);
   *v2 *= dt;

   // v2 = <v, div u> T + (1.0)*v2
   weakDiv->AddMultTranspose(T, *v2, 1.0);

   // apply the thermal BC
   Vector zero_vec(3); zero_vec = 0.0;
   VectorConstantCoefficient Zero_vec(zero_vec);
   ParGridFunction F_gf(&HDivFESpace);
   F_gf = 0.0;
   F_gf.ProjectBdrCoefficientNormal(Zero_vec,thermal_ess_bdr);

   // form the linear system, including eliminating essential BC's and applying
   // static condensation. The system to solve is A2 X2 = B2
   Array<int> thermal_ess_tdof_list;
   HDivFESpace.GetEssentialTrueDofs(thermal_ess_bdr, thermal_ess_tdof_list);
   a2->FormLinearSystem(thermal_ess_tdof_list,F_gf,*v2,*A2,*X2,*B2);

   // We only need to create the solver and preconditioner once
   if ( ads_a2 == NULL )
   {
      ParFiniteElementSpace *prec_fespace =
         (a2->StaticCondensationIsEnabled() ? a2->SCParFESpace() : &HDivFESpace);
      ads_a2 = new HypreADS(*A2, prec_fespace);
   }
   if ( pcg_a2 == NULL )
   {
      pcg_a2 = new HyprePCG(*A2);
      pcg_a2->SetTol(SOLVER_TOL);
      pcg_a2->SetMaxIter(SOLVER_MAX_IT);
      pcg_a2->SetPrintLevel(SOLVER_PRINT_LEVEL);
      pcg_a2->SetPreconditioner(*ads_a2);
   }
   // solve for dF from a2 dF = v2
   // dF = (A2)^-1 [S2*F + rhs]
   pcg_a2->Mult(*B2, *X2);

   // this is required because of static condensation
   a2->RecoverFEMSolution(*X2,*v2,F);

   // c dT = [W - div F]
   //
   // <u,u> dT = <1/c W,u> - <1/c div v,u>
   //
   // where W is Joule heating and F is the flux that we just computed
   //
   // note: if div is a BilinearForm, then W should be converted to a LoadVector
   // compute load vector <1/c W, u> where W is the Joule heating GF

   // create the Coefficient 1/c W
   //ScaledGFCoefficient Wcoeff(&W, *InvTcap);
   GridFunctionCoefficient Wcoeff(&W);

   // compute <W,u>
   ParLinearForm temp_lf(&L2FESpace);
   temp_lf.AddDomainIntegrator(new DomainLFIntegrator(Wcoeff));
   temp_lf.Assemble();

   // lf = lf - div F
   weakDiv->AddMult(F, temp_lf, -1.0);

   // need to perform mass matrix solve to get temperature T
   // <c u, u> Tdot = -<div v, u> F +  <1/c W, u>
   // NOTE: supposedly we can just invert any L2 matrix, could do that here
   // instead of a solve

   if (dsp_m3 == NULL) { dsp_m3 = new HypreDiagScale(*M3); }
   if (pcg_m3 == NULL)
   {
      pcg_m3 = new HyprePCG(*M3);
      pcg_m3->SetTol(SOLVER_TOL);
      pcg_m3->SetMaxIter(SOLVER_MAX_IT);
      pcg_m3->SetPrintLevel(SOLVER_PRINT_LEVEL);
      pcg_m3->SetPreconditioner(*dsp_m3);
   }

   // solve for dT from M3 dT = lf
   // no boundary conditions on this solve
   pcg_m3->Mult(temp_lf, dT);
}

void MagneticDiffusionEOperator::buildA0(MeshDependentCoefficient &Sigma)
{
   if ( a0 != NULL ) { delete a0; }

   // First create and assemble the bilinear form.  For now we assume the mesh
   // isn't moving, the materials are time independent, and dt is constant. So
   // we only need to do this once.

   // ConstantCoefficient Sigma(sigma);
   a0 = new ParBilinearForm(&HGradFESpace);
   a0->AddDomainIntegrator(new DiffusionIntegrator(Sigma));
   if (STATIC_COND == 1) { a0->EnableStaticCondensation(); }
   a0->Assemble();

   // Don't finalize or parallel assemble this is done in FormLinearSystem.
}

void MagneticDiffusionEOperator::buildA1(double muInv,
                                         MeshDependentCoefficient &Sigma,
                                         double dt)
{
   if ( a1 != NULL ) { delete a1; }

   // First create and assemble the bilinear form.  For now we assume the mesh
   // isn't moving, the materials are time independent, and dt is constant. So
   // we only need to do this once.

   ConstantCoefficient dtMuInv(dt*muInv);
   a1 = new ParBilinearForm(&HCurlFESpace);
   a1->AddDomainIntegrator(new VectorFEMassIntegrator(Sigma));
   a1->AddDomainIntegrator(new CurlCurlIntegrator(dtMuInv));
   if (STATIC_COND == 1) { a1->EnableStaticCondensation(); }
   a1->Assemble();

   // Don't finalize or parallel assemble this is done in FormLinearSystem.

   dt_A1 = dt;
}

void MagneticDiffusionEOperator::buildA2(MeshDependentCoefficient &InvTcond,
                                         MeshDependentCoefficient &InvTcap,
                                         double dt)
{
   if ( a2 != NULL ) { delete a2; }

   InvTcap.SetScaleFactor(dt);
   a2 = new ParBilinearForm(&HDivFESpace);
   a2->AddDomainIntegrator(new VectorFEMassIntegrator(InvTcond));
   a2->AddDomainIntegrator(new DivDivIntegrator(InvTcap));
   if (STATIC_COND == 1) { a2->EnableStaticCondensation(); }
   a2->Assemble();

   // Don't finalize or parallel assemble this is done in FormLinearSystem.

   dt_A2 = dt;
}

void MagneticDiffusionEOperator::buildM1(MeshDependentCoefficient &Sigma)
{
   if ( m1 != NULL ) { delete m1; }

   m1 = new ParBilinearForm(&HCurlFESpace);
   m1->AddDomainIntegrator(new VectorFEMassIntegrator(Sigma));
   m1->Assemble();

   // Don't finalize or parallel assemble this is done in FormLinearSystem.
}

void MagneticDiffusionEOperator::buildM2(MeshDependentCoefficient &Alpha)
{
   if ( m2 != NULL ) { delete m2; }

   // ConstantCoefficient MuInv(muInv);
   m2 = new ParBilinearForm(&HDivFESpace);
   m2->AddDomainIntegrator(new VectorFEMassIntegrator(Alpha));
   m2->Assemble();

   // Don't finalize or parallel assemble this is done in FormLinearSystem.
}

void MagneticDiffusionEOperator::buildM3(MeshDependentCoefficient &Tcapacity)
{
   if ( m3 != NULL ) { delete m3; }

   // ConstantCoefficient Sigma(sigma);
   m3 = new ParBilinearForm(&L2FESpace);
   m3->AddDomainIntegrator(new MassIntegrator(Tcapacity));
   m3->Assemble();
   m3->Finalize();
   M3 = m3->ParallelAssemble();
}

void MagneticDiffusionEOperator::buildS1(double muInv)
{
   if ( s1 != NULL ) { delete s1; }

   ConstantCoefficient MuInv(muInv);
   s1 = new ParBilinearForm(&HCurlFESpace);
   s1->AddDomainIntegrator(new CurlCurlIntegrator(MuInv));
   s1->Assemble();
}

void MagneticDiffusionEOperator::buildS2(MeshDependentCoefficient &InvTcap)
{
   if ( s2 != NULL ) { delete s2; }

   // ConstantCoefficient param(a);
   s2 = new ParBilinearForm(&HDivFESpace);
   s2->AddDomainIntegrator(new DivDivIntegrator(InvTcap));
   s2->Assemble();
}

void MagneticDiffusionEOperator::buildCurl(double muInv)
{
   if ( curl != NULL ) { delete curl; }
   if ( weakCurl != NULL ) { delete weakCurl; }

   curl = new ParDiscreteLinearOperator(&HCurlFESpace, &HDivFESpace);
   curl->AddDomainInterpolator(new CurlInterpolator);
   curl->Assemble();

   ConstantCoefficient MuInv(muInv);
   weakCurl = new ParMixedBilinearForm(&HCurlFESpace, &HDivFESpace);
   weakCurl->AddDomainIntegrator(new VectorFECurlIntegrator(MuInv));
   weakCurl->Assemble();

   // no ParallelAssemble since this will be applied to GridFunctions
}

void MagneticDiffusionEOperator::buildDiv(MeshDependentCoefficient &InvTcap)
{
   if ( weakDiv != NULL ) { delete weakDiv; }
   if ( weakDivC != NULL ) { delete weakDivC; }

   weakDivC = new ParMixedBilinearForm(&HDivFESpace, &L2FESpace);
   weakDivC->AddDomainIntegrator(new VectorFEDivergenceIntegrator(InvTcap));
   weakDivC->Assemble();

   weakDiv = new ParMixedBilinearForm(&HDivFESpace, &L2FESpace);
   weakDiv->AddDomainIntegrator(new VectorFEDivergenceIntegrator());
   weakDiv->Assemble();

   // no ParallelAssemble since this will be applied to GridFunctions
}

void MagneticDiffusionEOperator::buildGrad()
{
   if ( grad != NULL ) { delete grad; }

   grad = new ParDiscreteLinearOperator(&HGradFESpace, &HCurlFESpace);
   grad->AddDomainInterpolator(new GradientInterpolator());
   grad->Assemble();

   // no ParallelAssemble since this will be applied to GridFunctions
}

double MagneticDiffusionEOperator::ElectricLosses(ParGridFunction &E_gf) const
{
   double el = m1->InnerProduct(E_gf,E_gf);

   double global_el;
   MPI_Allreduce(&el, &global_el, 1, MPI_DOUBLE, MPI_SUM,
                 m2->ParFESpace()->GetComm());

   return el;
}

// E is the input GF, w is the output GF which is assumed to be an L2 scalar
// representing the Joule heating
void MagneticDiffusionEOperator::GetJouleHeating(ParGridFunction &E_gf,
                                                 ParGridFunction &w_gf) const
{
   // The w_coeff object stashes a reference to sigma and E, and it has
   // an Eval method that will be used by ProjectCoefficient.
   JouleHeatingCoefficient w_coeff(*sigma, E_gf);

   // This applies the definition of the finite element degrees-of-freedom
   // to convert the function to a set of discrete values
   w_gf.ProjectCoefficient(w_coeff);
}

void MagneticDiffusionEOperator::SetTime(const double t_)
{ t = t_; }

MagneticDiffusionEOperator::~MagneticDiffusionEOperator()
{
   if ( ams_a1 != NULL ) { delete ams_a1; }
   if ( pcg_a1 != NULL ) { delete pcg_a1; }

   if ( dsp_m1 != NULL ) { delete dsp_m1; }
   if ( pcg_m1 != NULL ) { delete pcg_m1; }

   if ( dsp_m2 != NULL ) { delete dsp_m2; }
   if ( pcg_m2 != NULL ) { delete pcg_m2; }

   if ( curl != NULL ) { delete curl; }
   if ( weakDiv != NULL ) { delete weakDiv; }
   if ( weakDivC != NULL ) { delete weakDivC; }
   if ( weakCurl != NULL ) { delete weakCurl; }
   if ( grad != NULL ) { delete grad; }

   if ( a0 != NULL ) { delete a0; }
   if ( a1 != NULL ) { delete a1; }
   if ( a2 != NULL ) { delete a2; }
   if ( m1 != NULL ) { delete m1; }
   if ( m2 != NULL ) { delete m2; }
   if ( s1 != NULL ) { delete s1; }
   if ( s2 != NULL ) { delete s2; }

   if ( A0 != NULL ) { delete A0; }
   if ( X0 != NULL ) { delete X0; }
   if ( B0 != NULL ) { delete B0; }

   if ( A1 != NULL ) { delete A1; }
   if ( X1 != NULL ) { delete X1; }
   if ( B1 != NULL ) { delete B1; }

   if ( A2 != NULL ) { delete A2; }
   if ( X2 != NULL ) { delete X2; }
   if ( B2 != NULL ) { delete B2; }

   if ( v1 != NULL ) { delete v1; }
   if ( v2 != NULL ) { delete v2; }

   if (sigma     != NULL) { delete sigma; }
   if (Tcapacity != NULL) { delete Tcapacity; }
   if (InvTcap   != NULL) { delete InvTcap; }
   if (InvTcond  != NULL) { delete InvTcond; }

   delete amg_a0;
   delete pcg_a0;
   delete pcg_a2;
   delete ads_a2;
   delete m3;
   delete dsp_m3;
   delete pcg_m3;
   delete M1;
   delete M2;
   delete M3;
   delete v0;
   delete B3;
}

void MagneticDiffusionEOperator::Debug(const char *base, double)
{
   // {
   //    hypre_ParCSRMatrixPrint(*A1,"A1_");
   //    HypreParVector tempB1(A1->GetComm(),A1->N(),B1->GetData(),A1->ColPart());
   //    tempB1.Print("B1_");
   //    HypreParVector tempX1(A1->GetComm(),A1->N(),X1->GetData(),A1->ColPart());
   //    tempX1.Print("X1_");
   // }

   // {
   //    hypre_ParCSRMatrixPrint(*A2,"A2_");
   //    HypreParVector tempB2(A2->GetComm(),A2->N(),B2->GetData(),A2->ColPart());
   //    tempB2.Print("B2_");
   //    HypreParVector tempX2(A2->GetComm(),A2->N(),X2->GetData(),A2->ColPart());
   //    tempX2.Print("X2_");
   // }
}

double JouleHeatingCoefficient::Eval(ElementTransformation &T,
                                     const IntegrationPoint &ip)
{
   Vector E;
   double thisSigma;
   E_gf.GetVectorValue(T, ip, E);
   thisSigma = sigma.Eval(T, ip);
   return thisSigma*(E*E);
}

MeshDependentCoefficient::MeshDependentCoefficient(
   const std::map<int, double> &inputMap, double scale)
   : Coefficient()
{
   // make a copy of the magic attribute-value map for later use
   materialMap = new std::map<int, double>(inputMap);
   scaleFactor = scale;
}

MeshDependentCoefficient::MeshDependentCoefficient(
   const MeshDependentCoefficient &cloneMe)
   : Coefficient()
{
   // make a copy of the magic attribute-value map for later use
   materialMap = new std::map<int, double>(*(cloneMe.materialMap));
   scaleFactor = cloneMe.scaleFactor;
}

double MeshDependentCoefficient::Eval(ElementTransformation &T,
                                      const IntegrationPoint &ip)
{
   // given the attribute, extract the coefficient value from the map
   std::map<int, double>::iterator it;
   int thisAtt = T.Attribute;
   double value;
   it = materialMap->find(thisAtt);
   if (it != materialMap->end())
   {
      value = it->second;
   }
   else
   {
      value = 0.0; // avoid compile warning
      std::cerr << "MeshDependentCoefficient attribute " << thisAtt
                << " not found" << std::endl;
      mfem_error();
   }

   return value*scaleFactor;
}

ScaledGFCoefficient::ScaledGFCoefficient(GridFunction *gf,
                                         MeshDependentCoefficient &input_mdc)
   : GridFunctionCoefficient(gf), mdc(input_mdc) {}

double ScaledGFCoefficient::Eval(ElementTransformation &T,
                                 const IntegrationPoint &ip)
{
   return mdc.Eval(T,ip) * GridFunctionCoefficient::Eval(T,ip);
}

} // namespace electromagnetics

} // namespace mfem

// #endif // MFEM_USE_MPI

// Copyright (c) 2010-2021, Lawrence Livermore National Security, LLC. Produced
// at the Lawrence Livermore National Laboratory. All Rights reserved. See files
// LICENSE and NOTICE for details. LLNL-CODE-806117.
//
// This file is part of the MFEM library. For more information and source code
// availability visit https://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the BSD-3 license. We welcome feedback and contributions, see file
// CONTRIBUTING.md for details.
//
//            -----------------------------------------------------
//            Joule Miniapp:  Transient Magnetics and Joule Heating
//            -----------------------------------------------------
//
// This miniapp solves a time dependent eddy current problem, resulting in Joule
// heating.
//
// This version has electrostatic potential, Phi, which is a source term in the
// EM diffusion equation. The potential itself is driven by essential BC's
//
//               Div sigma Grad Phi = 0
//               sigma E  =  Curl B/mu - sigma grad Phi
//               dB/dt = - Curl E
//               F = -k Grad T
//               c dT/dt = -Div(F) + sigma E.E,
//
// where B is the magnetic flux, E is the electric field, T is the temperature,
// F is the thermal flux, sigma is electrical conductivity, mu is the magnetic
// permeability, and alpha is the thermal diffusivity.  The geometry of the
// domain is assumed to be as follows:
//
//                              boundary attribute 3
//                            +---------------------+
//               boundary --->|                     | boundary
//               attribute 1  |                     | attribute 2
//               (driven)     +---------------------+
//
// The voltage BC condition is essential BC on attribute 1 (front) and 2 (rear)
// given by function p_bc() at bottom of this file.
//
// The E-field boundary condition specifies the essential BC (n cross E) on
// attribute 1 (front) and 2 (rear) given by function edot_bc at bottom of this
// file. The E-field can be set on attribute 3 also.
//
// The thermal boundary condition for the flux F is the natural BC on attribute 1
// (front) and 2 (rear). This means that dT/dt = 0 on the boundaries, and the
// initial T = 0.
//
// See Section 3 for how the material propertied are assigned to mesh
// attributes, this needs to be changed for different applications.
//
// See Section 5 for how the boundary conditions are assigned to mesh
// attributes, this needs to be changed for different applications.
//
// This code supports a simple version of AMR, all elements containing material
// attribute 1 are (optionally) refined.
//
// Compile with: make joule
//
// Sample runs:
//
//    mpirun -np 8 joule -m cylinder-hex.mesh -p rod
//    mpirun -np 8 joule -m cylinder-tet.mesh -sc 1 -amr 1 -p rod
//    mpirun -np 8 joule -m cylinder-hex-q2.gen -s 22 -dt 0.1 -tf 240.0 -p rod
//
// Options:
//
//    -m [string]   the mesh file name
//    -o [int]      the order of the basis
//    -rs [int]     number of times to serially refine the mesh
//    -rp [int]     number of times to refine the mesh in parallel
//    -s [int]      time integrator 1=Backward Euler, 2=SDIRK2, 3=SDIRK3,
//                  22=Midpoint, 23=SDIRK23, 34=SDIRK34
//    -tf [double]  the final time
//    -dt [double]  time step
//    -mu [double]  the magnetic permeability
//    -cnd [double] the electrical conductivity
//    -f [double]   the frequency of the applied EM BC
//    -vis [int]    GLVis -vis = true -no-vis = false
//    -vs [int]     visualization step
//    -k [string]   base file name for output file
//    -print [int]  print solution (gridfunctions) to disk  0 = no, 1 = yes
//    -amr [int]    0 = no amr, 1 = amr
//    -sc [int]     0 = no static condensation, 1 = use static condensation
//    -p [string]   specify the problem to run, "rod", "coil", or "test"
//
//
// NOTE:  Example meshes for this miniapp are the included cylinder/rod meshes:
//        cylinder-hex.mesh, cylinder-tet.mesh, cylinder-hex-q2.gen,
//        cylinder-tet-q2.gen, as well as the coil.gen mesh which can be
//        downloaded from github.com/mfem/data (its size is 21MB). Note that the
//        meshes with the "gen" extension require MFEM to be built with NetCDF.
//
// NOTE:  This code is set up to solve two example problems, 1) a straight metal
//        rod surrounded by air, 2) a metal rod surrounded by a metal coil all
//        surrounded by air. To specify problem (1) use the command line options
//        "-p rod -m cylinder-hex-q2.gen", to specify problem (2) use the
//        command line options "-p coil -m coil.gen". Problem (1) has two
//        materials and problem (2) has three materials, and the BC's are
//        different.
//
// NOTE:  We write out, optionally, grid functions for P, E, B, W, F, and
//        T. These can be visualized using "glvis -np 4 -m mesh.mesh -g E",
//        assuming we used 4 processors.

#include <memory>
#include <iostream>
#include <fstream>

using namespace std;
using namespace mfem;
using namespace mfem::common;
using namespace mfem::electromagnetics;

void display_banner(ostream & os);

static double mj_ = 0.0;
static double sj_ = 0.0;
static double wj_ = 0.0;

// Initialize variables used in joule_solver.cpp
int electromagnetics::SOLVER_PRINT_LEVEL = 0;
int electromagnetics::STATIC_COND        = 0;



int joule_solve(int argc, char *argv[])
{
   // 1. Initialize MPI.
   // MPI_Session mpi(argc, argv);
   // int myid = mpi.WorldRank();

   int myid;
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);


   // print the cool banner
   if (myid == 0) { display_banner(cout); }

   // 2. Parse command-line options.
   const char *mesh_file = "cylinder-hex.mesh";
   int ser_ref_levels = 0;
   int par_ref_levels = 0;
   int order = 2;
   int ode_solver_type = 1;
   double t_final = 100.0;
   double dt = 0.5;
   double mu = 1.0;
   double sigma = 2.0*M_PI*10;
   double Tcapacity = 1.0;
   double Tconductivity = 0.01;
   double freq = 1.0/60.0;
   bool visualization = true;
   bool visit = true;
   int vis_steps = 1;
   int gfprint = 0;
   const char *basename = "Joule";
   int amr = 0;
   int debug = 0;
   const char *problem = "rod";

   OptionsParser args(argc, argv);
   args.AddOption(&mesh_file, "-m", "--mesh",
                  "Mesh file to use.");
   args.AddOption(&ser_ref_levels, "-rs", "--refine-serial",
                  "Number of times to refine the mesh uniformly in serial.");
   args.AddOption(&par_ref_levels, "-rp", "--refine-parallel",
                  "Number of times to refine the mesh uniformly in parallel.");
   args.AddOption(&order, "-o", "--order",
                  "Order (degree) of the finite elements.");
   args.AddOption(&ode_solver_type, "-s", "--ode-solver",
                  "ODE solver: 1 - Backward Euler, 2 - SDIRK2, 3 - SDIRK3\n\t."
                  "\t   22 - Mid-Point, 23 - SDIRK23, 34 - SDIRK34.");
   args.AddOption(&t_final, "-tf", "--t-final",
                  "Final time; start time is 0.");
   args.AddOption(&dt, "-dt", "--time-step",
                  "Time step.");
   args.AddOption(&mu, "-mu", "--permeability",
                  "Magnetic permeability coefficient.");
   args.AddOption(&sigma, "-cnd", "--sigma",
                  "Conductivity coefficient.");
   args.AddOption(&freq, "-f", "--frequency",
                  "Frequency of oscillation.");
   args.AddOption(&visualization, "-vis", "--visualization", "-no-vis",
                  "--no-visualization",
                  "Enable or disable GLVis visualization.");
   args.AddOption(&visit, "-visit", "--visit", "-no-visit", "--no-visit",
                  "Enable or disable VisIt visualization.");
   args.AddOption(&vis_steps, "-vs", "--visualization-steps",
                  "Visualize every n-th timestep.");
   args.AddOption(&basename, "-k", "--outputfilename",
                  "Name of the visit dump files");
   args.AddOption(&gfprint, "-print", "--print",
                  "Print results (grid functions) to disk.");
   args.AddOption(&amr, "-amr", "--amr",
                  "Enable AMR");
   args.AddOption(&STATIC_COND, "-sc", "--static-condensation",
                  "Enable static condensation");
   args.AddOption(&debug, "-debug", "--debug",
                  "Print matrices and vectors to disk");
   args.AddOption(&SOLVER_PRINT_LEVEL, "-hl", "--hypre-print-level",
                  "Hypre print level");
   args.AddOption(&problem, "-p", "--problem",
                  "Name of problem to run");
   args.Parse();
   if (!args.Good())
   {
      if (myid == 0)
      {
         args.PrintUsage(cout);
      }
      return 1;
   }
   if (myid == 0)
   {
      args.PrintOptions(cout);
   }

   mj_  = mu;
   sj_  = sigma;
   wj_  = 2.0*M_PI*freq;

   if (myid == 0)
   {
      cout << "\nSkin depth sqrt(2.0/(wj*mj*sj)) = " << sqrt(2.0/(wj_*mj_*sj_))
           << "\nSkin depth sqrt(2.0*dt/(mj*sj)) = " << sqrt(2.0*dt/(mj_*sj_))
           << endl;
   }

   // 3. Here material properties are assigned to mesh attributes.  This code is
   //    not general, it is assumed the mesh has 3 regions each with a different
   //    integer attribute: 1, 2 or 3.
   //
   //       The coil problem has three regions: 1) coil, 2) air, 3) the rod.
   //       The rod problem has two regions: 1) rod, 2) air.
   //
   //    We can use the same material maps for both problems.

   std::map<int, double> sigmaMap, InvTcondMap, TcapMap, InvTcapMap;
   double sigmaAir;
   double TcondAir;
   double TcapAir;
   if (strcmp(problem,"rod")==0 || strcmp(problem,"coil")==0)
   {
      sigmaAir     = 1.0e-6 * sigma;
      TcondAir     = 1.0e6  * Tconductivity;
      TcapAir      = 1.0    * Tcapacity;
   }
   else
   {
      cerr << "Problem " << problem << " not recognized\n";
      mfem_error();
   }

   if (strcmp(problem,"rod")==0 || strcmp(problem,"coil")==0)
   {

      sigmaMap.insert(pair<int, double>(1, sigma));
      sigmaMap.insert(pair<int, double>(2, sigmaAir));
      sigmaMap.insert(pair<int, double>(3, sigmaAir));

      InvTcondMap.insert(pair<int, double>(1, 1.0/Tconductivity));
      InvTcondMap.insert(pair<int, double>(2, 1.0/TcondAir));
      InvTcondMap.insert(pair<int, double>(3, 1.0/TcondAir));

      TcapMap.insert(pair<int, double>(1, Tcapacity));
      TcapMap.insert(pair<int, double>(2, TcapAir));
      TcapMap.insert(pair<int, double>(3, TcapAir));

      InvTcapMap.insert(pair<int, double>(1, 1.0/Tcapacity));
      InvTcapMap.insert(pair<int, double>(2, 1.0/TcapAir));
      InvTcapMap.insert(pair<int, double>(3, 1.0/TcapAir));
   }
   else
   {
      cerr << "Problem " << problem << " not recognized\n";
      mfem_error();
   }

   if (myid == 0)
   {
      cout << "\nMaterial properties applied" << endl;
   }
   // 4. Read the serial mesh from the given mesh file on all processors. We can
   //    handle triangular, quadrilateral, tetrahedral and hexahedral meshes
   //    with the same code.
   Mesh *mesh;
   mesh = new Mesh(mesh_file, 1, 1);
   int dim = mesh->Dimension();

   // 5. Assign the boundary conditions
   Array<int> ess_bdr(mesh->bdr_attributes.Max());
   Array<int> thermal_ess_bdr(mesh->bdr_attributes.Max());
   Array<int> poisson_ess_bdr(mesh->bdr_attributes.Max());
   if (strcmp(problem,"coil")==0)
   {
      // BEGIN CODE FOR THE COIL PROBLEM
      // For the coil in a box problem we have surfaces 1) coil end (+),
      // 2) coil end (-), 3) five sides of box, 4) side of box with coil BC

      ess_bdr = 0;
      ess_bdr[0] = 1; // boundary attribute 4 (index 3) is fixed
      ess_bdr[1] = 1; // boundary attribute 4 (index 3) is fixed
      ess_bdr[2] = 1; // boundary attribute 4 (index 3) is fixed
      ess_bdr[3] = 1; // boundary attribute 4 (index 3) is fixed

      // Same as above, but this is for the thermal operator for HDiv
      // formulation the essential BC is the flux

      thermal_ess_bdr = 0;
      thermal_ess_bdr[2] = 1; // boundary attribute 4 (index 3) is fixed

      // Same as above, but this is for the poisson eq for H1 formulation the
      // essential BC is the value of Phi

      poisson_ess_bdr = 0;
      poisson_ess_bdr[0] = 1; // boundary attribute 1 (index 0) is fixed
      poisson_ess_bdr[1] = 1; // boundary attribute 2 (index 1) is fixed
      // END CODE FOR THE COIL PROBLEM
   }
   else if (strcmp(problem,"rod")==0)
   {
      // BEGIN CODE FOR THE STRAIGHT ROD PROBLEM
      // the boundary conditions below are for the straight rod problem

      ess_bdr = 0;
      ess_bdr[0] = 1; // boundary attribute 1 (index 0) is fixed (front)
      ess_bdr[1] = 1; // boundary attribute 2 (index 1) is fixed (rear)
      ess_bdr[2] = 1; // boundary attribute 3 (index 2) is fixed (outer)

      // Same as above, but this is for the thermal operator.  For HDiv
      // formulation the essential BC is the flux, which is zero on the front
      // and sides. Note the Natural BC is T = 0 on the outer surface.

      thermal_ess_bdr = 0;
      thermal_ess_bdr[0] = 1; // boundary attribute 1 (index 0) is fixed (front)
      thermal_ess_bdr[1] = 1; // boundary attribute 2 (index 1) is fixed (rear)

      // Same as above, but this is for the poisson eq for H1 formulation the
      // essential BC is the value of Phi

      poisson_ess_bdr = 0;
      poisson_ess_bdr[0] = 1; // boundary attribute 1 (index 0) is fixed (front)
      poisson_ess_bdr[1] = 1; // boundary attribute 2 (index 1) is fixed (back)
      // END CODE FOR THE STRAIGHT ROD PROBLEM
   }
   else
   {
      cerr << "Problem " << problem << " not recognized\n";
      mfem_error();
   }

   // The following is required for mesh refinement
   mesh->EnsureNCMesh();

   if (myid == 0)
   {
      cout << "\nBoundary conditions applied" << endl;
   }
   // 6. Define the ODE solver used for time integration. Several implicit
   //    methods are available, including singly diagonal implicit Runge-Kutta
   //    (SDIRK).
   ODESolver *ode_solver;
   switch (ode_solver_type)
   {
      // Implicit L-stable methods
      case 1:  ode_solver = new BackwardEulerSolver; break;
      case 2:  ode_solver = new SDIRK23Solver(2); break;
      case 3:  ode_solver = new SDIRK33Solver; break;
      // Implicit A-stable methods (not L-stable)
      case 22: ode_solver = new ImplicitMidpointSolver; break;
      case 23: ode_solver = new SDIRK23Solver; break;
      case 34: ode_solver = new SDIRK34Solver; break;
      default:
         if (myid == 0)
         {
            cout << "Unknown ODE solver type: " << ode_solver_type << '\n';
         }
         delete mesh;
         return 3;
   }
   if (myid == 0)
   {
      cout << "\nODE solver applied" << endl;
   }

   // 7. Refine the mesh in serial to increase the resolution. In this example
   //    we do 'ser_ref_levels' of uniform refinement, where 'ser_ref_levels' is
   //    a command-line parameter.
   for (int lev = 0; lev < ser_ref_levels; lev++)
   {
      mesh->UniformRefinement();
   }
   if (myid == 0)
   {
      cout << "\nMesh refined in serial" << endl;
   }

   // 8. Define a parallel mesh by a partitioning of the serial mesh. Refine
   //    this mesh further in parallel to increase the resolution. Once the
   //    parallel mesh is defined, the serial mesh can be deleted.
   ParMesh *pmesh = new ParMesh(MPI_COMM_WORLD, *mesh);
   delete mesh;
   for (int lev = 0; lev < par_ref_levels; lev++)
   {
      pmesh->UniformRefinement();
   }
   // Make sure tet-only meshes are marked for local refinement.
   pmesh->Finalize(true);
   if (myid == 0)
   {
      cout << "\nParallel mesh defined" << endl;
   }

   // 9. Apply non-uniform non-conforming mesh refinement to the mesh.  The
   //    whole metal region is refined once, before the start of the time loop,
   //    i.e. this is not based on any error estimator.
   if (amr == 1)
   {
      Array<int> ref_list;
      int numElems = pmesh->GetNE();
      for (int ielem = 0; ielem < numElems; ielem++)
      {
         int thisAtt = pmesh->GetAttribute(ielem);
         if (thisAtt == 1)
         {
            ref_list.Append(ielem);
         }
      }

      pmesh->GeneralRefinement(ref_list);

      ref_list.DeleteAll();
   }
   if (myid == 0)
   {
      cout << "\nParallel mesh refined" << endl;
   }
   // 10. Reorient the mesh. Must be done after refinement but before definition
   //     of higher order Nedelec spaces
   pmesh->ReorientTetMesh();
   if (myid == 0)
   {
      cout << "\nMesh reoriented" << endl;
   }

   // 11. Rebalance the mesh. Since the mesh was adaptively refined in a
   //     non-uniform way it will be computationally unbalanced.
   if (pmesh->Nonconforming())
   {
      pmesh->Rebalance();
   }
   if (myid == 0)
   {
      cout << "\nMesh rebalanced" << endl;
   }

   // 12. Define the parallel finite element spaces. We use:
   //
   //     H(curl) for electric field,
   //     H(div) for magnetic flux,
   //     H(div) for thermal flux,
   //     H(grad)/H1 for electrostatic potential,
   //     L2 for temperature

   // L2 contains discontinuous "cell-center" finite elements, type 2 is
   // "positive"
   L2_FECollection L2FEC(order-1, dim);

   // ND contains Nedelec "edge-centered" vector finite elements with continuous
   // tangential component.
   ND_FECollection HCurlFEC(order, dim);

   // RT contains Raviart-Thomas "face-centered" vector finite elements with
   // continuous normal component.
   RT_FECollection HDivFEC(order-1, dim);

   // H1 contains continuous "node-centered" Lagrange finite elements.
   H1_FECollection HGradFEC(order, dim);

   ParFiniteElementSpace    L2FESpace(pmesh, &L2FEC);
   ParFiniteElementSpace HCurlFESpace(pmesh, &HCurlFEC);
   ParFiniteElementSpace  HDivFESpace(pmesh, &HDivFEC);
   ParFiniteElementSpace  HGradFESpace(pmesh, &HGradFEC);

   if (myid == 0)
   {
      cout << "\nFESpaces Initialised" << endl;
   }
   // // The terminology is TrueVSize is the unique (non-redundant) number of dofs
   // HYPRE_BigInt glob_size_l2 = L2FESpace.GlobalTrueVSize();
   // HYPRE_BigInt glob_size_nd = HCurlFESpace.GlobalTrueVSize();
   // HYPRE_BigInt glob_size_rt = HDivFESpace.GlobalTrueVSize();
   // HYPRE_BigInt glob_size_h1 = HGradFESpace.GlobalTrueVSize();

   // if (myid == 0)
   // {
   //    cout << "Number of Temperature Flux unknowns:  " << glob_size_rt << endl;
   //    cout << "Number of Temperature unknowns:       " << glob_size_l2 << endl;
   //    cout << "Number of Electric Field unknowns:    " << glob_size_nd << endl;
   //    cout << "Number of Magnetic Field unknowns:    " << glob_size_rt << endl;
   //    cout << "Number of Electrostatic unknowns:     " << glob_size_h1 << endl;
   // }

   int Vsize_l2 = L2FESpace.GetVSize();
   int Vsize_nd = HCurlFESpace.GetVSize();
   int Vsize_rt = HDivFESpace.GetVSize();
   int Vsize_h1 = HGradFESpace.GetVSize();

   // the big BlockVector stores the fields as
   //    0 Temperature
   //    1 Temperature Flux
   //    2 P field
   //    3 E field
   //    4 B field
   //    5 Joule Heating

   Array<int> true_offset(7);
   true_offset[0] = 0;
   true_offset[1] = true_offset[0] + Vsize_l2;
   true_offset[2] = true_offset[1] + Vsize_rt;
   true_offset[3] = true_offset[2] + Vsize_h1;
   true_offset[4] = true_offset[3] + Vsize_nd;
   true_offset[5] = true_offset[4] + Vsize_rt;
   true_offset[6] = true_offset[5] + Vsize_l2;

   // The BlockVector is a large contiguous chunk of memory for storing required
   // data for the hypre vectors, in this case: the temperature L2, the T-flux
   // HDiv, the E-field HCurl, and the B-field HDiv, and scalar potential P.
   BlockVector F(true_offset);

   // grid functions E, B, T, F, P, and w which is the Joule heating
   ParGridFunction E_gf, B_gf, T_gf, F_gf, w_gf, P_gf;
   T_gf.MakeRef(&L2FESpace,F,   true_offset[0]);
   F_gf.MakeRef(&HDivFESpace,F, true_offset[1]);
   P_gf.MakeRef(&HGradFESpace,F,true_offset[2]);
   E_gf.MakeRef(&HCurlFESpace,F,true_offset[3]);
   B_gf.MakeRef(&HDivFESpace,F, true_offset[4]);
   w_gf.MakeRef(&L2FESpace,F,   true_offset[5]);
   if (myid == 0)
   {
      cout << "\nFESpaces Initialised" << endl;
   }
   // 13. Get the boundary conditions, set up the exact solution grid functions
   //     These VectorCoefficients have an Eval function.  Note that e_exact and
   //     b_exact in this case are exact analytical solutions, taking a 3-vector
   //     point as input and returning a 3-vector field
   VectorFunctionCoefficient E_exact(3, e_exact);
   VectorFunctionCoefficient B_exact(3, b_exact);
   FunctionCoefficient T_exact(t_exact);
   E_exact.SetTime(0.0);
   B_exact.SetTime(0.0);

   if (myid == 0)
   {
      cout << "\nExact Boundary Solutions Initialised" << endl;
   }
   // 14. Initialize the Diffusion operator, the GLVis visualization and print
   //     the initial energies.
   MagneticDiffusionEOperator oper(true_offset[6], L2FESpace, HCurlFESpace,
                                   HDivFESpace, HGradFESpace,
                                   ess_bdr, thermal_ess_bdr, poisson_ess_bdr,
                                   mu, sigmaMap, TcapMap, InvTcapMap,
                                   InvTcondMap);

   if (myid == 0)
   {
      cout << "\nDiffusion operator Initialised" << endl;
   }
   // This function initializes all the fields to zero or some provided IC
   oper.Init(F);

   socketstream vis_T, vis_E, vis_B, vis_w, vis_P;
   char vishost[] = "localhost";
   int  visport   = 19916;
   if (visualization)
   {
      // Make sure all ranks have sent their 'v' solution before initiating
      // another set of GLVis connections (one from each rank):
      MPI_Barrier(pmesh->GetComm());

      vis_T.precision(8);
      vis_E.precision(8);
      vis_B.precision(8);
      vis_P.precision(8);
      vis_w.precision(8);

      int Wx = 0, Wy = 0; // window position
      int Ww = 350, Wh = 350; // window size
      int offx = Ww+10, offy = Wh+45; // window offsets

      VisualizeField(vis_P, vishost, visport,
                     P_gf, "Electric Potential (Phi)", Wx, Wy, Ww, Wh);
      Wx += offx;

      VisualizeField(vis_E, vishost, visport,
                     E_gf, "Electric Field (E)", Wx, Wy, Ww, Wh);
      Wx += offx;

      VisualizeField(vis_B, vishost, visport,
                     B_gf, "Magnetic Field (B)", Wx, Wy, Ww, Wh);
      Wx = 0;
      Wy += offy;

      VisualizeField(vis_w, vishost, visport,
                     w_gf, "Joule Heating", Wx, Wy, Ww, Wh);

      Wx += offx;

      VisualizeField(vis_T, vishost, visport,
                     T_gf, "Temperature", Wx, Wy, Ww, Wh);
   }
   // VisIt visualization
   VisItDataCollection visit_dc(basename, pmesh);
   if ( visit )
   {
      visit_dc.RegisterField("E", &E_gf);
      visit_dc.RegisterField("B", &B_gf);
      visit_dc.RegisterField("T", &T_gf);
      visit_dc.RegisterField("w", &w_gf);
      visit_dc.RegisterField("Phi", &P_gf);
      visit_dc.RegisterField("F", &F_gf);

      visit_dc.SetCycle(0);
      visit_dc.SetTime(0.0);
      visit_dc.Save();
   }

   E_exact.SetTime(0.0);
   B_exact.SetTime(0.0);


   // 15. Perform time-integration (looping over the time iterations, ti, with a
   //     time-step dt). The object oper is the MagneticDiffusionOperator which
   //     has a Mult() method and an ImplicitSolve() method which are used by
   //     the time integrators.
   ode_solver->Init(oper);
   double t = 0.0;

   bool last_step = false;
   for (int ti = 1; !last_step; ti++)
   {
      if (t + dt >= t_final - dt/2)
      {
         last_step = true;
      }

      // F is the vector of dofs, t is the current time, and dt is the time step
      // to advance.
      ode_solver->Step(F, t, dt);

      if (debug == 1)
      {
         oper.Debug(basename,t);
      }

      if (gfprint == 1)
      {
         ostringstream T_name, E_name, B_name, F_name, w_name, P_name, mesh_name;
         T_name << basename << "_" << setfill('0') << setw(6) << t << "_"
                << "T." << setfill('0') << setw(6) << myid;
         E_name << basename << "_" << setfill('0') << setw(6) << t << "_"
                << "E." << setfill('0') << setw(6) << myid;
         B_name << basename << "_" << setfill('0') << setw(6) << t << "_"
                << "B." << setfill('0') << setw(6) << myid;
         F_name << basename << "_" << setfill('0') << setw(6) << t << "_"
                << "F." << setfill('0') << setw(6) << myid;
         w_name << basename << "_" << setfill('0') << setw(6) << t << "_"
                << "w." << setfill('0') << setw(6) << myid;
         P_name << basename << "_" << setfill('0') << setw(6) << t << "_"
                << "P." << setfill('0') << setw(6) << myid;
         mesh_name << basename << "_" << setfill('0') << setw(6) << t << "_"
                   << "mesh." << setfill('0') << setw(6) << myid;

         ofstream mesh_ofs(mesh_name.str().c_str());
         mesh_ofs.precision(8);
         pmesh->Print(mesh_ofs);
         mesh_ofs.close();

         ofstream T_ofs(T_name.str().c_str());
         T_ofs.precision(8);
         T_gf.Save(T_ofs);
         T_ofs.close();

         ofstream E_ofs(E_name.str().c_str());
         E_ofs.precision(8);
         E_gf.Save(E_ofs);
         E_ofs.close();

         ofstream B_ofs(B_name.str().c_str());
         B_ofs.precision(8);
         B_gf.Save(B_ofs);
         B_ofs.close();

         ofstream F_ofs(F_name.str().c_str());
         F_ofs.precision(8);
         F_gf.Save(B_ofs);
         F_ofs.close();

         ofstream P_ofs(P_name.str().c_str());
         P_ofs.precision(8);
         P_gf.Save(P_ofs);
         P_ofs.close();

         ofstream w_ofs(w_name.str().c_str());
         w_ofs.precision(8);
         w_gf.Save(w_ofs);
         w_ofs.close();
      }

      if (last_step || (ti % vis_steps) == 0)
      {
         double el = oper.ElectricLosses(E_gf);

         if (myid == 0)
         {
            cout << fixed;
            cout << "step " << setw(6) << ti << ",\tt = " << setw(6)
                 << setprecision(3) << t
                 << ",\tdot(E, J) = " << setprecision(8) << el << endl;
         }

         // Make sure all ranks have sent their 'v' solution before initiating
         // another set of GLVis connections (one from each rank):
         MPI_Barrier(pmesh->GetComm());

         if (visualization)
         {
            int Wx = 0, Wy = 0; // window position
            int Ww = 350, Wh = 350; // window size
            int offx = Ww+10, offy = Wh+45; // window offsets

            VisualizeField(vis_P, vishost, visport,
                           P_gf, "Electric Potential (Phi)", Wx, Wy, Ww, Wh);
            Wx += offx;

            VisualizeField(vis_E, vishost, visport,
                           E_gf, "Electric Field (E)", Wx, Wy, Ww, Wh);
            Wx += offx;

            VisualizeField(vis_B, vishost, visport,
                           B_gf, "Magnetic Field (B)", Wx, Wy, Ww, Wh);

            Wx = 0;
            Wy += offy;

            VisualizeField(vis_w, vishost, visport,
                           w_gf, "Joule Heating", Wx, Wy, Ww, Wh);

            Wx += offx;

            VisualizeField(vis_T, vishost, visport,
                           T_gf, "Temperature", Wx, Wy, Ww, Wh);
         }

         if (visit)
         {
            visit_dc.SetCycle(ti);
            visit_dc.SetTime(t);
            visit_dc.Save();
         }
      }
   }
   if (visualization)
   {
      vis_T.close();
      vis_E.close();
      vis_B.close();
      vis_w.close();
      vis_P.close();
   }
   if (myid == 0)
   {
      cout << "\nSolved" << endl;
   }

   // 16. Free the used memory.
   delete ode_solver;
   delete pmesh;

   return 0;
}

namespace mfem
{

namespace electromagnetics
{

void edot_bc(const Vector &x, Vector &E)
{
   E = 0.0;
}

void e_exact(const Vector &x, double t, Vector &E)
{
   E[0] = 0.0;
   E[1] = 0.0;
   E[2] = 0.0;
}

void b_exact(const Vector &x, double t, Vector &B)
{
   B[0] = 0.0;
   B[1] = 0.0;
   B[2] = 0.0;
}

double t_exact(const Vector &x)
{
   double T = 0.0;
   return T;
}

double p_bc(const Vector &x, double t)
{
   // the value
   double T;
   if (x[2] < 0.0)
   {
      T = 1.0;
   }
   else
   {
      T = -1.0;
   }

   return T*cos(wj_ * t);
}

} // namespace electromagnetics

} // namespace mfem

void display_banner(ostream & os)
{
   os << "     ____.            .__             " << endl
      << "    |    | ____  __ __|  |   ____     " << endl
      << "    |    |/  _ \\|  |  \\  | _/ __ \\ " << endl
      << "/\\__|    (  <_> )  |  /  |_\\  ___/  " << endl
      << "\\________|\\____/|____/|____/\\___  >" << endl
      << "                                \\/   " << endl
      << flush;
}
