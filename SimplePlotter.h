#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include <thread>
#include <iostream>
#include <Windows.h>
#include <iomanip>
#include <sstream>

extern std::atomic_bool stop_threads = false;

std::string float_to_sf_string(float f, int nd)
{
    if (f < 1) //num less than 1
    {
        std::string num_as_string = std::to_string(f);
        std::string sig_figs = num_as_string.substr(0, 2 + nd);
        int num_sig_figs = sig_figs.length();
        if (num_sig_figs < nd)
        {
            for (int i = 0; i < nd - num_sig_figs; i++) sig_figs += "0";
        }

        return sig_figs;
    }

    std::string num_as_string = std::to_string(f);

    int period_location_or_end = num_as_string.find(".");
    if (period_location_or_end == num_as_string.size()) period_location_or_end -= 1;

    std::string digits_before_period = num_as_string.substr(0, period_location_or_end);

    if (digits_before_period.length() >= nd)
    {
        std::string sig_figs = digits_before_period.substr(0, nd);
        for (int i = 0; i < digits_before_period.length() - nd; i++) sig_figs += "0";
        return sig_figs;
    }

    int sig_fig_num_after_period = nd - digits_before_period.size();

    std::string digits_after_period = num_as_string.substr(period_location_or_end + 1, digits_before_period.size());


    std::string sig_figs = num_as_string.substr(0, period_location_or_end + sig_fig_num_after_period + 1);



    for (int i = 0; i < nd - (int)digits_after_period.length() - (int)digits_before_period.length() - 1; i++) sig_figs += "0";

    return sig_figs;
}
struct vec2f

{
    double x;
    double y;
    vec2f(double x = 0, double y = 0)
    {
        this->x = x;
        this->y = y;
    }
};

class sfLine : public sf::Drawable
{
public:
    sfLine(const sf::Vector2f& point1, const sf::Vector2f& point2, sf::Color color, float thickness)
    {
        this->thickness = thickness;
        this->color = color;

        sf::Vector2f direction = point2 - point1;
        sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

        sf::Vector2f offset = (thickness / 2.f) * unitPerpendicular;

        vertices[0].position = point1 + offset;
        vertices[1].position = point2 + offset;
        vertices[2].position = point2 - offset;
        vertices[3].position = point1 - offset;

        for (int i = 0; i < 4; ++i)
            vertices[i].color = color;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(vertices, 4, sf::Quads);
    }
    std::string float_to_sf_string(float f, int nd)
    {
        if (f < 1) //num less than 1
        {
            std::string num_as_string = std::to_string(f);
            std::string sig_figs = num_as_string.substr(0, 2 + nd);
            int num_sig_figs = sig_figs.length();
            if (num_sig_figs < nd)
            {
                for (int i = 0; i < nd - num_sig_figs; i++) sig_figs += "0";
            }

            return sig_figs;
        }

        std::string num_as_string = std::to_string(f);

        int period_location_or_end = num_as_string.find(".");
        if (period_location_or_end == num_as_string.size()) period_location_or_end -= 1;

        std::string digits_before_period = num_as_string.substr(0, period_location_or_end);

        if (digits_before_period.length() >= nd)
        {
            std::string sig_figs = digits_before_period.substr(0, nd);
            for (int i = 0; i < digits_before_period.length() - nd; i++) sig_figs += "0";
            return sig_figs;
        }

        int sig_fig_num_after_period = nd - digits_before_period.size();

        std::string digits_after_period = num_as_string.substr(period_location_or_end + 1, digits_before_period.size());

        std::string sig_figs = num_as_string.substr(0, period_location_or_end + sig_fig_num_after_period + 1);



        for (int i = 0; i < (int)digits_after_period.length() - (int)digits_before_period.length(); i++); sig_figs += "0";

        return sig_figs;
    }
private:




    sf::Vertex vertices[4];
    float thickness;
    sf::Color color;
};

class Plot
{
public:
    std::vector<std::vector<vec2f>> point_list_by_catagory;
    double padding_ratio_x;
    double padding_ratio_y;
    int window_width;
    int window_height;
    double point_size;
    double connecting_line_thickness;
    double axis_thickness;

    Plot()
    {
        padding_ratio_x = 1.4;
        padding_ratio_y = 1.4;
        window_width = 3840;
        window_height = 2160;
        point_size = 0;
        connecting_line_thickness = 3;
        axis_thickness = 4;
    }

private:

    void show_plot()
    {
        if (point_list_by_catagory.size() == 0)
        {
            std::cout << "CANNOT PLOT DATA WITH LENGTH ZERO" << std::endl;
            exit(-1);
        }
        //Generate correct scale
        double max_x = -INFINITY;
        double max_y = -INFINITY;
        double min_x = INFINITY;
        double min_y = INFINITY;

        auto compute_minmax = [&max_x, &max_y, &min_x, &min_y, this]()
        {
            for (auto& graph : point_list_by_catagory)
            {
                for (auto& point : graph)
                {
                    //Max
                    if (point.x > max_x) max_x = point.x;
                    if (point.y > max_y) max_y = point.y;

                    //Min
                    if (point.x < min_x) min_x = point.x;
                    if (point.y < min_y) min_y = point.y;

                }
            }
        };

        compute_minmax();

        sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Plot");
        window.setFramerateLimit(5);
        sf::View view = window.getDefaultView();

        auto draw_graphs = [&window, this, max_x, max_y, min_x, min_y]()
        {
            auto transform_x = [min_x, max_x, this](double x)
            {
                return(((x - min_x) / ((max_x - min_x) * padding_ratio_x)) * window_width + ((padding_ratio_x - 1) / 3) * window_width);
            };

            auto transform_y = [min_y, max_y, this](double y)
            {
                return(((y - min_y) / ((max_y - min_y) * padding_ratio_y)) * window_height + ((padding_ratio_y - 1) / 3) * window_height);
            };

            window.clear(sf::Color(255, 255, 255));

            for (auto& graph : point_list_by_catagory)
            {
                vec2f prev_point = vec2f(transform_x(graph[0].x), window_height - transform_y(graph[0].y));
                for (auto& point : graph)
                {
                    int screen_space_x_position = transform_x(point.x);
                    int screen_space_y_position = transform_y(point.y);

                    sf::CircleShape point_shape(point_size);
                    point_shape.setPosition(screen_space_x_position, window_height - screen_space_y_position);
                    point_shape.setFillColor(sf::Color(0, 0, 0));
                    window.draw(point_shape);

                    if (screen_space_x_position >= 0 || window_height - screen_space_y_position >= 0)
                    {
                        sfLine connecting_line(window.mapPixelToCoords(sf::Vector2i(prev_point.x, prev_point.y)),
                            window.mapPixelToCoords(sf::Vector2i(screen_space_x_position, window_height - screen_space_y_position)),
                            sf::Color(0, 0, 0), connecting_line_thickness);

                        window.draw(connecting_line);
                    }
                    prev_point = vec2f(screen_space_x_position, window_height - screen_space_y_position);

                }
            }

            //Axes
            double x_axes = transform_x(min_x);
            double y_axes = transform_y(min_y);

            sf::RectangleShape x_axis_rect;
            x_axis_rect.setPosition(window.mapPixelToCoords(sf::Vector2i(0, window_height - y_axes)));
            x_axis_rect.setSize(sf::Vector2f(window_width, axis_thickness));
            x_axis_rect.setFillColor(sf::Color(0, 0, 0, 100));
            window.draw(x_axis_rect);

            sf::RectangleShape y_axis_rect;
            y_axis_rect.setPosition(window.mapPixelToCoords(sf::Vector2i(x_axes, 0)));
            y_axis_rect.setSize(sf::Vector2f(axis_thickness, window_height));
            y_axis_rect.setFillColor(sf::Color(0, 0, 0, 100));
            window.draw(y_axis_rect);


            //Y scale
            sf::Font font;

            if (!font.loadFromFile("C:\\Users\\Aspen\\Desktop\\SFML FONTS\\OpenSans-Light.ttf"))
            {
                std::cout << "FAILED TO LAOD FONT, EXITING..." << std::endl;
                exit(-1);
            }
            constexpr int y_labels_count = 5;

            double scale_increment = (max_y - min_y) / (float)y_labels_count;
            double current_y = min_y;

            auto draw_label_text = [&font, &current_y, &window, this, &transform_y, &x_axes]()
            {

                std::string label_string = float_to_sf_string(current_y, 3);
                sf::Text y_label;
                y_label.setFont(font);
                y_label.setCharacterSize(24);
                y_label.setFillColor(sf::Color::Black);
                y_label.setString(label_string);

                y_label.setPosition(window.mapPixelToCoords(
                    sf::Vector2i(x_axes - y_label.getLocalBounds().width - 10,
                        window_height - (transform_y(current_y) + y_label.getLocalBounds().height)))
                );

                window.draw(y_label);
            };

            draw_label_text();
            for (int i = 0; i < y_labels_count; i++)
            {
                current_y += scale_increment;
                draw_label_text();
            }


            window.display();
        };

        draw_graphs();

        while (window.isOpen())
        {
            if (stop_threads) std::terminate();
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::Resized)
                {


                    this->window_width = event.size.width;
                    this->window_height = event.size.height;
                    view.setSize({
                   static_cast<float>(event.size.width),
                   static_cast<float>(event.size.height)
                        });
                    window.setView(view);


                    draw_graphs();
                }
            }
        }
    }
public:
    void show()
    {
        std::thread t1(&Plot::show_plot, this);
        t1.detach();

    }

    void plot(std::vector<double> x, std::vector<double> y)
    {
        if (x.size() != y.size())
        {
            std::cout << "X, Y VECTOR DIMS MUST MATCH, BUT RECIEVED DIMS: " << x.size() << ", " << y.size() << " ...EXITING..." << std::endl;
            exit(-1);
        }

        std::vector<vec2f> points;

        for (int i = 0; i < x.size(); i++)
        {
            points.push_back(vec2f(x[i], y[i]));
        }
        point_list_by_catagory.push_back(points);
    }
    void plot(std::vector<double> y, int begin_index = 0, int x_shift = 0) // Y incremnted by 1
    {
        std::vector<vec2f> points;

        for (int i = begin_index; i < y.size(); i++)
        {
            points.push_back(vec2f(i + x_shift, y[i]));
        }
        point_list_by_catagory.push_back(points);
    }

    //Suggested values:
    /*
    * Padding Ratio: 1.3
    * +
    */
    void set_plot_parameters(double padding_ratio_x, double padding_ratio_y, double point_size, double connecting_line_thickness)
    {
        this->padding_ratio_x = padding_ratio_x;
        this->padding_ratio_y = padding_ratio_y;
        this->point_size = point_size;
        this->connecting_line_thickness = connecting_line_thickness;

    }


};