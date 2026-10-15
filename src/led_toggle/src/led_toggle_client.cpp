#include "led_toggle/led_toggle_client.hpp"
#include <algorithm>


static constexpr auto kGpioParam = "gpio_number";
static constexpr auto kLedStateParam = "led_state"; 


LedClient::LedClient() : rclcpp::Node("led_toggle_client"){

    
    this->declare_parameter<int64_t>(kGpioParam, 0); 
    this->declare_parameter<bool>(kLedStateParam, false);   
    led_.gpio_number = this->get_parameter(kGpioParam).as_int(); 
    led_.led_on = this->get_parameter(kLedStateParam).as_bool(); 

    on_set_handle_ = this->add_on_set_parameters_callback(std::bind(&LedClient::ValidateUserInput, this, std::placeholders::_1));
    post_set_handle_ = this->add_post_set_parameters_callback(std::bind(&LedClient::UserInputHandler, this, std::placeholders::_1));


    client_ = this->create_client<LedStateSrv>("request_led_state_activation");
    led_status_pub_ = this->create_publisher<LedStatusMsg>("/led_status", 100);

    
    RCLCPP_INFO(this->get_logger(), "Led State Client node has been started");
}

rcl_interfaces::msg::SetParametersResult LedClient::ValidateUserInput(const std::vector<rclcpp::Parameter>& parameters){

    rcl_interfaces::msg::SetParametersResult result; 
    result.successful = true;

    for(const auto& p : parameters){

        if(p.get_name() == kGpioParam && (p.as_int() < 0 || p.as_int() > 27)){
            result.successful = false; 
            result.reason = "gpio number MUST be in between 0 and 27!";
            return result; 
        }
    }

    return result; 
}

void LedClient::UserInputHandler(const std::vector<rclcpp::Parameter>& parameters){

    //**Note: Since the kLedStateParam is a GLOBAL constant, the lambda does NOT require a capture clause! 
    const bool state_touched = std::any_of(
        parameters.begin(), parameters.end(),
        [](const rclcpp::Parameter& p){return p.get_name() == kLedStateParam; });

    if(!state_touched){
        RCLCPP_WARN(this->get_logger(), "Warning! You must choose to set a state (High/Low) of the gpio you have configured the LED to.");
        return; 
    }

    LedConfig params{

        static_cast<int>(get_parameter(kGpioParam).as_int()),
        get_parameter(kLedStateParam).as_bool()};

    RCLCPP_INFO(this->get_logger(), "Requesting pin %d to be set to %s", params.gpio_number, (params.led_on ? "HIGH" : "LOW")); 
    SendLedRequest(params);


}


void LedClient::SendLedRequest(const LedConfig&  params){

    if(!client_->service_is_ready()){
        RCLCPP_WARN(this->get_logger(), "The service is not yet available; request dropped");
        return; 
    }

    auto request = std::make_shared<LedStateSrv::Request>();

    request->led_gpio = params.gpio_number; 
    request->state = params.led_on;

    client_->async_send_request(request, 
        [this, params](rclcpp::Client<LedStateSrv>::SharedFuture future){
            const auto response = future.get(); 
            if(!response->success){
                RCLCPP_WARN(this->get_logger(), "The request was unsuccessful for the following reason: %s", response->message.c_str());
                return; 
            }
            
            led_ = {params.gpio_number, params.led_on}; 
            PublishLedStates(); 
        }
    );

}

void LedClient::PublishLedStates(){

    auto msg = LedStatusMsg();   
    msg.gpio_selected = led_.gpio_number; 
    msg.state = led_.led_on; 

    const char* state = msg.state ? "HIGH" : "LOW"; 
    
    led_status_pub_->publish(msg); 
    RCLCPP_INFO(this->get_logger(), "Currently, the LED is connected to GPIO %d, and is set to %s", static_cast<int>(msg.gpio_selected), state);


}



int main(int argc, char** argv){

    rclcpp::init(argc, argv);
    auto node = std::make_shared<LedClient>(); 
    rclcpp::spin(node);
    rclcpp::shutdown();
}