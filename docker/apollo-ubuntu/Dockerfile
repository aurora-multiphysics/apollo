# To make use of multiple cores during the compile stages of the docker build
# docker build -t apollo-ubuntu --build-arg compile_cores=8 .

# To compile a particular git sha use
# docker build -t apollo-ubuntu --build-arg build_git_sha=${GITHUB_SHA} .

# Get MOOSE image
FROM helenbrooks/moose-ubuntu

# By default one core is used to compile
ARG compile_cores=1

# By default checkout main branch
ARG build_git_sha="master"

RUN cd /home && \
cd moose/framework/src/base && \
sed -i 's/(type.family == NEDELEC_ONE)/(type.family == NEDELEC_ONE||type.family == LAGRANGE_VEC)/g' Assembly.C

RUN cd /home && \
    git clone https://github.com/aurora-multiphysics/apollo.git && \
    cd apollo && \
    git checkout "$build_git_sha" && \
    make -j"$compile_cores" && \
    make -j"$compile_cores" test &&\
    ./run_tests