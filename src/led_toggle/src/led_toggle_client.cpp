#include "led_toggle/led_toggle_client.hpp"
#include <chrono>
#include <thread>


LedClient::LedClient::LedClient() : rclcpp::Node("led_toggle_client"), led_states(17, 0){

    client_ = this->create_client<service_interface::srv::LedState>("request_led_activation");
    led_state_publisher = this->create_publisher<service_interface::msg::LedStatus>("/toggle_cmd", rclcpp::SystemDefaultsQoS());
    timer_ = this->create_wall_timer(std::chrono::seconds(1), std::bind(&LedClient::LedClient::getCurrentState, this)); 

    while(!client_->wait_for_service(std::chrono::seconds(3))){
        RCLCPP_WARN(this->get_logger(), "Waiting for Service server to be up...");
    }
    RCLCPP_INFO(this->get_logger(), "Led State Client node has been started");
}

//getCurrentState checks the current status of the LED light, wheather it is on/off, and which GPIO pin is active. 
void LedClient::LedClient::getCurrentState(){
 
    if(led_states.second == 0){

        RCLCPP_INFO(this->get_logger(), "The Led on the PI is currently inactive"); 
        setLedState(17, 1);

    }else{
        RCLCPP_INFO(this->get_logger(), "The Led on the PI is currently active!");
    }
    
}

void LedClient::LedClient::setLedState(int led_gpio, int state){
    
    auto request = std::make_shared<service_interface::srv::LedState::Request>();
    
    request->state = state; 
    request->led_gpio = led_gpio; 

    auto future = client_->async_send_request(request, std::bind(&LedClient::VerifyLedStatus, this, std::placeholders::_1));
}

void LedClient::LedClient::VerifyLedStatus(rclcpp::Client<service_interface::srv::LedState>::SharedFuture future){
    
    
    auto response = future.get(); 

    RCLCPP_INFO(this->get_logger(), "%s", response->message.c_str()); 

    if(!response->success){
        RCLCPP_INFO(this->get_logger(), "%s", response->message.c_str());
    }

    led_states.second = response->success;
    
}


int main(int argc, char** argv){

    rclcpp::init(argc, argv);
    auto node = std::make_shared<LedClient::LedClient>(); 
    rclcpp::spin(node);
    rclcpp::shutdown();
}