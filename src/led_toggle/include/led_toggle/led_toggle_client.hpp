#pragma once

#ifndef LED_TOGGLE_CLIENT_HPP
#define LED_TOGGLE_CLIENT_HPP

#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/msg/string.hpp"
#include <iostream>
#include "service_interface/srv/led_state.hpp"
#include "service_interface/msg/led_status.hpp"

namespace LedClient{ 

class LedClient: public rclcpp::Node{

    public: 

        LedClient();
     
        //Tells user the previous state of the LED pin. 
        void getCurrentState();

        //Makes call request to service to set a particular gpio pin to the requested state
        void setLedState(int led_gpio, int state);

        //Verifies if service successfully set the pin on the pi to HIGH/LOW
        void VerifyLedStatus(rclcpp::Client<service_interface::srv::LedState>::SharedFuture future); 

    private:


        void UserInputHandler(const std::vector<rclcpp::Parameter>& parameters);
        
        std::pair<int8_t, int64_t> led_states; 
        rclcpp::Client<service_interface::srv::LedState>::SharedPtr client_;
        rclcpp::TimerBase::SharedPtr timer_; 
        rclcpp::Publisher<example_interfaces::msg::String>::SharedPtr led_state_publisher;

        rclcpp::node_interfaces::PostSetParametersCallbackHandle::SharedPtr param_cb_handle; 


};

} //namespace LedClient
#endif