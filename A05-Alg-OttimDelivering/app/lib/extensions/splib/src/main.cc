
#include "route_planner.h"

int main(int argc, char* argv[]) {

 if ((argc - 1) == 6) {
  gol::route_planner* rp = new gol::route_planner(argv[5]);	 	
  rp->route_optimization(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
 } //*/

  if ((argc - 1) == 7) {
  gol::route_planner* rp = new gol::route_planner(argv[5]);	
  rp->route_optimization(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]); // argv[7] out_filename 
  rp->route_optimization(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]); 
  //rp->route_optimization("foot_optimization", argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
  //rp->update_OSMdb(argv[5]);
  //rp->route_optimization(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);    
 }

  if ((argc - 1) == 1) {
  //gol::route_planner* rp = new gol::route_planner();  	
  //rp->update_OSMdb(argv[1]);
 } 

 return 0;

}
