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

#include "sqlite_database_helper.h"

namespace gol {

sqlite_database_helper_t::sqlite_database_helper_t(
  std::string db_name) :
    _db(db_name.c_str()),
    _cached_nds(),
    _cached_wys(),
    _cached_rls()
{
  try {
    create_schema();
  } catch (std::exception& e) {
    throw runtime_exception(e.what());
  }
}

void sqlite_database_helper_t::create_schema()
{
  _db.execute(
    "CREATE TABLE IF NOT EXISTS node( "
    "  id    INTEGER PRIMARY KEY,     "
    "  lat   REAL NOT NULL,           "
    "  lon   REAL NOT NULL,           "
    "  ele   REAL NULL,               "
    "  ntags INTEGER NOT NULL)");

  _db.execute(
    "CREATE TABLE IF NOT EXISTS way( "
    "  id    INTEGER PRIMARY KEY,    "
    "  nrefs INTEGER NOT NULL,       "
    "  ntags INTEGER NOT NULL)");

  _db.execute(
    "CREATE TABLE IF NOT EXISTS relation( "
    "  id    INTEGER PRIMARY KEY,         "
    "  nrefs INTEGER NOT NULL,            "
    "  ntags INTEGER NOT NULL)");

  _db.execute(
    "CREATE TABLE IF NOT EXISTS node_in_way(  "
    "  way_id  INTEGER NOT NULL               "
    "          REFERENCES way                 "
    "          DEFERRABLE INITIALLY DEFERRED, "
    "  node_id INTEGER NOT NULL               "
    "          REFERENCES node                "
    "          DEFERRABLE INITIALLY DEFERRED, "
    "  order_  INTEGER NOT NULL)");

  _db.execute(
    "CREATE TABLE IF NOT EXISTS member_in_relation(  "
    "  id_of_relation INTEGER NOT NULL               "
    "                 REFERENCES relation            "
    "                 DEFERRABLE INITIALLY DEFERRED, "
    "  node_id        INTEGER NULL                   "
    "                 REFERENCES node                "
    "                 DEFERRABLE INITIALLY DEFERRED, "
    "  way_id         INTEGER NULL                   "
    "                 REFERENCES way                 "
    "                 DEFERRABLE INITIALLY DEFERRED, "
    "  relation_id    INTEGER NULL                   "
    "                 REFERENCES relation            "
    "                 DEFERRABLE INITIALLY DEFERRED, "
    "  role           TEXT)");

  _db.execute(
    "CREATE TABLE IF NOT EXISTS tag(              "
    "  node_id     INTEGER NULL                   "
    "              REFERENCES node                "
    "              DEFERRABLE INITIALLY DEFERRED, "
    "  way_id      INTEGER NULL                   "
    "              REFERENCES way                 "
    "              DEFERRABLE INITIALLY DEFERRED, "
    "  relation_id INTEGER NULL                   "
    "              REFERENCES relation            "
    "              DEFERRABLE INITIALLY DEFERRED, "
    "  k           TEXT NOT NULL,                 "
    "  v           TEXT NOT NULL)");

  // INDEXES

  _db.execute(
    "CREATE INDEX node_in_way__way_id  "
    "  ON node_in_way (way_id)");

  _db.execute(
    "CREATE INDEX node_in_way__node_id  "
    "  ON node_in_way (way_id)");

  _db.execute(
    "CREATE INDEX tag__v "
    "  ON tag (v)");

  _db.execute(
    "CREATE INDEX tag__k_v  "
    "  ON tag (k, v)");

  _db.execute(
    "CREATE INDEX tag__node_id  "
    "  ON tag (node_id)");

  _db.execute(
    "CREATE INDEX tag__way_id  "
    "  ON tag (way_id)");

  _db.execute(
    "CREATE INDEX tag__relation_id  "
    "  ON tag (relation_id)");

  _db.execute(
    "CREATE INDEX member_in_relation__node_id  "
    "  ON member_in_relation (node_id)");

  _db.execute(
    "CREATE INDEX member_in_relation__way_id  "
    "  ON member_in_relation (way_id)");

  _db.execute(
    "CREATE INDEX member_in_relation__relation_id  "
    "  ON member_in_relation (relation_id)");

  // VIEWS

  _db.execute(
    "CREATE VIEW highway AS                     "
    " SELECT way.*                              "
    "  FROM way                                 "
    "   INNER JOIN tag                          "
    "    ON way.id = way_id                     "
    "  WHERE tag.k LIKE '%highway%'             "
    "          AND tag.k NOT LIKE 'abandoned:%' " // Relate to features that have fallen into serious disrepair
    "             AND  tag.v != 'construction'  " // For roads under construction
    "             AND  tag.v != 'proposed'      " // For planned roads
    "             AND  tag.v != 'abandoned'     " // Relate to features that have fallen into serious disrepair
    "             AND  tag.v != 'services'      " // Are places along a road
    "             AND  tag.v != 'disused'       " // Currently unused
    "             AND  tag.v != 'collapsed'     " // Damaged roads
    "             AND  tag.v != 'bus_guideway'  " // A busway is not suitable for other traffic
    " UNION                                     "
    " SELECT way.*                              "
    "  FROM member_in_relation                  "
    "   INNER JOIN way                          "
    "    ON way.id = way_id                     "
    "  WHERE role LIKE 'outer'                  ");

  _db.execute(
    "CREATE VIEW node_as_via_restriction AS            "
    " SELECT                                           "
    "   memV.id_of_relation AS id,                     "
    "   memV.node_id        AS via,                    "
    "   memF.role           AS role_from,              "
    "   memF.way_id         AS way_from,               "
    "   memT.role           AS role_to,                "
    "   memT.way_id         AS way_to,                 "
    "   tag.v               AS restriction             "
    " FROM member_in_relation AS memV                  "
    "  INNER JOIN  member_in_relation AS memF          "
    "    ON memV.id_of_relation = memF.id_of_relation  "
    "  INNER JOIN  member_in_relation AS memT          "
    "    ON memV.id_of_relation = memT.id_of_relation  "
    "  INNER JOIN tag                                  "
    "    ON memV.id_of_relation = tag.relation_id      "
    " WHERE memV.node_id IS NOT NULL                   "
    "   AND memV.role LIKE 'via'                       "
    "   AND memF.role LIKE 'from'                      "
    "   AND memT.role LIKE 'to'                        "
    "   AND tag.k LIKE 'restriction%'                  ");

}

bool sqlite_database_helper_t::empty()
{
  try {
    sqlite3xx::query qry(_db,
    "SELECT COUNT(*) FROM node LIMIT 10");
    for (auto q : qry) {
      int n; std::tie(n) = q.get_columns<int>(0);
      if (n == 0)
        return true;
    }
    return false;
  } catch (std::exception& e) {
    return true; // TODO throw exception
  }
}

void sqlite_database_helper_t::reset()
{
  _db.execute("DELETE FROM node");
  _db.execute("DELETE FROM way");
  _db.execute("DELETE FROM relation");
  _db.execute("DELETE FROM node_in_way");
  _db.execute("DELETE FROM member_in_relation");
  _db.execute("DELETE FROM tag");
}

void sqlite_database_helper_t::clear_cache()
{
    _cached_nds.clear();
    _cached_wys.clear();
    _cached_rls.clear();
}

void sqlite_database_helper_t::commit()
{
  try
  {
    insert_cached_nodes();
    insert_cached_ways();
    insert_cached_relations();

    clear_cache();
  }
  catch (std::exception& e) {
    throw runtime_exception(e.what());
  }
}

void sqlite_database_helper_t::insert_cached_nodes()
{
  sqlite3xx::transaction xct(_db);
  try {
    for (auto n : _cached_nds)
    {
      sqlite3xx::command cmd(
         _db, "INSERT INTO node (id, lat, lon, ele, ntags) "
              "  VALUES (:id, :lat, :lon, :ele, :ntags)");
      cmd.bind(":id",    n.id);
      cmd.bind(":lat",   n.lat);
      cmd.bind(":lon",   n.lon);
      cmd.bind(":ele",   n.ele);
      cmd.bind(":ntags", static_cast<int>(n.tags.size()));
      cmd.execute();

      std::string key, value;
      for (auto KV : n.tags)
      {
        std::tie(key, value) = KV;
        if (key == "ele") {
          sqlite3xx::command cmd(
            _db, "UPDATE node SET ele = :ele, ntags = :ntags WHERE id = :id");
          cmd.bind(":id",     n.id);
          cmd.bind(":ele",    std::stod(value));
          cmd.bind(":ntags", (static_cast<int>(n.tags.size()) - 1) );
          cmd.execute();
        }
        else {
          sqlite3xx::command cmd(
            _db, "INSERT INTO tag (node_id, k, v) VALUES (:node_id, :k, :v)");
          cmd.bind(":node_id", n.id);
          cmd.bind(":k",       key,   sqlite3xx::nocopy);
          cmd.bind(":v",       value, sqlite3xx::nocopy);
          cmd.execute();
        }
      }
    }
    xct.commit();
  } catch(std::exception& e) {
    xct.rollback();
    throw runtime_exception(e.what());
  }

}

void sqlite_database_helper_t::insert_cached_ways()
{
  sqlite3xx::transaction xct(_db);
  try {
    for (auto w : _cached_wys)
    {
      sqlite3xx::command cmd(
         _db, "INSERT INTO way (id, nrefs, ntags) VALUES (:id, :nrefs, :ntags)");
      cmd.bind(":id",    w.id);
      cmd.bind(":nrefs", static_cast<int>(w.refs.size()));
      cmd.bind(":ntags", static_cast<int>(w.tags.size()));
      cmd.execute();

      int order_ = 0;
      for (auto node_id : w.refs)
      {
        sqlite3xx::command cmd(
          _db, "INSERT INTO node_in_way (way_id, node_id, order_)"
               " VALUES (:way_id, :node_id, :order_)");
        cmd.bind(":way_id",  w.id);
        cmd.bind(":node_id", node_id);
        cmd.bind(":order_",  order_);
        cmd.execute();
        order_++;
      }

      std::string key, value;
      for (auto KV : w.tags)
      {
        std::tie(key, value) = KV;
        sqlite3xx::command cmd(
          _db, "INSERT INTO tag (way_id, k, v) VALUES (:way_id, :k, :v)");
        cmd.bind(":way_id", w.id);
        cmd.bind(":k",      key,   sqlite3xx::nocopy);
        cmd.bind(":v",      value, sqlite3xx::nocopy);
        cmd.execute();
      }

    }
    xct.commit();
  } catch(std::exception& e) {
    xct.rollback();
    throw runtime_exception(e.what());
  }
}

void sqlite_database_helper_t::insert_cached_relations()
{
  sqlite3xx::transaction xct(_db);
  try {
    for (auto r : _cached_rls)
    {
      sqlite3xx::command cmd(
         _db, "INSERT INTO relation (id, nrefs, ntags) VALUES (:id, :nrefs, :ntags)");
      cmd.bind(":id",    r.id);
      cmd.bind(":nrefs", static_cast<int>(r.refs.size()));
      cmd.bind(":ntags", static_cast<int>(r.tags.size()));
      cmd.execute();

      for (auto ref : r.refs) {
        if(ref.member_type == OSMPBF::Relation::MemberType(0)) // NODE
        {
          sqlite3xx::command cmd(
            _db, "INSERT INTO member_in_relation (id_of_relation, node_id, role)"
                 " VALUES (:id_of_relation, :node_id, :role)");
          cmd.bind(":id_of_relation", r.id);
          cmd.bind(":node_id",        ref.id);
          cmd.bind(":role",           ref.role, sqlite3xx::nocopy);
          cmd.execute();
        }
        else if (ref.member_type == OSMPBF::Relation::MemberType(1)) // WAY
        {
          sqlite3xx::command cmd(
            _db, "INSERT INTO member_in_relation (id_of_relation, way_id, role)"
                 " VALUES (:id_of_relation, :way_id, :role)");
          cmd.bind(":id_of_relation", r.id);
          cmd.bind(":way_id",         ref.id);
          cmd.bind(":role",           ref.role, sqlite3xx::nocopy);
          cmd.execute();
        }
        else if (ref.member_type == OSMPBF::Relation::MemberType(2)) // RELATION
        {
          sqlite3xx::command cmd(
            _db, "INSERT INTO member_in_relation (id_of_relation, relation_id, role)"
                 " VALUES (:id_of_relation, :relation_id, :role)");
          cmd.bind(":id_of_relation", r.id);
          cmd.bind(":relation_id",    ref.id);
          cmd.bind(":role",           ref.role, sqlite3xx::nocopy);
          cmd.execute();
        }
        else {
          //std::cout<<"Unknown member_type"<<std::endl;
        }

      }

      std::string key, value;
      for (auto KV : r.tags)
      {
        std::tie(key, value) = KV;
        sqlite3xx::command cmd(
          _db, "INSERT INTO tag (relation_id, k, v) VALUES (:relation_id, :k, :v)");
        cmd.bind(":relation_id", r.id);
        cmd.bind(":k",           key,   sqlite3xx::nocopy);
        cmd.bind(":v",           value, sqlite3xx::nocopy);
        cmd.execute();
      }

    }
    xct.commit();
  } catch(std::exception& e) {
    xct.rollback();
    throw runtime_exception(e.what());
  }
}

void sqlite_database_helper_t::retrieve(std::vector<osm::node>& nds)
{
  logger(logINFO)
    << left("[DB]", 14)
    <<"> Add Junctions";

  sqlite3xx::query qry(_db,
    "SELECT node.*, k, v              "
    " FROM node                       "
    "  LEFT JOIN tag ON node_id = id  "
    " WHERE id IN (                   "
    "   SELECT node_id                "
    "    FROM node_in_way             "
    "     INNER JOIN highway          "
    "      ON way_id = id )           ");

  sqlite3xx::query::iterator qit = qry.begin();
  while (qit != qry.end())
  {
    osm::node n;
    int ntags;

    std::tie(n.id, n.lat, n.lon, n.ele, ntags) =
      (*qit).get_columns<
        long long int, double, double, double, int>(0, 1, 2, 3, 4);
    //std::cout << n.id << "\t"
    //    << n.lat << "\t"
    //    << n.lon << "\t"
    //    << n.ele << std::endl;

    for (int i = 0; i < ntags; ++i)
    {
      std::string k, v;
      std::tie(k, v) =
        (*qit).get_columns<std::string, std::string>(5, 6);
      n.add_tag(k, v);
      ++qit;
      //std::cout << w.id << "\t"
      //    << k << "\t" << v << std::endl;
    }
    if (!ntags) ++qit;

    nds.push_back(n);
  }

}

void sqlite_database_helper_t::retrieve(std::vector<osm::way>& wys)
{
  logger(logINFO)
    << left("[DB]", 14)
    <<"> Add Segments";

  sqlite3xx::query qry(_db,
    " SELECT                         "
    "   highway.id    AS id,         "
    "   highway.ntags AS ntags,      "
    "   highway.nrefs AS nrefs,      "
    "   tag.k         AS k,          "
    "   tag.v         AS v,          "
    "   NULL          AS node_id,    "
    "   NULL                         "
    "  FROM highway                  "
    "   LEFT JOIN tag                "
    "    ON id = tag.way_id          "
    " UNION                          "
    " SELECT                         "
    "   highway.id    AS id,         "
    "   highway.ntags AS ntags,      "
    "   highway.nrefs AS nrefs,      "
    "   NULL          AS k,          "
    "   NULL          AS v,          "
    "   nd.node_id    AS node_id,    "
    "   nd.order_                    "
    "  FROM highway                  "
    "   INNER JOIN node_in_way AS nd "
    "    ON id = nd.way_id           "
    " ORDER BY id ASC, nd.order_ ASC ");

  sqlite3xx::query::iterator qit = qry.begin();
  while (qit != qry.end())
  {
    osm::way w;
    int nrefs, ntags;

    std::tie(w.id, ntags, nrefs) =
      (*qit).get_columns<long long int, int, int>(0, 1, 2);
    //std::cout << w.id << "\t"
    //    << ntags << "\t" << nrefs << std::endl;

    for (int i = 0; i < ntags; ++i)
    {
      std::string k, v;
      std::tie(k, v) =
        (*qit).get_columns<std::string, std::string>(3, 4);
      w.add_tag(k, v);
      ++qit;
      //std::cout << w.id << "\t"
      //    << k << "\t" << v << std::endl;
    }
    if (!ntags) ++qit;

    for (int i = 0; i < nrefs; ++i)
    {
      long long int nd;
      std::tie(nd) =
        (*qit).get_columns<long long int>(5);
      w.add_ref(nd);
      ++qit;
      //std::cout << w.id << "\t" << nd << std::endl;
    }
    if (!nrefs) ++qit;

    wys.push_back(w);
  }

}

void sqlite_database_helper_t::retrieve(
  std::vector<osm::node_as_via_turn_restriction>& trs)
{
  logger(logINFO)
    << left("[DB]", 14)
    << "> Add Turn Costs";

  sqlite3xx::query qry(_db,
    " SELECT                                  "
    "   nvr.id              AS id,            "
    "   nvr.restriction     AS restriction,   "
    "   nvr.via             AS via,           "
    "   nvr.role_from       AS role,          "
    "   nvr.way_from        AS way_from,      "
    "   highway.nrefs       AS nrefs_from,    "
    "   ndF.node_id         AS nd_from,       "
    "   ndF.order_          AS order_from,    "
    "   NULL                AS way_to,        "
    "   NULL                AS nrefs_to,      "
    "   NULL                AS nd_to,         "
    "   NULL                AS order_to       "
    "  FROM node_as_via_restriction AS nvr    "
    "   INNER JOIN node_in_way AS ndF         "
    "    ON nvr.way_from = ndF.way_id         "
    "   INNER JOIN highway                    "
    "    ON ndF.way_id = highway.id           "
    " UNION                                   "
    " SELECT                                  "
    "   nvr.id              AS id,            "
    "   nvr.restriction     AS restriction,   "
    "   nvr.via             AS via,           "
    "   nvr.role_to         AS role,          "
    "   NULL                AS way_from,      "
    "   NULL                AS nrefs_from,    "
    "   NULL                AS nd_from,       "
    "   NULL                AS order_from,    "
    "   nvr.way_to          AS way_to,        "
    "   highway.nrefs       AS nrefs_to,      "
    "   ndT.node_id         AS nd_to,         "
    "   ndT.order_          AS order_to       "
    "  FROM node_as_via_restriction AS nvr    "
    "   INNER JOIN node_in_way AS ndT         "
    "    ON nvr.way_to = ndT.way_id           "
    "   INNER JOIN highway                    "
    "    ON ndT.way_id = highway.id           "
    " ORDER BY id, way_from DESC, order_from, "
    "    way_to, order_to                     ");

  sqlite3xx::query::iterator qit = qry.begin();
  while (qit != qry.end())
  {
    osm::node_as_via_turn_restriction r;

    long long int id, prev_id;
    int           nrefs;
    std::string   role;
    bool          fromFound, toFound;

    fromFound = false;
    toFound   = false;

    std::tie(id, r.restriction, r.via, role) =
      (*qit).get_columns<
        long long int, std::string, long long int, std::string>(0, 1, 2, 3);
    //std::cout << r.via << std::endl;

    prev_id = id;

    do //while (prev_id == id)
    {
      if (role == "from")
      {   fromFound = true;

          osm::way wfrom;
          std::tie(wfrom.id, nrefs) =
            (*qit).get_columns<long long int, int>(4, 5);
          //std::cout << wfrom.id <<" \t "<< nrefs << std::endl;

          for (int i = 0; i < nrefs; ++i)
          {
            long long int nd;
            std::tie(nd) =
              (*qit).get_columns<long long int>(6);
            wfrom.add_ref(nd);
            ++qit;
            //std::cout << wfrom.id << "\t" << nd << std::endl;
          }
          if (!nrefs) ++qit;
          r.from.push_back(wfrom);
      }
      else if (role == "to")
      {   toFound = true;

          osm::way wto;
          std::tie(wto.id, nrefs) =
            (*qit).get_columns<long long int, int>(8, 9);
          //std::cout << wto.id <<" \t "<< nrefs << std::endl;

          for (int i = 0; i < nrefs; ++i)
          {
            long long int nd;
            std::tie(nd) =
              (*qit).get_columns<long long int>(10);
            wto.add_ref(nd);
            ++qit;
            //std::cout << wto.id << "\t" << nd << std::endl;
          }
          if (!nrefs) ++qit;
          r.to.push_back(wto);
      }
      //else

      if (qit == qry.end()) break;

      std::tie(id, role) =
        (*qit).get_columns<long long int, std::string>(0, 3);

    } while ( prev_id == id );

    if (fromFound && toFound)
      trs.push_back(r);
  }

}


} // namespace gol
