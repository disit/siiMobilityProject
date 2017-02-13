
srcdir = src

CXX = g++
CXXFLAGS = -g -O0 -std=c++1y -fno-fast-math -Wall -Wextra -pedantic 
LDFLAGS = -lpthread -ldl -lcrypt -lm -lc -lboost_system -lboost_filesystem -lboost_serialization -lxerces-c -lprotobuf-lite -losmpbf -lz 

PROGRAMS = \
geo.o \
utility.o \
json_utf8.o \
bitset.o \
json_parser.o \
logger.o \
raptor_timetable_builder.o \
algorithm.o \
cache.o \
engine.o \
main.o

TEST = \
logger.o \
osm_tags_logger.o

all: splib clean
test: osm_tags_logger clean

splib: $(PROGRAMS)
	$(CXX) -o $@ $^ $(LDFLAGS)
	mv splib build

osm_tags_logger: $(TEST)
	$(CXX) -o $@ $^ $(LDFLAGS)
	mv osm_tags_logger build

osm_tags_logger.o: $(srcdir)/test/osm_tags_logger.cc
	$(CXX) $(CXXFLAGS) -c $<

main.o: $(srcdir)/main.cc
	$(CXX) $(CXXFLAGS) -c $<

engine.o: $(srcdir)/engine.cc
	$(CXX) $(CXXFLAGS) -c $<

cache.o: $(srcdir)/cache.cc
	$(CXX) $(CXXFLAGS) -c $<

algorithm.o: $(srcdir)/algorithm.cc
	$(CXX) $(CXXFLAGS) -c $<

raptor_timetable_builder.o: $(srcdir)/round_based/round_based_model/raptor_timetable_builder.cc
	$(CXX) $(CXXFLAGS) -c $<

bitset.o: $(srcdir)/utils/bitset.cc
	$(CXX) $(CXXFLAGS) -c $<

logger.o: $(srcdir)/utils/logger.cc
	$(CXX) $(CXXFLAGS) -c $<

json_parser.o: $(srcdir)/utils/json_parser.cc
	$(CXX) $(CXXFLAGS) -c $<

json_utf8.o: $(srcdir)/utils/json_utf8.cc
	$(CXX) $(CXXFLAGS) -c $<

utility.o: $(srcdir)/utility.cc
	$(CXX) $(CXXFLAGS) -c $<

geo.o: $(srcdir)/geo.cc
	$(CXX) $(CXXFLAGS) -c $<


.PHONY: clean
clean:
	rm -f *.o core $(PROGRAMS)


