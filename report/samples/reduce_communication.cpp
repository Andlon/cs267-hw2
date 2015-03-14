Rank 0: partition the particles in the domain into subdomains
Rank 0: scatter the particles (including ghost particles) to the appropriate subdomains

outbox = { P particle containers }
inbox = { }

for each timestep:
    compute forces in subdomain
    move particles in subdomain
    for each particle in subdomain:
        if particle in (subdomain i or ghost layer i):
            add to outbox[i]
        if particle in subdomain i:
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
