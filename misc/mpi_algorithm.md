MPI Algorithm description
=========================

Definitions
------------
The *domain* is the physical domain the particles in the simulation reside in. The domain has a physical size, which in this case is two-dimensional and square.

A *subdomain* is a subset of the domain with the following properties:

- Contains a set of particles which physically reside in the subdomain.
- Contains a set of ghost particles, called the *ghost layer*, which reside in neighboring subdomains and are close enough to the subdomain to interact with the particles inside it.
- Has a predetermined physical size.

The algorithm
--------------
Given N particles and n processors, where each processor is assigned a subdomain, the below algorithm describes the task to be executed on a single compute node. Unless otherwise specified, every line is executed by every processor.

    Rank 0: partition the particles in the domain into subdomains
    Rank 0: scatter the particles (including ghost particles) to the appropriate subdomains

    outbox = { subdomain_1, ..., subdomain_i, ..., subdomain_P }
    inbox = { }
    
    for each timestep:
        compute forces in subdomain
        move particles in subdomain
        for each particle in subdomain:
            if particle in (subdomain i or ghost layer i):
                add to outbox[i]
                remove particle

        begin synchronize
            distribute outbox contents
            receive inbox contents
        end synchronize

        for each particle in inbox:
            if particle in subdomain:
                add to subdomain particles
            else:
                add to subdomain ghost layer

        clear outbox and inbox

    Rank 0: gather all particles

MPI Implementation details
-----------------------------
In the above algorithm, computing forces for the local subdomain can be implemented serially or in parallel by means of the algorithms used for the serial and OpenMP/pthreads implementations. 

The algorithm has been designed around the use of MPI_Alltoallv for synchronization in every timestep, which effectively distributes the outbox contents to the appropriate subdomains in exchange for inbox contents from all other processors.