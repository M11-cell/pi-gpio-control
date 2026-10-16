#ifndef LED_TOGGLE_CLIENT_HPP
#define LED_TOGGLE_CLIENT_HPP

#include "rclcpp/rclcpp.hpp"

#include <vector>
#include <memory>
#include <cstdint>

#include "service_interface/srv/led_state.hpp"
#include "service_interface/msg/led_status.hpp"



class LedClient: public rclcpp::Node{

    public: 

        LedClient();
     
        
        private:

            using LedStateSrv = service_interface::srv::LedState; 
            using LedStatusMsg = service_interface::msg::LedStatus;

            struct LedConfig{

                int gpio_number{0};
                bool led_on{false}; 
            };

            LedConfig led_; 
            
            rcl_interfaces::msg::SetParametersResult ValidateUserInput(const std::vector<rclcpp::Parameter>& parameters);

            void UserInputHandler(const std::vector<rclcpp::Parameter>& parameters);

            //Makes call request to service to set a particular gpio pin to the requested state
            void SendLedRequest(const LedConfig&  params);
        
            //Publishes Led Status
            void HandlePublishedLedStates(const LedStatusMsg::SharedPtr msg);

        
        
            rclcpp::Client<LedStateSrv>::SharedPtr client_;
            rclcpp::Subscription<LedStatusMsg>::SharedPtr led_status_pub_;

            rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr on_set_handle_; 
            rclcpp::node_interfaces::PostSetParametersCallbackHandle::SharedPtr post_set_handle_; 


};

#endif