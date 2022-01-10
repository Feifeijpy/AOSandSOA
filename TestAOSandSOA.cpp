//#include <windows.h>
#include<chrono>
#include <iostream>
#include <random>

#include "Array3D_T.h"

using namespace std;

constexpr int Nx = 200, Ny = 200, Nz = 200;
//constexpr int repeatN = 1;
constexpr int widthN = 2;

struct Sxyz
{
    double x;
    double y;
    double z;
};

struct SxyzA
{
    Array3D<double> x, y, z;
};


int main()
{
    // Random
    double lower_bound = 0;
    double upper_bound = 100000;
    std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
    std::default_random_engine re;
    // Time
    using double_ms = chrono::duration<double, milli>;

    // SOA
    SxyzA SOA;
    SOA.x = Array3D<double>(Nx, Ny, Nz);
    SOA.y = Array3D<double>(Nx, Ny, Nz);
    SOA.z = Array3D<double>(Nx, Ny, Nz);
    for (int ix = 0; ix < Nx; ix++) {
        for (int iy = 0; iy < Ny; iy++) {
            for (int iz = 0; iz < Nz; iz++) {
                SOA.x[ix][iy][iz] = unif(re);
                SOA.y[ix][iy][iz] = unif(re);
                SOA.z[ix][iy][iz] = 0.;
            }
        }
    }

    auto t1 = chrono::steady_clock::now();
    //for (int ire = 0; ire < repeatN; ire++) {
        for (int ix = widthN; ix < Nx - widthN; ix++) {
            for (int iy = widthN; iy < Ny - widthN; iy++) {
                for (int iz = widthN; iz < Nz - widthN; iz++) {
                    double temp = 0.;
                    for (int ixwd = -widthN; ixwd <= widthN; ixwd++) {
                        for (int iywd = -widthN; iywd <= widthN; iywd++) {
                            for (int izwd = -widthN; izwd <= widthN; izwd++) {
                                temp += SOA.x[ix + ixwd][iy + iywd][iz + izwd] * SOA.y[ix + ixwd][iy + iywd][iz + izwd];
                            }
                        }
                    }
                    SOA.z[ix][iy][iz] += temp;
                }
            }
        }
    //}
    auto t2 = chrono::steady_clock::now();
    auto dt1 = t2 - t1;
    double time1 = chrono::duration_cast<double_ms>(dt1).count();
    cout << "SOA time = " << time1 << endl;


    // AOS
    auto AOS = Array3D<Sxyz>(Nx, Ny, Nz);
    for (int ix = 0; ix < Nx; ix++) {
        for (int iy = 0; iy < Ny; iy++) {
            for (int iz = 0; iz < Nz; iz++) {
                AOS[ix][iy][iz] = { unif(re) ,unif(re) ,0. };
            }
        }
    }

    t1 = chrono::steady_clock::now();
    //for (int ire = 0; ire < repeatN; ire++) {
        for (int ix = widthN; ix < Nx - widthN; ix++) {
            for (int iy = widthN; iy < Ny - widthN; iy++) {
                for (int iz = widthN; iz < Nz - widthN; iz++) {
                    double temp = 0.;
                    for (int ixwd = -widthN; ixwd <= widthN; ixwd++) {
                        for (int iywd = -widthN; iywd <= widthN; iywd++) {
                            for (int izwd = -widthN; izwd <= widthN; izwd++) {
                                temp += AOS[ix + ixwd][iy + iywd][iz + izwd].x * AOS[ix + ixwd][iy + iywd][iz + izwd].y;
                            }
                        }
                    }
                    AOS[ix][iy][iz].z += temp;
                }
            }
        }
    //}

    t2 = chrono::steady_clock::now();
    auto dt2 = t2 - t1;
    double time2 = chrono::duration_cast<double_ms>(dt2).count();
    cout << "AOS time = " << time2 << endl;

    return 0;
}

// Intel C++ 2022
// -O3
// std=c++20
//
// 
//SOA time = 406.008
//AOS time = 423.098