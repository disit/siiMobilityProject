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
