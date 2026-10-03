#pragma once

#ifndef LED_TOGGLE_SERVICE_HPP
#define LED_TOGGLE_SERVICE_HPP

#include "rclcpp/rclcpp.hpp"
#include "service_interface/srv/led_state.hpp"
#include <wiringPi.h>


namespace LedService{


class LedServer: public rclcpp::Node{

    public: 

        LedServer();

        void callbackSetLEDState(const service_interface::srv::LedState::Request::SharedPtr request,
                                    const service_interface::srv::LedState::Response::SharedPtr response); 

        void updateLEDState(); 

    private:

        rclcpp::Service<service_interface::srv::LedState()>::SharedPtr server_;
        rclcpp::TimerBase::SharedPtr timer_; 

};

} //namespace LedService


#endif