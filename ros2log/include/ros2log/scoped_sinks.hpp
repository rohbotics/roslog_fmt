#ifndef SCOPED_SINKS_H
#define SCOPED_SINKS_H

#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <ros2log/logger.hpp>
#include <rosgraph_msgs/msg/log.hpp>

class ScopedRosoutSink {
 public:
  ScopedRosoutSink(std::shared_ptr<Logger> logger,
                   std::shared_ptr<rclcpp::node::Node> n)
      : logger_(logger), node(n) {
    rosout_pub = node->create_publisher<rosgraph_msgs::msg::Log>("rosout");
    logger_->register_sink(
        Sink("rosout", Log_Levels::INFO, [this](LogMessage message) {
          auto now = message.timestamp.time_since_epoch();
          auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now);
          now -= seconds;
          auto nano_seconds =
              std::chrono::duration_cast<std::chrono::nanoseconds>(now);

          auto secs = seconds.count();
          auto nsecs = nano_seconds.count();

          auto msg = rosgraph_msgs::msg::Log();
          msg.header.stamp.sec = secs;
          msg.header.stamp.nanosec = nsecs;
          msg.level = static_cast<uint8_t>(message.level);
          msg.file = message.file;
          msg.function = message.function;
          msg.line = message.line;
          msg.msg = message.log_string;
          rosout_pub->publish(msg);
        }));
  };

  ~ScopedRosoutSink() { logger_->deregister_sink("rosout"); };

 private:
  std::shared_ptr<Logger> logger_;

  std::shared_ptr<rclcpp::node::Node> node;
  rclcpp::Publisher<rosgraph_msgs::msg::Log>::SharedPtr rosout_pub;
};

class ScopedPrintSink {
 public:
  ScopedPrintSink(std::shared_ptr<Logger> logger) : logger_(logger) {
    logger_->register_sink(
        Sink("print", Log_Levels::INFO, [](LogMessage message) {
          switch (message.level) {
            case Log_Levels::FATAL:
            case Log_Levels::ERROR:
              printf("\x1b[31m%s\x1b[0m\n", message.log_string);
              break;
            case Log_Levels::WARN:
              printf("\x1b[33m%s\x1b[0m\n", message.log_string);
              break;
            case Log_Levels::DEBUG:
              printf("\x1b[32m%s\x1b[0m\n", message.log_string);
              break;
            case Log_Levels::INFO:
              printf("%s\n", message.log_string);
              break;
          }
        }));
  };

  ~ScopedPrintSink() { logger_->deregister_sink("print"); };

 private:
  std::shared_ptr<Logger> logger_;
};

#endif