
#include <boost/filesystem.hpp>

namespace gol { namespace osm {

/// JSON Format

template <typename BuilderT>
void json_director<BuilderT>::construct_model() {

  uint32_t n_routes = 0, n_trips = 0, n_stimes = 0, n_stops = 0;  
  std::map<stop_Rt, std::list<route_Rt> > sr_map;// map[stop] = list{route,..}
  std::map<route_Rt, std::list<stop_Rt> > rs_map; // map[route] = list{stop,..}

  logger(logINFO) << "[json-reader] " << left(">", 3) << boost::filesystem::path(_filename);

  Jzon::Object rootn;
  Jzon::FileReader::ReadFile(_filename.c_str(), rootn);

  for (Jzon::Object::iterator it = rootn.begin(); it != rootn.end(); ++it) {
      
    //std::string name = (*it).first;
    Jzon::Node &node = (*it).second;

    //std::cout << name << " = ";
    if (node.IsValue()) {
      if (node.AsValue().GetValueType() == Jzon::Value::VT_STRING) {
        //std::cout << node.ToString()<<std::endl;
      }
    }

  }
  Jzon::Node &date = rootn.Get("date");
  if (date.IsValue()) {
    if (date.AsValue().GetValueType() == Jzon::Value::VT_STRING) {
      //std::cout << date.ToString()<<std::endl;
    }
  }
  // LINES
  const Jzon::Array &lines = rootn.Get("line").AsArray();
  for (Jzon::Array::const_iterator lineit = lines.begin(); lineit != lines.end(); ++lineit) {
    
    Jzon::Object line = (*lineit);

    Jzon::Node &line_company = line.Get("company");
    if (line_company.IsValue()) {
      if (line_company.AsValue().GetValueType() == Jzon::Value::VT_STRING) {
        //std::cout << line_company.ToString()<<std::endl;
      }
    }
    Jzon::Node &line_name = line.Get("name");
    if (line_name.IsValue()) {
      if (line_name.AsValue().GetValueType() == Jzon::Value::VT_STRING) {
        //std::cout << line_name.ToString()<<std::endl;
      }
    }
    Jzon::Node &line_direction = line.Get("direction");
    if (line_direction.IsValue()) {
      if (line_direction.AsValue().GetValueType() == Jzon::Value::VT_STRING) {
        //std::cout << line_direction.ToString()<<std::endl;
      }
    }
    // RIDES
    const Jzon::Array &rides = line.Get("ride").AsArray();
    for (Jzon::Array::const_iterator rideit = rides.begin(); rideit != rides.end(); ++rideit) {

      Jzon::Object ride = (*rideit);

      Jzon::Node &ride_number = ride.Get("number");
      if (ride_number.IsValue()) {
        if (ride_number.AsValue().GetValueType() == Jzon::Value::VT_NUMBER) {
          //std::cout << ride_number.ToString()<<std::endl;
        }
      }
      // STOPS
      const Jzon::Array &stops = ride.Get("stop").AsArray();
      for (Jzon::Array::const_iterator stopit = stops.begin(); stopit != stops.end(); ++stopit) {
          //std::cout<<"n stop "<<stops.GetCount()<<std::endl;

        Jzon::Object stop = (*stopit);

        Jzon::Node &stop_name = stop.Get("name");
        if (stop_name.IsValue()) {
          if (stop_name.AsValue().GetValueType() == Jzon::Value::VT_STRING) {
            //std::cout << stop_name.ToString()<<std::endl;
          }
        }
        Jzon::Node &stop_time = stop.Get("time");
        if (stop_time.IsValue()) {
          if (stop_time.AsValue().GetValueType() == Jzon::Value::VT_STRING) {
            //std::cout << stop_time.ToString()<<std::endl;
          }
        }

        Jzon::Object location = stop.Get("smartyLocation");

        Jzon::Node &location_id = location.Get("locationId");
        if (location_id.IsValue()) {
          if (location_id.AsValue().GetValueType() == Jzon::Value::VT_STRING) {
            //std::cout << location_id.ToString()<<std::endl;
          }
        }
        Jzon::Node &lat = location.Get("lat");
        if (lat.IsValue()) {
          if (lat.AsValue().GetValueType() == Jzon::Value::VT_NUMBER) {
            //std::cout << lat.ToDouble()<<std::endl;
          }
        }
        Jzon::Node &lon = location.Get("lon");
        if (lon.IsValue()) {
          if (lon.AsValue().GetValueType() == Jzon::Value::VT_NUMBER) {
            //std::cout << lon.ToDouble()<<std::endl;              
          }
        }

        if (ride_number.ToInt() == 0){
          (*_builder).add_stop(/*location_id.ToString()*/to_osm_nd(location_id.ToString()), lon.ToDouble(), lat.ToDouble(), line_direction.ToString());
          n_stops++;
        }

        (*_builder).add_stop_time(line_direction.ToString() , 
           (line_direction.ToString() +" | "+ ride_number.ToString()), /*location_id.ToString()*/
              to_osm_nd(location_id.ToString()), to_rtime(stop_time.ToString(), date.ToString()), to_rtime(stop_time.ToString(), date.ToString()));

        n_stimes++;

      } // stops

      n_trips++;
        
    } // rides

    n_routes++;

  } // lines

  // genrate raptor structures
  _builder->rtimetable();

}

}  // namespace osm
}  // namespace gol
