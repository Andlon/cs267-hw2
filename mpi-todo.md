MPI TODO
========

The following describes an incremental approach to build (hopefully efficient) MPI code. Measure differences between each step.

- Naive O(n): Modify provided O(N^2) code to run in O(n) time, but let all communication code remain, and see how much of an improvement it gives.
- Keep the same MPI code from earlier, but parallellize the local computations with OpenMP. Try to run 4 MPI nodes on one compute node, and 6 OpenMP threads per MPI node, as described in https://www.nersc.gov/users/computational-systems/hopper/running-jobs/using-openmp-with-mpi/
- Domain decomposition: perform a coarse partition of the domain and give each processor a partition. Then perform fine partitioning locally and update. Then gather particles and repeat. Start with initial partition on rank 0 and see how it goes. Each partition goes to an MPI node, and then 6 OpenMP threads solve the fine local problem on each node.
