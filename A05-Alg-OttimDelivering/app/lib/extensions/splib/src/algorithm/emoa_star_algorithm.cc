#ifndef GOL_EMOASTAR_ALGORITHM_H_
#define GOL_EMOASTAR_ALGORITHM_H_

namespace gol {

template <typename LabelSet, typename WeightT>
bool emoa_star_algorithm::epsilon_approximation_merge(
     LabelSet& Y, 
     const LabelSet& X, 
     const WeightT& len, 
     double epsilon) 
{  
  bool merged = false;

  // pruning epsilon-dominated components of X 
  LabelSet epsilon_pruned_X;
  typename LabelSet::const_iterator iit = X.begin();
  typename LabelSet::iterator jit = Y.begin();
  while(iit != X.end() && jit != Y.end()) { // bicriterion-ordered sets
    WeightT lbl = *iit + len;
    WeightT eps_lbl = lbl * (1 + epsilon);    
    if (eps_lbl.first < jit->first) { 
      while (jit != Y.end() && eps_lbl.second <= jit->second) {
        typename LabelSet::iterator del = jit++;
      }
      epsilon_pruned_X.insert(lbl);
      ++iit;
    }
    else if (eps_lbl.first > jit->first) {
      while (eps_lbl.second >= jit->second) {
        assert(eps_lbl.first > jit->first);
        if (++iit == X.end()) 
          break;
        lbl = (*iit) + len;
        eps_lbl =  lbl * (1 + epsilon); 
      }
      ++jit;
    }
    // the first components are equal
    else {
      if (eps_lbl.second < jit->second) {
        while (jit != Y.end() && eps_lbl.second <= jit->second) {
          assert(eps_lbl.first <= jit->first); 
          typename LabelSet::iterator del = jit++;
        }
      epsilon_pruned_X.insert(lbl);
      ++iit;
      }
      else if (eps_lbl.second > jit->second) {
        while(eps_lbl.second >= jit->second) {
          if (++iit == X.end()) 
            break;
        lbl = (*iit) + len;
        eps_lbl =  lbl * (1 + epsilon); 
        }
        ++jit;
      }
      // both components are equal
      else {
        ++iit; 
        ++jit;
      }
    }
  } 
  for ( ; iit != X.end(); ++iit) { 
    WeightT lbl = *iit + len;
    epsilon_pruned_X.insert(lbl);
  }

  // merge sets 
  iit = epsilon_pruned_X.begin();
  jit = Y.begin();
  while(iit != epsilon_pruned_X.end() && jit != Y.end()) { // bicriterion-ordered sets
    WeightT lbl = *iit;    
    if (lbl.first < jit->first) { 
      while (jit != Y.end() && lbl.second <= jit->second) {
        typename LabelSet::iterator del = jit++;
        Y.erase(del); // dominated
      }
      Y.insert(lbl);
      merged = true;
      ++iit;
    }
    else if (lbl.first > jit->first) {
      while (lbl.second >= jit->second) {
        assert(lbl.first > jit->first);
        if (++iit == epsilon_pruned_X.end()) 
          break;
        lbl = (*iit); 
      }
      ++jit;
    }
    // the first components are equal
    else {
      if (lbl.second < jit->second) {
        while (jit != Y.end() && lbl.second <= jit->second) {
          assert(lbl.first <= jit->first); 
          typename LabelSet::iterator del = jit++;
          Y.erase(del); // dominated
        }
        Y.insert(lbl);
        merged = true;
        ++iit;
      }
      else if (lbl.second > jit->second) {
        while(lbl.second >= jit->second) {
          if (++iit == epsilon_pruned_X.end()) 
            break;
          lbl = (*iit); 
        }
        ++jit;
      }
      // both components are equal
      else {
        ++iit; 
        ++jit;
      }
    }
  } 
  // when Y is empty, overtaking
  for ( ; iit != epsilon_pruned_X.end(); ++iit) { 
    WeightT lbl = *iit;
    Y.insert(lbl);
    merged = true;
  }

  return merged;
}

template <typename LabelSet>
bool emoa_star_algorithm::target_pruning(
    const LabelSet& target, 
    const LabelSet& X) 
{
  bool merged = false;

  typename LabelSet::const_iterator iit = X.begin();
  typename LabelSet::iterator jit = target.begin();
  while (iit != X.end() && jit != target.end()) { // ordered sets   
    if (iit->first < jit->first) {
      while (jit != target.end() && iit->second <= jit->second)
        jit++;
      merged = true;
      ++iit;
    }
    else if (iit->first > jit->first) {
      while (iit->second >= jit->second) {
        assert(iit->first > jit->first);
        if (++iit == X.end()) 
          break;
      }
      ++jit;
    }
    // the first components are equal
    else {
      if (iit->second < jit->second) {
        while (jit != target.end() && iit->second <= jit->second) {
          assert(iit->first <= jit->first); 
          jit++;
        }
        merged = true;
        ++iit;
      }
      else if (iit->second > jit->second) {
        while (iit->second >= jit->second) {
          if (++iit == X.end()) 
            break;
        }
        ++jit;
      }
      // both components are equal
      else {
        ++iit; 
        ++jit;
      }
    }
  }
  for ( ; iit != X.end(); ++iit) {
    merged = true;
  }
  return !merged;
}

template <typename WeightT, typename LabelSet>
bool emoa_star_algorithm::contains(const WeightT& elem, const LabelSet& s) {
  typename LabelSet::const_iterator cit = s.find(elem);
  if (cit == s.end()) return false;
  else return (cit->second == elem.second);
}  

template <typename BiGraphT, typename VertexDescriptor, typename Heuristic, 
          typename ParetoSet, typename WeightMap, typename Stats>
void emoa_star_algorithm::compute(
    BiGraphT&        g, 
    VertexDescriptor s, 
    VertexDescriptor t, 
    Heuristic        H, 
    ParetoSet&       pareto_set,
    WeightMap&       weight_map,  
    Stats&           stats) 
{
  typedef boost::graph_traits<BiGraphT>      Traits;
  typedef typename Traits::out_edge_iterator out_edge_iterator;
  typedef typename Traits::in_edge_iterator  in_edge_iterator;
  typedef typename Traits::edge_descriptor   edge_descriptor;
  typedef typename WeightMap::value_type     WeightT;

  typedef std::set<WeightT, bicriteria_ord<WeightT> > LabelSet;

  typedef std::pair<unsigned int, WeightT> HeuristicEvaluetion;
  typedef std::set<HeuristicEvaluetion, 
      bicriteria_heuristic_evaluation_ord<WeightT> > HeuristicEvaluetionSet;

  stopwatch chrono;

  // heuristic check
  //assert(H(t) == std::make_pair(0,0));

  unsigned int n = boost::num_vertices(g);
  std::vector<LabelSet> G(n);
  std::vector<unsigned int> open;
  std::vector<bool> is_open(n, false);

  double epsilon = EPLSILON_PARETO; 
  double focus_euclidean_distance = 
    distance(g[s].geo.lon, g[s].geo.lat, g[t].geo.lon, g[t].geo.lat);
  // ellipse periapsis, peripheral distance from focus on the main axis
  double periapsis = ELLIPSE_PERIPHERAL_DISTANCE_PERCENT * (focus_euclidean_distance/100); 
  
  // initialization step
  G[s].insert(std::make_pair(0, 0));
  G[s].insert(WeightT()); // label (0, 0)
  open.push_back(s);
  is_open[s] = true; 
  
  while(!open.empty()) 
  {
    (stats.expansions)++;

    HeuristicEvaluetionSet F; 
    // select a vertex with nd-label in open, 
    // choosing among all the first labels in bags  
    for (unsigned int k = 0; k < open.size(); ++k) {
        F.insert(std::make_pair(k, *(G[open[k]].begin()) + H(open[k])));
    }
    VertexDescriptor i = open[F.begin()->first]; 

    open.erase(open.begin() + F.begin()->first);
    is_open[i] = false;

    out_edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::out_edges(i, g); ei != ei_end; ++ei) 
    { 
      VertexDescriptor j = boost::target(*ei, g); 
      bool changed = epsilon_approximation_merge(G[j], G[i], get(weight_map, *ei), epsilon);      
      
      // if distance source/target over threshold allow pruning
      if (focus_euclidean_distance > ELLIPSE_PRUNING_THRESHOLD) 
      { 
        if (distance(g[s].geo.lon, g[s].geo.lat, g[j].geo.lon, g[j].geo.lat) 
                + distance(g[j].geo.lon, g[j].geo.lat, g[t].geo.lon, g[t].geo.lat) > 
              focus_euclidean_distance + 2*periapsis)
          continue;
      } 
      else 
      { 
        if (distance(g[s].geo.lon, g[s].geo.lat, g[j].geo.lon, g[j].geo.lat) 
                + distance(g[j].geo.lon, g[j].geo.lat, g[t].geo.lon, g[t].geo.lat) > 
              (1.5)*ELLIPSE_PRUNING_THRESHOLD) 
        continue;
      }    
      if (changed && !is_open[j] && j != t) 
      { 
        if (!target_pruning(G[t], G[j] + H(j))) { // TODO cost_pruning?
          open.push_back(j);
          is_open[j] = true;
        }  
      }

    } 
  } // end while 
  chrono.lap();
  stats.run_time = chrono.partial_wall_time(); 
 
  for (auto it = G[t].begin(); it != G[t].end(); ++it) 
  {
    WeightT label = *it;
    // backward recostruction 
    VertexDescriptor v = t;
    std::list<edge_descriptor> path;
    while (v != s) 
    {      
      in_edge_iterator ei, ei_end;
      for (std::tie(ei, ei_end) = boost::in_edges(v, g); ei != ei_end; ++ei) 
      {
        VertexDescriptor u = boost::source(*ei, g);
        if (contains(label - get(weight_map, *ei), G[u])) 
        {
          v = u;          
          label = label - get(weight_map, *ei);
          path.push_front(*ei);
          break;
        }
      }      
    } 
    pareto_set.push_back(std::make_pair(*it, path));  
  } 
#ifdef DEBUG 
  dump(G[t], stats);
#endif
}

template <typename LabelSet, typename Stats>
void emoa_star_algorithm::dump(LabelSet& Gt, Stats& stats) 
{
  logger(logDEBUG) 
    << left("[emoa*]", 14) 
    << left(">", 3);           
  logger(logDEBUG) 
    << left("[emoa*]", 14) 
    << left(">", 3) 
    << center("Node Expansions:", 20) 
    << " | " << stats.expansions;
  logger(logDEBUG) 
    << left("[emoa*]", 14) 
    << left(">", 3) 
    << center("Pareto Set (t):", 20) 
    << " | " 
    << "# " 
    << Gt.size();
  
  for (typename LabelSet::const_iterator it = Gt.begin(); 
        it != Gt.end();  ++it)
    logger(logDEBUG) 
      << left("[emoa*]", 14) 
      << left(">", 3) 
      << center("-", 20) 
      << " | " 
      << right(prd(*it, 0), 40);    
  
  logger(logDEBUG) 
    << left("[emoa*]", 14) 
    << left(">", 3) 
    << std::string(20 + 40 + 2*2, '-');
  logger(logDEBUG) 
    << left("[emoa*]", 14) 
    << left(">", 3) 
    << center(" ", 20) << "  " 
    << right("run-time: " + prd(stats.run_time, 5) + "s", 40);
}  

} // namespace gol

#endif // GOL_EMOASTAR_ALGORITHM_H_

