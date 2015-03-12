#include <grid.h>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <iterator>

namespace {

int compute_partitions_per_dimension(double size, double minimum_bin_size) {
    assert(size > 0);
    assert(minimum_bin_size > 0);

    minimum_bin_size = std::min(size, minimum_bin_size);

    return static_cast<int> (size / minimum_bin_size);
}

}

grid::grid(double gridsize, double minimum_partition_size)
    : _partitions_per_dim(compute_partitions_per_dimension(gridsize, minimum_partition_size)),
      _particle_count(0),
      _size(gridsize)
{
    _partition_size = _size / _partitions_per_dim;
}

grid::grid(size_t particle_count, double gridsize, double minimum_partition_size)
    : _partitions_per_dim(compute_partitions_per_dimension(gridsize, minimum_partition_size)),
      _particle_count(particle_count),
      _size(gridsize)
{
    _partition_size = _size / _partitions_per_dim;
}

size_t grid::partition_count() const
{
    return partitions_per_dim() * partitions_per_dim();
}

size_t grid::partitions_per_dim() const
{
    return _partitions_per_dim;
}

double grid::partition_size() const
{
    return _partition_size;
}

double grid::size() const
{
    return _size;
}

size_t grid::particle_count() const
{
    return _particle_count;
}

size_t determine_partition_for_particle(const grid &grid, const particle_t &t)
{
    assert(t.x >= 0);
    assert(t.x <= grid.size());
    assert(t.y >= 0);
    assert(t.y <= grid.size());

    // Place particle in bin determined by x, y coordinates
    int x = static_cast<int> (t.x / grid.partition_size());
    int y = static_cast<int> (t.y / grid.partition_size());

    return static_cast<size_t>(x + y * grid.partitions_per_dim());
}
