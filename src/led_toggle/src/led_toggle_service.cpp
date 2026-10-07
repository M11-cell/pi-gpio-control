#include "led_toggle/led_toggle_service.hpp"


LedService::LedServer::LedServer(): Node("led_toggle_server"){

    server_ = this->create_service<service_interface::srv::LedState>("set_led", 
        std::bind(&LedService::LedServer::callbackSetLEDState, this, std::placeholders::_1, std::placeholders::_2)); 

    timer_  = this->create_wall_timer(std::chrono::seconds(5), std::bind(&LedService::LedServer::updateLEDStatus, this));

    led_status_publisher_ = this->create_publisher<service_interface::msg::LedStatus>("/led_status", 1000);

    wiringPiSetup();

    RCLCPP_INFO(this->get_logger(), "led service initialized successfully... "); 
}


void LedService::LedServer::updateLEDStatus(){

}

void LedService::LedServer::callbackSetLEDState(const service_interface::srv::LedState::Request::SharedPtr request,
                                            const service_interface::srv::LedState::Response::SharedPtr response)
{

    int64_t led_gpio = request->led_gpio; 
    int64_t led_state = request->state;

    if(led_state > 0 || led_gpio != -1){
        digitalWrite(led_gpio, led_state); 
        response->success = true;
        response->message = "The state of the LED is HIGH!"; 
        return; 
    }else{
        response->success = false; 
        response->message = "The state of the LED is currently LOW, to turn it on, specify the GPIO pin you want to activate and switch it to HIGH";
        return; 
    }

    response->success = false; 

}


int main(int argc, char** argv){

    rclcpp::init(argc, argv);
    auto node = std::make_shared<LedService::LedServer>();
    rclcpp::spin(node); 
    rclcpp::shutdown();


}