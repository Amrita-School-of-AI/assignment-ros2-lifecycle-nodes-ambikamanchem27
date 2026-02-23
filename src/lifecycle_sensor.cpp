#include <chrono>
#include <memory>
#include <random>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "std_msgs/msg/float64.hpp"

using namespace std::chrono_literals;
using rclcpp_lifecycle::LifecycleNode;
using rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface;

class LifecycleSensor : public LifecycleNode
{
public:
  LifecycleSensor()
  : LifecycleNode("lifecycle_sensor")
  {
    RCLCPP_INFO(get_logger(), "Lifecycle Sensor Node created");
  }

  LifecycleNodeInterface::CallbackReturn
  on_configure(const rclcpp_lifecycle::State &)
  {
    publisher_ = this->create_publisher<std_msgs::msg::Float64>(
      "/sensor_data", 10);

    RCLCPP_INFO(get_logger(), "Sensor configured");
    return LifecycleNodeInterface::CallbackReturn::SUCCESS;
  }

  LifecycleNodeInterface::CallbackReturn
  on_activate(const rclcpp_lifecycle::State &)
  {
    publisher_->on_activate();

    timer_ = this->create_wall_timer(
      500ms,
      std::bind(&LifecycleSensor::publish_sensor_data, this));

    RCLCPP_INFO(get_logger(), "Sensor activated");
    return LifecycleNodeInterface::CallbackReturn::SUCCESS;
  }

  LifecycleNodeInterface::CallbackReturn
  on_deactivate(const rclcpp_lifecycle::State &)
  {
    timer_.reset();
    publisher_->on_deactivate();

    RCLCPP_INFO(get_logger(), "Sensor deactivated");
    return LifecycleNodeInterface::CallbackReturn::SUCCESS;
  }

  LifecycleNodeInterface::CallbackReturn
  on_cleanup(const rclcpp_lifecycle::State &)
  {
    timer_.reset();
    publisher_.reset();

    RCLCPP_INFO(get_logger(), "Sensor cleaned up");
    return LifecycleNodeInterface::CallbackReturn::SUCCESS;
  }

  LifecycleNodeInterface::CallbackReturn
  on_shutdown(const rclcpp_lifecycle::State &)
  {
    RCLCPP_INFO(get_logger(), "Sensor shutting down");
    return LifecycleNodeInterface::CallbackReturn::SUCCESS;
  }

private:
  void publish_sensor_data()
  {
    auto msg = std_msgs::msg::Float64();
    msg.data = get_random_value();

    RCLCPP_INFO(get_logger(), "Publishing sensor data: %.2f", msg.data);

    publisher_->publish(msg);
  }

  double get_random_value()
  {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 100.0);
    return dis(gen);
  }

  rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Float64>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<LifecycleSensor>();
  rclcpp::spin(node->get_node_base_interface());
  rclcpp::shutdown();
  return 0;
}
