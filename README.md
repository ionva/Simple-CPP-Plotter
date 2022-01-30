# Simple-CPP-Plotter

This is a free to use, open-source single header file plotting library for C++ based on SFML.

## Installation
It is required that SFML 2.0 is setup and configured to link with your project.
Next, simply download the header file from this repo and put #include "dir/SimplePlotter.h"

## Tutorial

One can create a plot object by declaring it using the Plot class name, as follows:

> Plot plot;

Then the member functions:
> plot.plot(vector_of_x_vals, vector_of_y_vals)

and

> plot.show()

to start a new thread which displays the graph.

Additionally, 

>  plot.set_plot_parameters(params)

can be used in order to change things such as edge margins.
