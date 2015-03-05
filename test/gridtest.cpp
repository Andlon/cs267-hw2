#include <catch.hpp>
#include <grid.h>
#include <algorithm>

TEST_CASE("grid class members", "[grid]") {

    SECTION("construction") {
        grid grid(3.2, 1.0);

        SECTION("size is correct") {
            REQUIRE(grid.size() == 3.2);
        }

        SECTION("basic bin division") {
            REQUIRE(grid.bins_per_dim() == 3);
            REQUIRE(grid.binsize() == Approx(3.2 / 3));
        }

        SECTION("bin count") {
            REQUIRE(grid.bincount() == 9);
        }
    }

    SECTION("distribution") {
        grid grid(1.0, 0.45);

        // Place one particle in each quadrant
        particle_t particles[] = {
            { 0.1, 0.1 },
            { 0.8, 0.1 },
            { 0.1, 0.8 },
            { 0.8, 0.8 }
        };

        grid.distribute_particles(particles, 4);
        REQUIRE(grid.bincount() == 4);

        // Check that the particles reside in each their own bin
        REQUIRE(grid[0].particles.size() == 1);
        REQUIRE(grid[0].particles.front() == particles);

        REQUIRE(grid[1].particles.size() == 1);
        REQUIRE(grid[1].particles.front() == particles + 1);

        REQUIRE(grid[2].particles.size() == 1);
        REQUIRE(grid[2].particles.front() == particles + 2);

        REQUIRE(grid[3].particles.size() == 1);
        REQUIRE(grid[3].particles.front() == particles + 3);
    }
}

TEST_CASE("associated algorithms") {
    SECTION("acquire_interacting_bins: 2x2") {
        grid grid(1.0, 0.45);

        auto bins0 = acquire_interacting_bins(grid, 0);
        auto bins1 = acquire_interacting_bins(grid, 1);
        auto bins2 = acquire_interacting_bins(grid, 2);
        auto bins3 = acquire_interacting_bins(grid, 3);

        auto expected = std::vector<size_t> { { 0, 1, 2, 3 } };

        REQUIRE(bins0 == expected);
        REQUIRE(bins1 == expected);
        REQUIRE(bins2 == expected);
        REQUIRE(bins3 == expected);
    }
}
