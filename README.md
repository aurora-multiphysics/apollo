# Apollo
Apollo is a MOOSE-based application created for the scalable solution of finite element electromagnetics problems using the MFEM-based library Hephaestus, as part of the Aurora multiphysics package.

Apollo provides a MOOSE interface to the Hephaestus electromagnetic solvers and formulations, and allows MFEM based finite element problems to be customised and launched from the MOOSE user interface. This enables access to electromagnetic formulations relying on use of finite elements that are conformal to underlying electromagnetic fields, that are well supported in MFEM, but which have less support in MOOSE's native libMesh finite element library.

Apollo facilitates coupling between the extensive range of existing MOOSE physics modules and derived electromagnetic variables such as heat sources from Ohmic heating, allowing the simulation of systems such as induction heaters.

Apollo is still under active development and is being updated frequently.

# Getting Started
Docker images of Apollo for Ubuntu with all dependencies are built weekly and uploaded to DockerHub, and
can be downloaded via
```
docker pull alexanderianblair/apollo:master
```
Once downloaded, the image can be run in interactive mode with the command
```
docker run -it alexanderianblair/apollo:master
```
Additional information and options for using Docker can be found at this [tutorial](https://docs.docker.com/get-started/) on the Docker website.

Alternatively, up-to-date images of only the current dependencies for Apollo can be downloaded from
```
docker pull alexanderianblair/apollo-deps:master
```
for those who wish to build Apollo themselves.

Dockerfiles used to build these images can be found in the `apollo/docker` directory.

# Building Software and Tests
After downloading Apollo from this repository, first update the Hephaestus git submodule with
```
git submodule update --init --recursive
```
This step is not needed if using the supplied alexanderianblair/apollo container. After updating, build the Hephaestus library with
```
cd /opt/apollo/contrib/hephaestus/
mkdir build
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DMFEM_DIR=/opt/mfem/build ..
ninja
```

Apollo can then be built with the following commands from the top level `apollo` directory in either of the above containers:
```
make -j4 
make test -j4
```

Running `make test` after Apollo is built will run the entire set of tests found in `apollo/test`. Running a specific test (or tests) is possible by using the `apollo/run_tests` python script and using the `--re` command line argument to pass in a regular expression satisfied by the names of the tests you wish to run; for example:
```
/opt/apollo/run_tests --re=CoupledFONodalMFEMVar
```

## Electromagnetic Models
Apollo currently provides access to a range of electromagnetic formulations in MOOSE, for use in different regimes:

- Definite curl-curl formulations in the time domain for solving low frequency problems using H(Curl) conforming FEs - AFormulation, HFormulation, EFormulation.
- Mixed formulations in the time domain using H(Curl) and H1 conforming FEs for the representation of vector and scalar potentials respectively - AVFormulation. 
- Indefinite complex Maxwell problem in the frequency domain using H(Curl) conforming FEs (currently requires use of direct solvers, limiting scalability) - ComplexEFormulation and ComplexAFormulation.

Custom MFEM-based formulations and weak forms can also be developed based on CustomFormulation from Apollo.

More information on the available formulations can be found at https://aurora-multiphysics.github.io/apollo/
