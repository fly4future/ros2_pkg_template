/* includes //{ */

#include <rclcpp/logging.hpp>
#include <rclcpp/rclcpp.hpp>

/* for smart pointers (do not use raw pointers) */
#include <memory>

/* for protecting variables from simultaneous manipulation by from multiple threads */
#include <mutex>

/* for writing and reading from streams */
#include <iostream>

/* custom helper functions from our library */
#include <mrs_lib/param_loader.h>
#include <mrs_lib/subscriber_handler.h>
#include <mrs_lib/mutex.h>
#include <mrs_lib/msg_extractor.h>
#include <mrs_lib/publisher_handler.h>
#include <mrs_lib/service_client_handler.h>
#include <mrs_lib/node.h>
#include <mrs_lib/service_server_handler.h>

/* for simple ros subs/pubs */
#include <std_msgs/msg/string.hpp>
#include <std_msgs/msg/bool.hpp>

/* for calling simple ros services */
#include <std_srvs/srv/trigger.hpp>
#include <std_msgs/msg/string.hpp>

//}

/* defines //{ */

#if USE_ROS_TIMER == 1
typedef mrs_lib::ROSTimer TimerType;
#else
typedef mrs_lib::ThreadTimer TimerType;
#endif

namespace f4f_ros2_template
{


/* class ROS2Template //{ */

class ROS2Template : public mrs_lib::Node {
public:
  ROS2Template(rclcpp::NodeOptions options);

  void initialize();

private:
  rclcpp::Node::SharedPtr  node_;
  rclcpp::Clock::SharedPtr clock_;

  rclcpp::CallbackGroup::SharedPtr cbkgrp_sub_;
  rclcpp::CallbackGroup::SharedPtr cbkgrp_client_;
  rclcpp::CallbackGroup::SharedPtr cbkgrp_server_;
  rclcpp::CallbackGroup::SharedPtr cbkgrp_timer_;

  std::string _uav_name_;
  bool        is_initialized_ = false;

  // | ----------------------- subscribers ---------------------- |
  mrs_lib::SubscriberHandler<std_msgs::msg::String> sub_hello_world_;

  void callbackSubHelloWorld(const std_msgs::msg::String::ConstSharedPtr msg);

  // | ----------------------- publishers ----------------------- |
  mrs_lib::PublisherHandler<std_msgs::msg::String> pub_hello_world_;

  // | ------------------------- timers ------------------------- |

  void                       timerPublishHelloWorld();
  std::shared_ptr<TimerType> timer_publisher_hello_world_;
  double                     _rate_timer_publisher_hello_world_;
  std::atomic<int>           count_ = 0;

  void                       timerCheckPermit();
  std::shared_ptr<TimerType> timer_check_permit_;
  double                     _rate_timer_check_permit_;

  // | ------------------------- servers ------------------------ |

  mrs_lib::ServiceServerHandler<std_srvs::srv::Trigger> server_start_publishing_;
  bool callbackStartPublishing(const std::shared_ptr<std_srvs::srv::Trigger::Request> req, const std::shared_ptr<std_srvs::srv::Trigger::Response> res);

  mrs_lib::ServiceServerHandler<std_srvs::srv::Trigger> server_stop_publishing_;
  bool callbackStopPublishing(const std::shared_ptr<std_srvs::srv::Trigger::Request> req, const std::shared_ptr<std_srvs::srv::Trigger::Response> res);

  // | ------------------------- clients ------------------------ |

  mrs_lib::ServiceClientHandler<std_srvs::srv::Trigger> client_start_publishing_;
  mrs_lib::ServiceClientHandler<std_srvs::srv::Trigger> client_stop_publishing_;

  // | ------------------- loading variables -------------------- |

  std_msgs::msg::Bool publishing_permited_;
  std::mutex          mutex_publishing_permited_;

  // | -------------------- support functions ------------------- |
};

//}

/* ROS2Template() //{ */

ROS2Template::ROS2Template(rclcpp::NodeOptions options) : Node("f4f_ros2_template", options) {
  initialize();
}

//}

/* initialize() //{ */

void ROS2Template::initialize() {

  node_  = this->this_node_ptr();
  clock_ = node_->get_clock();

  cbkgrp_sub_    = this_node().create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  cbkgrp_client_ = this_node().create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  cbkgrp_server_ = this_node().create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  cbkgrp_timer_  = this_node().create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

  mrs_lib::ParamLoader param_loader(node_);

  param_loader.addYamlFileFromParam("config");

  param_loader.loadParam("uav_name", _uav_name_);
  param_loader.loadParam("rate/publish_hello_world", _rate_timer_publisher_hello_world_);
  param_loader.loadParam("rate/check_permit", _rate_timer_check_permit_);

  if (!param_loader.loadedSuccessfully()) {
    RCLCPP_ERROR(node_->get_logger(), "Failed to load non-optional parameters!");
    rclcpp::shutdown();
    exit(1);
  }

  // | ----------------------- subscribers ---------------------- |

  mrs_lib::SubscriberHandlerOptions shopts;
  shopts.node                                = node_;
  shopts.node_name                           = "ROS2Template";
  shopts.threadsafe                          = true;
  shopts.autostart                           = true;
  shopts.subscription_options.callback_group = cbkgrp_sub_;

  sub_hello_world_ = mrs_lib::SubscriberHandler<std_msgs::msg::String>(shopts, "~/hello_world_in", &ROS2Template::callbackSubHelloWorld, this);

  // | ----------------------- publishers ----------------------- |

  pub_hello_world_ = mrs_lib::PublisherHandler<std_msgs::msg::String>(node_, "~/hello_world_out");

  publishing_permited_.data = false;

  // | ------------------------- timers ------------------------- |

  mrs_lib::TimerHandlerOptions opts_autostart;

  opts_autostart.node           = node_;
  opts_autostart.autostart      = true;
  opts_autostart.callback_group = cbkgrp_timer_;

  // Use local scope {} to reuse fuction name for init and to clean resources after init
  {
    std::function<void()> callback_fn = std::bind(&ROS2Template::timerCheckPermit, this);
    timer_check_permit_               = std::make_shared<TimerType>(opts_autostart, rclcpp::Rate(_rate_timer_check_permit_, clock_), callback_fn);
  }

  mrs_lib::TimerHandlerOptions opts_no_autostart;

  opts_no_autostart.node           = node_;
  opts_no_autostart.autostart      = false;
  opts_no_autostart.callback_group = cbkgrp_timer_;

  {
    std::function<void()> callback_fn = std::bind(&ROS2Template::timerPublishHelloWorld, this);
    timer_publisher_hello_world_      = std::make_shared<TimerType>(opts_no_autostart, rclcpp::Rate(_rate_timer_publisher_hello_world_, clock_), callback_fn);
  }

  // | ------------------------- servers ------------------------ |

  server_start_publishing_ = mrs_lib::ServiceServerHandler<std_srvs::srv::Trigger>(
      node_, "~/start_publishing_in", std::bind(&ROS2Template::callbackStartPublishing, this, std::placeholders::_1, std::placeholders::_2),
      rclcpp::SystemDefaultsQoS(), cbkgrp_server_);

  server_stop_publishing_ = mrs_lib::ServiceServerHandler<std_srvs::srv::Trigger>(
      node_, "~/stop_publishing_in", std::bind(&ROS2Template::callbackStopPublishing, this, std::placeholders::_1, std::placeholders::_2),
      rclcpp::SystemDefaultsQoS(), cbkgrp_server_);

  // | ------------------------- clients ------------------------ |

  client_start_publishing_ = mrs_lib::ServiceClientHandler<std_srvs::srv::Trigger>(node_, "~/start_publishing_out", cbkgrp_client_);
  client_stop_publishing_  = mrs_lib::ServiceClientHandler<std_srvs::srv::Trigger>(node_, "~/stop_publishing_out", cbkgrp_client_);

  // | --------------------- finish the init -------------------- |

  RCLCPP_INFO_ONCE(node_->get_logger(), "Initialized");

  is_initialized_ = true;
}

//}

// --------------------------------------------------------------
// |                     subscriber callbacks                   |
// --------------------------------------------------------------

/* callbackSubHelloWorld() //{ */

void ROS2Template::callbackSubHelloWorld(const std_msgs::msg::String::ConstSharedPtr message) {

  /* do not continue if the component is not initialized */
  if (!is_initialized_) {
    return;
  }

  RCLCPP_INFO(node_->get_logger(), "I've heard: %s", message->data.c_str());
}

//}

// --------------------------------------------------------------
// |                       timer callbacks                      |
// --------------------------------------------------------------

/* timerPublishHelloWorld() //{ */

void ROS2Template::timerPublishHelloWorld() {
  /* timer that starts only when called */

  if (!is_initialized_) {
    return;
  }

  std_msgs::msg::String message;

  message.data = "Hello World! " + std::to_string(count_++);

  pub_hello_world_.publish(message);
}

//}

/* timerCheckPermit() //{ */

void ROS2Template::timerCheckPermit() {
  /* timer that starts automatically */

  if (!is_initialized_) {
    return;
  }

  // get the variable under the mutex, use const if you know variable cannot change
  const auto current_permit = mrs_lib::get_mutexed(mutex_publishing_permited_, publishing_permited_);

  // start/stop the publishing timer if permit is true/false
  if (current_permit.data) {

    RCLCPP_WARN(node_->get_logger(), "Permit is true.");
    timer_publisher_hello_world_->start();
  } else {

    RCLCPP_WARN(node_->get_logger(), "Permit is false.");
    timer_publisher_hello_world_->stop();
  }
}

//}

// --------------------------------------------------------------
// |                      service callbacks                     |
// --------------------------------------------------------------

/* callbackStartPublishing() //{ */

bool ROS2Template::callbackStartPublishing([[maybe_unused]] const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
                                           const std::shared_ptr<std_srvs::srv::Trigger::Response>                 response) {

  if (!is_initialized_) {

    response->success = false;
    response->message = "[%s] not initialized!", node_->get_name();
    RCLCPP_WARN(node_->get_logger(), "Cannot start publishing, nodelet is not initialized.");

    // you almost always want to return true to allow the response to go through
    return true;
  }

  auto current_permit = mrs_lib::get_mutexed(mutex_publishing_permited_, publishing_permited_);

  if (!current_permit.data) {

    current_permit.data = true;

    mrs_lib::set_mutexed(mutex_publishing_permited_, current_permit, publishing_permited_);

    RCLCPP_INFO(node_->get_logger(), "Publishing started.");

    response->success = true;
    response->message = "Publishing started.";

  } else {

    // you can get a message from a subscriber this way as well
    const auto current_message = sub_hello_world_.getMsg();

    RCLCPP_WARN(node_->get_logger(), "Already publishing. The message is: %s", current_message->data.c_str());
    response->success = false;
    response->message = "Already publishing. The message is: " + current_message->data;
  }

  return true;
}

//}

/* callbackStartPublishing() //{ */

bool ROS2Template::callbackStopPublishing([[maybe_unused]] const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
                                          const std::shared_ptr<std_srvs::srv::Trigger::Response>                 response) {

  if (!is_initialized_) {

    response->success = false;
    response->message = "[%s] not initialized!", node_->get_name();
    RCLCPP_WARN(node_->get_logger(), "Cannot ban publishing, nodelet is not initialized.");
    return true;
  }

  auto current_permit = mrs_lib::get_mutexed(mutex_publishing_permited_, publishing_permited_);

  if (current_permit.data) {

    current_permit.data = false;

    mrs_lib::set_mutexed(mutex_publishing_permited_, current_permit, publishing_permited_);

    RCLCPP_INFO(node_->get_logger(), "Publishing stopped.");

    response->success = true;
    response->message = "Publishing stopped.";

  } else {

    RCLCPP_WARN(node_->get_logger(), "Publishing is already stopped.");
    response->success = false;
    response->message = "Publishing is already stopped.";
  }

  return true;
}

//}


// // --------------------------------------------------------------
// // |                      support functions                     |
// // --------------------------------------------------------------


}  // namespace f4f_ros2_template

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(f4f_ros2_template::ROS2Template)
