#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <OpenGL/OpenGL.h>

#include "grid.h"
#include "common.h"
#include "callback.h"

static std::random_device rd;
static mt19937 rng(rd()); // random number generator in C++11

Grid grid;

// starts a smoke at a random location
void randomize_grid(Grid &grid, int num_speckle = 3, int size = 5) {
    uni_dis dis_x(0, NUMCOL - size); // uniform distribution in C++11
    uni_dis dis_y(0, NUMROW - size); // uniform distribution in C++11
    uni_dis dis_density(25, 75); // uniform distribution in C++11
    uni_dis dis_size(1, size);
    while (num_speckle--) {
        int chosen_x = dis_x(rng);
        int chosen_y = dis_y(rng);
        int chosen_size = dis_size(rng);
        double chosen_density = dis_density(rng);
        for (int i = 0; i < chosen_size; ++i) {
            for (int j = 0; j < chosen_size; ++j) {
                grid.setDensity(chosen_x + i, chosen_y + j, grid.getDensity(chosen_x + i, chosen_y + j) + chosen_density);
            }
        }
    }
}

void display(const Grid &grid) {
    glClear(GL_COLOR_BUFFER_BIT);
    double width = 1 / (double) NUMCOL * 2;
    double height = 1 / (double) NUMROW * 2;

    for (int y = 0; y < NUMROW; ++y) {
        for (int x = 0; x < NUMCOL; ++x) {
            glColor3d(grid.getDensity(x, y) / 100, grid.getDensity(x, y) / 100, grid.getDensity(x, y) / 100);

            glBegin(GL_QUADS);
            double bottom_left_x = -1 + width * x;
            double bottom_left_y = -1 + height * y;

            glVertex2d(bottom_left_x, bottom_left_y);
            glVertex2d(bottom_left_x + width, bottom_left_y);
            glVertex2d(bottom_left_x + width, bottom_left_y + height);
            glVertex2d(bottom_left_x, bottom_left_y + height);
            glEnd();
        }
    }
    glEnd();
    glFlush();
}

int main() {
    grid = Grid(NUMCOL, NUMROW);

    GLFWwindow *window;
    // Initialize
    if (!glfwInit()) {
        return -1;
    }
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Smoke Simulation", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // To prevent screen tearing

    // Callback functions
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    auto last_time = steady_clock::now();
    while (!glfwWindowShouldClose(window)) {
        auto cur_time = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(cur_time - last_time);

        if (FREQ * elapsed.count() >= 1000) {
            //std::cout << "Update the grid" << std::endl;
            last_time = cur_time;
            grid.simulate(1);
            randomize_grid(grid, 1, 15);
            //grid.printGrid();
        }
        display(grid);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}