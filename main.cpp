#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>

#include "minigui.h"
#include "types.h" // Needed for AppState definition if not in minigui.h (it's in types.h)

// Define global state
AppState appState;

// Define global state (still needed for app logic? minigui.cpp has its own extern decl)
// AppState appState; // Only one definition allowed. It is below. 

int main() {
    // 1. Setup Data/State
    appState.searchBox.value = "";
    appState.notesBox.value = "Type here...";
    
    // 2. Bind Events
    bind_click("dashboard_btn", [](){ std::cout << "Navigating to Dashboard..." << std::endl; });
    bind_click("analytics_btn", [](){ std::cout << "Navigating to Analytics..." << std::endl; });
    bind_click("settings_btn", [](){ std::cout << "Opening Settings..." << std::endl; });
    bind_click("logout_btn", [](){ std::cout << "Logging out..." << std::endl; exit(0); });
    bind_click("search_btn", [](){ 
        std::cout << "Searching for: " << appState.notesBox.value << std::endl; // notesBox is reused as textfield
    });

    // 3. Define Layout
    std::string xmlLayout = R"(
        <Page backgroundColor="#ffffff">
            <Row width="100%" height="100%">
                
                <!-- Sidebar (25%) -->
                <Column width="25%" height="100%" backgroundColor="#1e272e" alignItems="center">
                    <View width="100%" height="40px" />
                    <Text fontSize="32px" color="#ecf0f1" fontWeight="bold">Mini-GUI</Text>
                    <View width="100%" height="50px" />
                    
                   
                </Column>
 <Button width="85%" height="50px" backgroundColor="transparent" color="#ffffff" fontSize="18px" onClick="dashboard_btn">Dashboard</Button>
                    <View width="100%" height="5px" />
                    <Button width="85%" height="50px" backgroundColor="transparent" color="#ffffff" fontSize="18px" onClick="analytics_btn">Analytics</Button>
                    <View width="100%" height="5px" />
                    <Button width="85%" height="50px" backgroundColor="transparent" color="#ffffff" fontSize="18px" onClick="settings_btn">Settings</Button>
                    <View width="100%" height="5px" />
                    <Button width="85%" height="50px" backgroundColor="#ff4848ff" color="#ffffff" fontSize="18px" onClick="logout_btn">Logout</Button>
                <!-- Main Content (75%) -->
                <Column width="75%" height="100%" backgroundColor="#f5f7fa">
                     <Scrollview width="100%" height="100%">
                        <Column width="100%" alignItems="center">
                            
                            <!-- Header -->
                            <Row width="95%" height="90px" justifyContent="between" alignItems="center" backgroundColor="#ffffff" shadow="true" borderRadius="0px" padding="10px">
                                <!-- Search -->
                                <Row width="60%" alignItems="center">
                                    <View width="20px"/>
                                    <Textfield width="350px" height="50px" fontSize="16px" />
                                    <View width="15px"/>
                                    <Button width="120px" height="50px" backgroundColor="#3498db" fontSize="16px" onClick="search_btn">SEARCH</Button>
                                </Row>
                                
                                <!-- User -->
                                <Row width="30%" justifyContent="end" alignItems="center">
                                    <Text fontSize="18px" color="#2c3e50">Admin User</Text>
                                    <View width="20px"/>
                                    <Image src="logo.png" width="50" height="50" />
                                    <View width="20px"/>
                                </Row>
                            </Row>

                            <View width="100%" height="40px" />

                            <!-- Dashboard Stats -->
                            <Column width="92%" alignItems="left">
                                <Text fontSize="32px" color="#2c3e50" fontWeight="bold">Overview</Text>
                                <View width="100%" height="25px" />
                                
                                <Row width="100%" justifyContent="between" height="200px">
                                    <Column width="31%" backgroundColor="#ffffff" borderRadius="15px" shadow="true" padding="25px" alignItems="center">
                                        <Text fontSize="18px" color="#7f8c8d">Total Revenue</Text>
                                        <Text fontSize="42px" color="#27ae60" fontWeight="bold">$54,230</Text>
                                        <Text fontSize="16px" color="#2ecc71">+12% this week</Text>
                                    </Column>
                                    <Column width="31%" backgroundColor="#ffffff" borderRadius="15px" shadow="true" padding="25px" alignItems="center">
                                        <Text fontSize="18px" color="#7f8c8d">Active Users</Text>
                                        <Text fontSize="42px" color="#2980b9" fontWeight="bold">1,204</Text>
                                        <Text fontSize="16px" color="#3498db">+5% new signups</Text>
                                    </Column>
                                    <Column width="31%" backgroundColor="#ffffff" borderRadius="15px" shadow="true" padding="25px" alignItems="center">
                                        <Text fontSize="18px" color="#7f8c8d">Bounce Rate</Text>
                                        <Text fontSize="42px" color="#e67e22" fontWeight="bold">42%</Text>
                                        <Text fontSize="16px" color="#f1c40f">Stable</Text>
                                    </Column>
                                </Row>

                                <View width="100%" height="50px" />
                                <Text fontSize="32px" color="#2c3e50" fontWeight="bold">Recent Transactions</Text>
                                <View width="100%" height="25px" />

                                <!-- Table -->
                                <Column width="100%" backgroundColor="#ffffff" borderRadius="10px" shadow="true" padding="10px">
                                    <Row width="100%" height="60px" alignItems="center" backgroundColor="#f8f9fa" borderRadius="5px">
                                        <View width="20px"/>
                                        <Text fontSize="18px" color="#7f8c8d" fontWeight="bold">ID</Text>
                                        <View width="120px"/> <Text fontSize="18px" color="#7f8c8d" fontWeight="bold">Description</Text>
                                        <View width="320px"/> <Text fontSize="18px" color="#7f8c8d" fontWeight="bold">Status</Text>
                                    </Row>
                                    
                                    <Row width="100%" height="70px" alignItems="center">
                                        <View width="20px"/>
                                        <Text fontSize="18px" color="#2c3e50">#001</Text>
                                        <View width="120px"/> <Text fontSize="18px" color="#2c3e50">Premium Subscription</Text>
                                        <View width="265px"/> <Text fontSize="16px" color="#27ae60">COMPLETED</Text>
                                    </Row>
                                    <Row width="100%" height="70px" alignItems="center">
                                        <View width="20px"/>
                                        <Text fontSize="18px" color="#2c3e50">#002</Text>
                                        <View width="120px"/> <Text fontSize="18px" color="#2c3e50">Refund Request</Text>
                                        <View width="310px"/> <Text fontSize="16px" color="#e67e22">PENDING</Text>
                                    </Row>
                                    <Row width="100%" height="70px" alignItems="center">
                                        <View width="20px"/>
                                        <Text fontSize="18px" color="#2c3e50">#003</Text>
                                        <View width="120px"/> <Text fontSize="18px" color="#2c3e50">Onboarding</Text>
                                        <View width="345px"/> <Text fontSize="16px" color="#2980b9">IN PROGRESS</Text>
                                    </Row>
                                </Column>
                                <View width="100%" height="100px" />
                            </Column>
                        </Column>
                     </Scrollview>
                </Column>
            </Row>
        </Page>
    )";

    // 4. Run GUI
    render_gui(xmlLayout);

    return 0;
}
