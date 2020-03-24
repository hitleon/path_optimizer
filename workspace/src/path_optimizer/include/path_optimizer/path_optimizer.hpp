//
// Created by ljn on 19-8-16.
//

#ifndef PATH_OPTIMIZER__PATHOPTIMIZER_HPP_
#define PATH_OPTIMIZER__PATHOPTIMIZER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <glog/logging.h>
#include "grid_map_core/grid_map_core.hpp"
#include "path_optimizer/config/config.hpp"

namespace PathOptimizationNS {

class ReferencePath;
class State;
class Map;
class CollisionChecker;
class VehicleState;

class PathOptimizer {
public:
    PathOptimizer() = delete;
    PathOptimizer(const State &start_state,
                  const State &end_state,
                  const grid_map::GridMap &map);
    ~PathOptimizer();

    // Change config.
    template<typename T>
    bool setConfig(const std::string &config_name, const T &value);

    // Call this to get the optimized path.
    bool solve(const std::vector<State> &reference_points, std::vector<State> *final_path);
    bool solveWithoutSmoothing(const std::vector<State> &reference_points, std::vector<State> *final_path);

    // Get config:
    const Config &getConfig() const;

    // Only for visualization purpose.
    const std::vector<State> &getRearBounds() const;
    const std::vector<State> &getCenterBounds() const;
    const std::vector<State> &getFrontBounds() const;
    const std::vector<State> &getSmoothedPath() const;
    std::vector<std::vector<double>> a_star_display_;

private:
    // TODO: abandon this function, use the config class instead.
    void setConfig();

    // Core function.
    bool optimizePath(std::vector<State> *final_path);

    // Divide smoothed path into segments.
    bool segmentSmoothedPath();

    const Map *grid_map_;
    CollisionChecker *collision_checker_;
    Config *config_;
    ReferencePath *reference_path_;
    VehicleState *vehicle_state_;
    size_t size_{};

    // For visualization purpose.
    std::vector<State> rear_bounds_;
    std::vector<State> center_bounds_;
    std::vector<State> front_bounds_;
    std::vector<State> smoothed_path_;
};

template<typename T>
bool PathOptimizer::setConfig(const std::string &config_name, const T &value) {
    if (config_name == "car_width_") {
        config_->car_width_ = static_cast<double>(value);
        config_->circle_radius_ = sqrt(pow(config_->car_length_ / 8, 2) + pow(config_->car_width_ / 2, 2));
    } else if (config_name == "car_length_") {
        config_->car_length_ = static_cast<double>(value);
        config_->circle_radius_ = sqrt(pow(config_->car_length_ / 8, 2) + pow(config_->car_width_ / 2, 2));
        config_->d1_ = -3.0 / 8.0 * config_->car_length_;
        config_->d2_ = -1.0 / 8.0 * config_->car_length_;
        config_->d3_ = 1.0 / 8.0 * config_->car_length_;
        config_->d4_ = 3.0 / 8.0 * config_->car_length_;
    } else if (config_name == "wheel_base_") {
        config_->wheel_base_ = static_cast<double>(value);
    } else if (config_name == "rear_axle_to_center_distance_") {
        config_->rear_axle_to_center_distance_ = static_cast<double>(value);
    } else if (config_name == "max_steer_angle_") {
        config_->max_steer_angle_ = static_cast<double>(value);
    } else if (config_name == "modify_input_points_") {
        config_->modify_input_points_ = static_cast<bool>(value);
    } else if (config_name == "constraint_end_heading_") {
        config_->constraint_end_heading_ = static_cast<bool>(value);
    } else if (config_name == "exact_end_position_") {
        config_->exact_end_position_ = static_cast<bool>(value);
    } else if (config_name == "expected_safety_margin_") {
        config_->expected_safety_margin_ = static_cast<double>(value);
    } else if (config_name == "raw_result_") {
        config_->raw_result_ = static_cast<bool>(value);
    } else if (config_name == "output_interval_") {
        config_->output_interval_ = static_cast<double>(value);
    } else if (config_name == "info_ootput_") {
        config_->info_output_ == static_cast<bool>(value);
    } else if (config_name == "optimization_method_") {
        config_->optimization_method_ = static_cast<OptimizationMethod>(value);
    }
    else {
        LOG(WARNING) << "[PathOptimizer] No config named " << config_name << " or this config can only be changed in config file.";
        return false;
    }
    LOG(INFO) << "[PathOptimizer] Config " << config_name << " is successfully changed to " << value << "!";
    return true;
}

}

#endif //PATH_OPTIMIZER__PATHOPTIMIZER_HPP_
