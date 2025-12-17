#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <iostream>
#include <string>

#include "types.h"
#include "renderer.h"
#include "widgets.h"
#include "parser.h"
#include "layout.h"

// Define global state
AppState appState;

// --- Callbacks ---

void char_callback(GLFWwindow* window, unsigned int c) {
    if (appState.activeTextbox && c >= 32 && c <= 126) {
        appState.activeTextbox->value.push_back((char)c);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_BACKSPACE && appState.activeTextbox) {
            if (!appState.activeTextbox->value.empty()) {
                appState.activeTextbox->value.pop_back();
            }
        }
    }
}

// --- Main ---

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Mini-GUI Dashboard", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCharCallback(window, char_callback);
    glfwSetKeyCallback(window, key_callback);
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Load Assets
    init_freetype();
    unsigned int logoTex = load_image("logo.png");
    if (!logoTex) std::cout << "Warning: logo.png not found" << std::endl;

    appState.searchBox.value = "";
    appState.notesBox.value = "Type here...";

    bool mouseClicked = false;
    
    // --- Define Layout via XML ---
    // Dashboard Layout with Sidebar
    std::string xmlLayout = R"(
        <Page backgroundColor="#ffffff">
            <Row width="100%" height="100%">
                
                <!-- Sidebar (20%) -->
                <Column width="20%" height="100%" backgroundColor="#2c3e50" alignItems="center">
                    <View width="100%" height="30px" />
                    <Text fontSize="24px" color="#ecf0f1" fontWeight="bold">Mini-GUI</Text>
                    <View width="100%" height="40px" />
                    
                    <!-- Menu Items -->
                    <Button width="80%" height="45px">Dashboard</Button>
                    <View width="100%" height="10px" />
                    <Button width="80%" height="45px">Analytics</Button>
                    <View width="100%" height="10px" />
                    <Button width="80%" height="45px">Settings</Button>
                    <View width="100%" height="10px" />
                    <Button width="80%" height="45px">Logout</Button>
                </Column>

                <!-- Main Content (80%) -->
                <Column width="80%" height="100%" backgroundColor="#f5f7fa">
                     <Scrollview width="100%" height="100%">
                        <Column width="100%" alignItems="center">
                            
                            <!-- Header -->
                            <Row width="95%" height="80px" justifyContent="between" alignItems="center" backgroundColor="#ffffff">
                                <!-- Search Container -->
                                <Row width="60%" alignItems="center">
                                    <View width="20px"/>
                                    <Textfield width="300px" height="40px" />
                                    <View width="10px"/>
                                    <Button width="100px" height="40px">Search</Button>
                                </Row>
                                
                                <!-- User Info -->
                                <Row width="30%" justifyContent="end" alignItems="center">
                                    <Text fontSize="16px" color="#2c3e50">Admin User</Text>
                                    <View width="20px"/>
                                    <Image src="logo.png" width="40" height="40" />
                                    <View width="20px"/>
                                </Row>
                            </Row>

                            <View width="100%" height="2px" backgroundColor="#ecf0f1" />
                            <View width="100%" height="30px" />

                            <!-- Dashboard Stats -->
                            <Column width="95%" alignItems="left">
                                <Text fontSize="26px" color="#34495e" fontWeight="bold">Overview</Text>
                                <View width="100%" height="20px" />
                                
                                <Row width="100%" justifyContent="between" height="160px">
                                    <View width="32%" height="150px" backgroundColor="#ffffff">
                                        <Column width="100%" alignItems="center">
                                            <View width="100%" height="20px"/>
                                            <Text fontSize="16px" color="#7f8c8d">Total Revenue</Text>
                                            <Text fontSize="36px" color="#27ae60">$54,230</Text>
                                            <Text fontSize="14px" color="#2ecc71">+12% this week</Text>
                                        </Column>
                                    </View>
                                    <View width="32%" height="150px" backgroundColor="#ffffff">
                                        <Column width="100%" alignItems="center">
                                            <View width="100%" height="20px"/>
                                            <Text fontSize="16px" color="#7f8c8d">Active Users</Text>
                                            <Text fontSize="36px" color="#2980b9">1,204</Text>
                                            <Text fontSize="14px" color="#3498db">+5% new signups</Text>
                                        </Column>
                                    </View>
                                    <View width="32%" height="150px" backgroundColor="#ffffff">
                                        <Column width="100%" alignItems="center">
                                             <View width="100%" height="20px"/>
                                             <Text fontSize="16px" color="#7f8c8d">Bounce Rate</Text>
                                             <Text fontSize="36px" color="#e67e22">42%</Text>
                                             <Text fontSize="14px" color="#f1c40f">Stable</Text>
                                        </Column>
                                    </View>
                                </Row>

                                <View width="100%" height="40px" />
                                <Text fontSize="26px" color="#34495e" fontWeight="bold">Recent Transactions</Text>
                                <View width="100%" height="20px" />

                                <!-- Table -->
                                <Column width="100%" backgroundColor="#ffffff">
                                    <Row width="100%" height="50px" alignItems="center" backgroundColor="#ecf0f1">
                                        <View width="20px"/>
                                        <Text fontSize="16px" color="#7f8c8d" fontWeight="bold">ID</Text>
                                        <View width="100px"/> <Text fontSize="16px" color="#7f8c8d" fontWeight="bold">Description</Text>
                                        <View width="300px"/> <Text fontSize="16px" color="#7f8c8d" fontWeight="bold">Status</Text>
                                    </Row>
                                    
                                    <Row width="100%" height="60px" alignItems="center">
                                        <View width="20px"/>
                                        <Text fontSize="16px" color="#2c3e50">#001</Text>
                                        <View width="100px"/> <Text fontSize="16px" color="#2c3e50">Premium Subscription</Text>
                                        <View width="245px"/> <Text fontSize="14px" color="#27ae60">COMPLETED</Text>
                                    </Row>
                                    <View width="100%" height="1px" backgroundColor="#ecf0f1"/>

                                    <Row width="100%" height="60px" alignItems="center">
                                        <View width="20px"/>
                                        <Text fontSize="16px" color="#2c3e50">#002</Text>
                                        <View width="100px"/> <Text fontSize="16px" color="#2c3e50">Refund Request</Text>
                                        <View width="290px"/> <Text fontSize="14px" color="#e67e22">PENDING</Text>
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

    // Layout parsing wrapped in try-catch
    Node root;
    try {
        XmlParser parser(xmlLayout);
        root = parser.parse();
    } catch (const std::exception& e) {
        std::cerr << "XML Parsing Error: " << e.what() << std::endl;
        return -1;
    }

    glfwSetScrollCallback(window, [](GLFWwindow* w, double xoffset, double yoffset) {
        appState.scrollOffset -= yoffset * 20;
        if (appState.scrollOffset < 0) appState.scrollOffset = 0;
        if (appState.scrollOffset > appState.maxScroll) appState.scrollOffset = appState.maxScroll;
    });

    while (!glfwWindowShouldClose(window)) {
        try {
            // Handle Retina Display Scaling for Mouse
            int winW, winH;
            glfwGetWindowSize(window, &winW, &winH);
            int fbW, fbH;
            glfwGetFramebufferSize(window, &fbW, &fbH);
            
            // Update viewport if resized
            glViewport(0, 0, fbW, fbH);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, fbW, fbH, 0, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            double mx, my;
            glfwGetCursorPos(window, &mx, &my);
            
            // Scale mouse coordinates to framebuffer size
            float scaleX = (float)fbW / (float)winW;
            float scaleY = (float)fbH / (float)winH;
            mx *= scaleX;
            my *= scaleY;

            int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
            bool click = (state == GLFW_PRESS && !mouseClicked);
            mouseClicked = (state == GLFW_PRESS);

            // Clear Screen - White
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Render the XML layout
            float drawW = (float)fbW;
            float drawH = (float)fbH;
            float startY = 0.0f;
            
            // std::cout << "FB: " << fbW << "x" << fbH << std::endl; // verify size

            render_node(root, 0, startY, mx, my, click, drawW, drawH);

            // Swap
            glfwSwapBuffers(window);
            glfwPollEvents();
        } catch (const std::exception& e) {
            std::cerr << "Runtime Error: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown Error occurred in main loop" << std::endl;
        }
    }

    glfwTerminate();
    return 0;
}
