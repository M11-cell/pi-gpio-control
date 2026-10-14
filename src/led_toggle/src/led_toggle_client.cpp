#include "led_toggle/led_toggle_client.hpp"
#include <chrono>
#include <thread>


LedClient::LedClient::LedClient() : rclcpp::Node("led_toggle_client"){

    
    this->declare_parameter<int8_t>("gpio_nunmber", 0); 
    this->declare_parameter<int64_t>("led_state", 0);

    led_states.first = this->get_parameter("gpio_nunmber").as_int();
    led_states.second = this->get_parameter("led_state").as_int(); 

    param_cb_handle = this->add_post_set_parameters_callback(std::bind(&LedClient::LedClient::UserInputHandler, this, std::placeholders::_1));
    client_ = this->create_client<service_interface::srv::LedState>("request_led_activation");
    led_state_publisher = this->create_publisher<example_interfaces::msg::String>("/toggle_cmd", 100);
    timer_ = this->create_wall_timer(std::chrono::seconds(1), std::bind(&LedClient::LedClient::getCurrentState, this)); 


    while(!client_->wait_for_service(std::chrono::seconds(3))){
        RCLCPP_WARN(this->get_logger(), "Waiting for Service server to be up...");
    }
    RCLCPP_INFO(this->get_logger(), "Led State Client node has been started");
}

void LedClient::LedClient::UserInputHandler(const std::vector<rclcpp::Parameter>& parameters){

    for(const auto &param: parameters){
        if(param.get_name() == "led_states.first"){
            led_states.first = param.as_int(); 
            RCLCPP_INFO(this->get_logger(), 
                "Set %s to %s",
                param.get_name().c_str(), 
                param.as_int()
            );

        }else if(param.get_name() == "led_states.second"){
            led_states.second = param.as_int();
            RCLCPP_INFO(this->get_logger(), 
                "Set %s to %s",
                param.get_name().c_str(), 
                param.as_int()
            );
        }else{
            RCLCPP_WARN(
                this->get_logger(), 
                "Unknown parameter input: %s",
                param.get_name().c_str()
            );
        }
    }

    setLedState(led_states.first, led_states.second); 
}

//getCurrentState checks the current status of the LED light, wheather it is on/off, and which GPIO pin is active. 
void LedClient::LedClient::getCurrentState(){

    auto led_status = example_interfaces::msg::String(); 
    

    if(led_states.second < 0){
        RCLCPP_WARN(this->get_logger(), "The status of the led is currently low. to turn it on, please input the gpio pin the led is connected to and enter 1");
        led_status.data = "LOW";
    }else{
        led_status.data = "HIGH";
    }

    led_state_publisher->publish(led_status); 
    RCLCPP_INFO(this->get_logger(), "Led status is currently: %s", led_status.data.c_str()); 
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