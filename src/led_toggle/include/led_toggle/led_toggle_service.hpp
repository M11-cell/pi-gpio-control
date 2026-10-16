#ifndef LED_TOGGLE_SERVICE_HPP
#define LED_TOGGLE_SERVICE_HPP

#include "rclcpp/rclcpp.hpp"


#include <map>
#include <memory>
#include <set>


#include "service_interface/srv/led_state.hpp"
#include "service_interface/msg/led_status.hpp"
#include <wiringPi.h>


class LedServer: public rclcpp::Node{

    public: 

        LedServer();

        ~LedServer() override; 

        
        private:

            using LedStateSrv = service_interface::srv::LedState; 
            using LedStateMsg = service_interface::msg::LedStatus;

            void HandleSetLed(
                const std::shared_ptr<LedStateSrv::Request> request,
                const std::shared_ptr<LedStateSrv::Response> response);

            void PublishStatus(int gpio, bool status);;

            std::set<int> allowed_pins; 
            std::map<int, bool> pin_state_; 


        rclcpp::Publisher<LedStateMsg>::SharedPtr led_status_pub_; 
        rclcpp::Service<LedStateSrv>::SharedPtr server_; 

};



#endif