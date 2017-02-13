#ifndef CONFIG_H
#define CONFIG_H

#ifndef NLOG
#define LOG                                  1
#endif

#ifndef NDEBUG
#define DEBUG                                1
#endif

#define PROGRAM_NAME                         "Sii-Mobility SPLib"
#define PROGRAM_SHORT_NAME                   "Sii-Mobility SPLib"
#define PROGRAM_VERSION                      "0.2"

#ifndef SII_MOBILITY
#	define RELATIVE_DIR	                     "../"
#else
#	define RELATIVE_DIR                      "app/lib/extensions/splib/"
#endif

#define PROGRAM_REVISION                     "$Rev: 1 $"
#define DEFAULT_LOGFILE                      "log/sii_mobility.log"
#define LOCK_FOLDER                          "/var/lock/"
#define DEFAULT_GEOJSON_OUTPUTFILE           "optimized_route.geojson"

#define AVERAGE_WALKING_SPEED                (5)
#define AVERAGE_BICYCLE_SPEED                (15)
#define AVERAGE_CAR_SPEED                    (45)
#define COUNTRY_AVERAGE_CAR_SPEED            (65)
#define MOTORWAY_AVERAGE_CAR_SPEED           (115)
#define MAX_WALKING_SPEED                    (7)
#define MAX_BICYCLE_SPEED                    (25)
#define MAX_BUS_SPEED                        (70)
#define MAX_TRAIN_SPEED                      (200)

#define MARIANA_TRENCH_DEPTH                 (-10994)

// eMOA*
#define EPLSILON_PARETO                      (0.05)
#define ELLIPSE_PERIPHERAL_DISTANCE_PERCENT  (20)
#define ELLIPSE_PRUNING_THRESHOLD            (1 * 1000) // 1km
#define PENALTY_COEFF_UPHILL                 (7.92)
#define MAX_DESC_GRAD                        (0.35)
#define MAX_DOWNHILL_SPEED_MULTIPLIER        (1.6)
#define EUCLIDEAN_DISTANCE_DELTA             (0.15)

// RAPTOR
#define RAPTOR_MAX_ROUNDS                    (5)
#define MAX_TRANSFER                         (3)
#define UNDEFINED                            (UINT32_MAX)
#define WALK                                 (UINT32_MAX-1)
#define UNREACHED                            (INT_MAX)    // time_Rt
#define ONBOARD                              (1)
#define CANCELED                             (INT16_MAX)

#define MAX_SEARCH_RADIUS                    (5.0 * 1000) // 5km
#define TRANSFER_SEARCH_RADIUS               (5.0 * 100)  // 0.5km


#endif  // CONFIG_H
