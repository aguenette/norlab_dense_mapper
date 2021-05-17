#ifndef MAPPER_H
#define MAPPER_H

#include <pointmatcher/PointMatcher.h>
#include "DenseMap.h"
#include "Trajectory.h"
#include <future>
#include <mutex>

namespace norlab_dense_mapper
{
class DenseMapper
{
  private:
    typedef PointMatcher<float> PM;

    PM::DataPointsFilters inputFilters;
    PM::DataPointsFilters mapPostFilters;
    std::string mapUpdateCondition;
    float mapUpdateDelay;
    float mapUpdateDistance;
    bool is3D;
    bool isOnline;
    std::atomic_bool isMapping;
    DenseMap denseMap;
    PM::TransformationParameters pose;
    Trajectory trajectory;
    std::shared_ptr<PM::Transformation> transformation;
    std::shared_ptr<PM::DataPointsFilter> radiusFilter;
    std::chrono::time_point<std::chrono::steady_clock> lastTimeMapWasUpdated;
    PM::TransformationParameters lastPoseWhereMapWasUpdated;
    std::mutex poseLock;
    std::mutex trajectoryLock;
    std::future<void> mapUpdateFuture;

    bool shouldUpdateMap(const std::chrono::time_point<std::chrono::steady_clock>& currentTime,
                         const PM::TransformationParameters& currentPose) const;
    void updateMap(const PM::DataPoints& currentInput,
                   const PM::TransformationParameters& currentPose,
                   const std::chrono::time_point<std::chrono::steady_clock>& currentTimeStamp);

  public:
    DenseMapper(const std::string& inputFiltersConfigFilePath,
                const std::string& mapPostFiltersConfigFilePath,
                std::string mapUpdateCondition,
                const float& mapUpdateDelay,
                const float& mapUpdateDistance,
                const float& minDistNewPoint,
                const float& sensorMaxRange,
                const float& priorDynamic,
                const float& thresholdDynamic,
                const float& beamHalfAngle,
                const float& epsilonA,
                const float& epsilonD,
                const float& alpha,
                const float& beta,
                const bool& is3D,
                const bool& isOnline,
                const bool& computeProbDynamic,
                const bool& isMapping,
                const bool& saveMapCellsOnHardDrive);
    void loadYamlConfig(const std::string& inputFiltersConfigFilePath,
                        const std::string& mapPostFiltersConfigFilePath);
    void processInput(const PM::DataPoints& inputInSensorFrame,
                      const PM::TransformationParameters& currentPose,
                      const std::chrono::time_point<std::chrono::steady_clock>& timeStamp);
    PM::DataPoints getMap();
    void setMap(const PM::DataPoints& newMap);
    bool getNewLocalMap(PM::DataPoints& mapOut);
    PM::TransformationParameters getPose();
    bool getIsMapping() const;
    void setIsMapping(const bool& newIsMapping);
    Trajectory getTrajectory();
};
}

#endif