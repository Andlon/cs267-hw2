#include <catch.hpp>
#include <grid.h>
#include <spatial_partition.h>
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

TEST_CASE("spatial_partition") {
    SECTION("partition") {
        grid grid(1.0, 0.45);
        partitioned_storage storage(grid);

        std::vector<particle_t> particles = {
            { 0.1, 0.1 },
            { 0.8, 0.1 },
            { 0.1, 0.8 },
            { 0.8, 0.8 }
        };

        storage.particles = particles;
        partition(storage, grid);

        REQUIRE(storage.particles.size() == 4);

        REQUIRE(storage.particles[0].x == Approx(0.1));
        REQUIRE(storage.particles[0].y == Approx(0.1));
        REQUIRE(storage.particles[1].x == Approx(0.8));
        REQUIRE(storage.particles[1].y == Approx(0.1));
        REQUIRE(storage.particles[2].x == Approx(0.1));
        REQUIRE(storage.particles[2].y == Approx(0.8));
        REQUIRE(storage.particles[3].x == Approx(0.8));
        REQUIRE(storage.particles[3].y == Approx(0.8));

        REQUIRE(storage.partitions[0] == 1);
        REQUIRE(storage.partitions[1] == 2);
        REQUIRE(storage.partitions[2] == 3);
        REQUIRE(storage.partitions[3] == 4);
    }
}
