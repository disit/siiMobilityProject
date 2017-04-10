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

#ifndef GOL_UTILS_STOPWATCH_H_
#define GOL_UTILS_STOPWATCH_H_

//std
#include <time.h>
#include <sys/times.h>
#include <iostream>

struct stopwatch {
  clock_t tstart;
  clock_t tprevlap;
  clock_t tlap;
  tms ustart;
  tms uprevlap;
  tms ulap;
  unsigned int clk_tk;

  stopwatch() 
    : tstart(clock()), 
      tprevlap(tstart), 
      tlap(tstart), 
      ustart(),
      uprevlap(),
      ulap(),
      clk_tk(sysconf(_SC_CLK_TCK)) {
    times(&ustart);
    std::copy(&ustart, &ustart + 1, &uprevlap);
    std::copy(&ustart, &ustart + 1, &ulap);
  };
  
  void lap() {
    std::copy(&ulap, &ulap + 1, &uprevlap);
    tprevlap = tlap;
    times(&ulap);
    tlap = clock();
  }

  double partial_wall_time() {
    return ((double)tlap-tstart)/CLOCKS_PER_SEC;
  }

  double lap_wall_time() {
    return ((double)tlap-tprevlap)/CLOCKS_PER_SEC;
  }

  double partial_user_time() {
    return ((double)ulap.tms_utime-ustart.tms_utime)/clk_tk;
  }

  double lap_user_time() {
    return ((double)ulap.tms_utime-uprevlap.tms_utime)/clk_tk;
  }

  double partial_sys_time() {
    return ((double)ulap.tms_stime-ustart.tms_stime)/clk_tk;
  }

  double lap_sys_time() {
    return ((double)ulap.tms_stime-uprevlap.tms_stime)/clk_tk;
  }
  
};

#endif // GOL_UTILS_STOPWATCH_H_
