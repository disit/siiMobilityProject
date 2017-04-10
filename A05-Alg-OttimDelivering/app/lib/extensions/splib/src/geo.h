// This file is part of Sii-Mobility - Algorithms Optimized Delivering.
//
// Copyright (C) 2017 GOL Lab http://webgol.dinfo.unifi.it/ - University of Florence
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with This program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GOL_GEO_H_
#define GOL_GEO_H_

// std
#define _USE_MATH_DEFINES
#include <cmath> 

namespace gol {

static inline double rad(double deg) { return deg*M_PI/180.0; }
static inline double deg(double rad) { return rad*180.0/M_PI; }
double distance(double lon1, double lat1, double lon2, double lat2);
double vincenty_distance(double lon1, double lat1, double lon2, double lat2);

} // namespace gol

#endif // GOL_GEO_H_
