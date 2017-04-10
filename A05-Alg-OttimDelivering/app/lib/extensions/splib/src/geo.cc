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

// std
#include <cmath>
#include <limits>
#include <iostream>

#include "geo.h"

double gol::distance(double lon1, double lat1, double lon2, double lat2) {
  
  double theta, dist;
  theta = lon1 - lon2;
  dist = sin(rad(lat1)) * sin(rad(lat2)) + cos(rad(lat1)) * cos(rad(lat2)) * cos(rad(theta));
  dist = acos(dist);
  dist = deg(dist);
  dist = dist * 60 * 1.1515;
  dist = dist * 1.609344;
  dist = dist * 1000;

  return dist;

}

double gol::vincenty_distance(double lon1, double lat1, double lon2, double lat2) {

  double a = 6378137.0, b = 6356752.3142,  f = 1.0/298.257223563;  // WGS-84 ellipsiod
  double l = gol::rad(lon2 - lon1);
  double U1 = std::atan((1.0-f) * std::tan(gol::rad(lat1)));
  double U2 = std::atan((1.0-f) * std::tan(gol::rad(lat2)));
  double sin_U1 = std::sin(U1), cos_U1 = std::cos(U1);
  double sin_U2 = std::sin(U2), cos_U2 = std::cos(U2);

  double lambda = l, lambdaP = 2.0 * M_PI;
  double cos_sq_alpha = 0.0, sin_sigma = 0.0, cos_2sigmaM = 0.0,
    cos_sigma = 0.0, sigma = 0.0;
  int iterlimit = 20;
  while (std::abs(lambda - lambdaP) > 1e-12 && --iterlimit>0) {
    double sin_lambda = std::sin(lambda), cos_lambda = std::cos(lambda);
    sin_sigma = std::sqrt((cos_U2*sin_lambda) * (cos_U2*sin_lambda) +
        (cos_U1*sin_U2-sin_U1*cos_U2*cos_lambda) *
        (cos_U1*sin_U2-sin_U1*cos_U2*cos_lambda));
    if (sin_sigma==0) return 0.0;  // co-incident points
    cos_sigma = sin_U1*sin_U2 + cos_U1*cos_U2*cos_lambda;
    sigma = std::atan2(sin_sigma, cos_sigma);
    double sin_alpha = cos_U1 * cos_U2 * sin_lambda / sin_sigma;
    cos_sq_alpha = 1.0 - sin_alpha*sin_alpha;
    cos_2sigmaM = ((cos_sq_alpha == 0.0) ? (0.0) : (cos_sigma - 2.0*sin_U1*sin_U2/cos_sq_alpha));
    double C = f/16.0*cos_sq_alpha*(4.0+f*(4.0-3.0*cos_sq_alpha));
    lambdaP = lambda;
    lambda = l + (1.0-C) * f * sin_alpha *
        (sigma + C*sin_sigma*(cos_2sigmaM+C*cos_sigma*(-1.0+2.0*cos_2sigmaM*cos_2sigmaM)));
  }

  // formula failed to converge
  if (iterlimit == 0) return std::numeric_limits<double>::infinity();

  double u_sq = cos_sq_alpha * (a*a - b*b) / (b*b);
  double A = 1.0 + u_sq/16384.0*(4096.0+u_sq*(-768.0+u_sq*(320.0-175.0*u_sq)));
  double B = u_sq/1024.0 * (256.0+u_sq*(-128.0+u_sq*(74.0-47.0*u_sq)));
  double delta_sigma = B*sin_sigma*(cos_2sigmaM+B/4.0*
      (cos_sigma*(-1.0+2.0*cos_2sigmaM*cos_2sigmaM)-
      B/6.0*cos_2sigmaM*(-3.0+4.0*sin_sigma*sin_sigma)*
      (-3.0+4.0*cos_2sigmaM*cos_2sigmaM)));
  return b*A*(sigma-delta_sigma);

}
