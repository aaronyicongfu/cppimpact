#include <cblas.h>

#include <chrono>
#include <string>

#include "include/analysis.h"
#include "include/dynamics.h"
#include "include/elastoplastic.h"
#include "include/mesh.h"
#include "include/physics.h"
#include "include/tetrahedral.h"
#include "include/wall.h"

int main(int argc, char *argv[]) {
  using T = double;
  using Basis = TetrahedralBasis<T>;
  using Quadrature = TetrahedralQuadrature;
  using Physics = NeohookeanPhysics<T>;
  using Analysis = FEAnalysis<T, Basis, Quadrature, Physics>;

  const int dof_per_node = 3;

  std::vector<std::string> node_set_names;
  // Load in the mesh
  std::string filename("../input/Dynamics Cube Coarse.inp");
  Mesh<T> tensile;

  // Material Properties
  T E = 200E6;   // Pa
  T rho = 7800;  // kg/m3
  T nu = 0.25;
  T beta = 0.0;
  T H = 10;
  T Y0 = 1.9 * std::sqrt(3.0);
  std::string name = "Steel";

  Elastoplastic<T, dof_per_node> material(E, rho, nu, beta, H, Y0, name);
  tensile.load_mesh(filename);

  // Set the number of degrees of freedom

  // Position and velocity in x, y, z
  T init_position[] = {0.1, 0.1, 0.2};
  T init_velocity[] = {0, 0.0, -0.5};

  const int normal = 1;
  std::string wall_name = "Wall";
  T location = 0.0999;
  double dt = 0.00001;
  double time_end = 0.168;

  Wall<T, 2, Basis> w(wall_name, location, E, tensile.slave_nodes,
                      tensile.num_slave_nodes, normal);

  Dynamics<T, Basis, Analysis> dyna(&tensile, &material, &w);
  dyna.initialize(init_position, init_velocity);

  // Solve loop with total timer
  auto start = std::chrono::high_resolution_clock::now();
  dyna.solve(dt, time_end);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

  return 0;
}