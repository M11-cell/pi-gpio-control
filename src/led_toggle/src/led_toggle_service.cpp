#include "led_toggle/led_toggle_service.hpp"


LedServer::LedServer() : Node("led_toggle_server"){

    if(wiringPiSetupGpio() == -1){
        throw std::runtime_error("GPIO initialization failed (permissions?)");
    }
    
    allowed_pins = {17, 18, 22, 23, 24, 25, 26, 27};
    for(const int pin: allowed_pins){
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        pin_state_[pin] = false; 
    }

    server_ = this->create_service<LedStateSrv>("set_led", std::bind(&LedServer::HandleSetLed, this, std::placeholders::_1, std::placeholders::_2));

    led_status_pub_ = this->create_publisher<LedStateMsg>("/led_status", rclcpp::SystemDefaultsQoS());

    RCLCPP_INFO(this->get_logger(), "LED server side is ready");
}; 

LedServer::~LedServer(){

    for(const int pins : allowed_pins){
        digitalWrite(pins, LOW); 
        pin_state_[pins] = false; 
    }

    RCLCPP_INFO(this->get_logger(), "Safely shut down hardware upon server shutdown"); 
}

void LedServer::HandleSetLed(
    const std::shared_ptr<LedStateSrv::Request> request,
    const std::shared_ptr<LedStateSrv::Response> response){

        const auto gpio = request->led_gpio;
        const auto state = request->state;

        if(allowed_pins.count(gpio) == 0){
            response->success = false; 
            response->message = "GPIO" + std::to_string(gpio) + "is not an allowed pin";
            RCLCPP_WARN(this->get_logger(), "%s", response->message.c_str());
            return; 
        }if(state != 0 && state != 1){
            response->success = false;
            response->message = "State must be 0 or 1, got" + std::to_string(state); 
            RCLCPP_WARN(this->get_logger(), "%s", response->message.c_str());
            return; 
        }

        const bool on = (state ==1); 
        digitalWrite(gpio, on ? HIGH : LOW);
        
        response->success = true; 
        response->message = "Successfully set LED on gpio pin " + std::to_string(gpio) + "to" + (on ? "HIGH" : "LOW"); 
        RCLCPP_INFO(this->get_logger(), "%s", response->message.c_str()); 

        PublishStatus(gpio, on); 
}

void LedServer::PublishStatus(int gpio, bool state){

    LedStateMsg msg; 

    msg.gpio_selected = gpio;
    msg.state = state; 

    led_status_pub_->publish(msg); 
}

int main(int argc, char** argv){

    rclcpp::init(argc, argv);
    try{
        auto node = std::make_shared<LedServer>();
        rclcpp::spin(node); 
    } catch (const std::exception& e){
        RCLCPP_FATAL(rclcpp::get_logger("led_toggle_server"), "%s", e.what());
        rclcpp::shutdown();
        return 1;
    }

    rclcpp::shutdown(); 
    return 0;
}