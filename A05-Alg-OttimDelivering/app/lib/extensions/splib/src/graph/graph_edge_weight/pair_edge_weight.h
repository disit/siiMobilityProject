#ifndef GOL_GRAPH_PAIR_EDGE_WEIGHT_H_
#define GOL_GRAPH_PAIR_EDGE_WEIGHT_H_

namespace gol {

// Some Standard Arithmetic Operators 

template <typename WeightFT, typename WeightST>
std::pair<WeightFT, WeightST> operator+(const std::pair<WeightFT, WeightST>& a, const std::pair<WeightFT, WeightST>& b) 
{
  return std::make_pair(a.first+b.first, a.second+b.second);
}

template <typename WeightFT, typename WeightST>
std::pair<WeightFT, WeightST> operator-(const std::pair<WeightFT, WeightST>& a, const std::pair<WeightFT, WeightST>& b) 
{
  return std::make_pair(a.first-b.first, a.second-b.second);
}

template <typename WeightFT, typename WeightST>
std::pair<WeightFT, WeightST> operator*(const std::pair<WeightFT, WeightST>& a, double c) 
{
  return std::make_pair(a.first * c, a.second * c);
}

// Pareto Dominance

template <typename WeightFT, typename WeightST>
struct bicriteria_pareto_dominance {
  bool operator()(const std::pair<WeightFT, WeightST>& a, const std::pair<WeightFT, WeightST>& b) const {
    return (a.first < b.first && a.second <= b.second) ||
      (a.first <= b.first && a.second < b.second);
  }
};

// MultiLabel Sets

template <typename PairWeightT>
struct bicriteria_ord {
  bool operator()(const PairWeightT& a, const PairWeightT& b) const {
    return (a.first < b.first);
  }
};

template <typename WeightT> // LabelSet
std::set<WeightT, bicriteria_ord<WeightT> > 
      operator+(const std::set<WeightT, bicriteria_ord<WeightT> >& G, const WeightT& H) 
{
  typedef std::set<WeightT, bicriteria_ord<WeightT> > LabelSet;
  LabelSet F;
  for(typename LabelSet::const_iterator it = G.begin(); it != G.end(); ++it) 
  {
    F.insert((*it) + H); // TODO: check overflow
  }
  return F;
}

template <typename WeightT> // HeuristicEvaluetion
struct bicriteria_heuristic_evaluation_ord {
  bool operator()(const std::pair<unsigned int, WeightT>& a, const std::pair<unsigned int, WeightT>& b) const {
    return (a.second.first < b.second.first) || // first weight
    (a.second.first == b.second.first && a.second.second < b.second.second) || // second weight
    (a.second.first == b.second.first && a.second.second == b.second.second && a.first < b.first); // open[] id
  }
};

// Weight Dump

template <typename WeightFT, typename WeightST>
std::ostream& operator<<(std::ostream& o, const std::pair<WeightFT, WeightST>& a) {
  o << "(" << a.first << ", " << a.second << ")";
  return o;
}

template <typename WeightT>
std::ostream& operator<<(std::ostream& o, const std::set<WeightT, bicriteria_ord<WeightT> >& a) {
  typedef std::set<WeightT, bicriteria_ord<WeightT> > LabelSet;
  for(typename LabelSet::const_iterator ii = a.begin(); ii != a.end(); ++ii)
    o << *ii << " ";
  return o;
}


} // namespace gol

#endif // GOL_GRAPH_PAIR_EDGE_WEIGHT_H_