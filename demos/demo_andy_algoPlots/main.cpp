#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
#include <ctime>


#include <wif_core/wif_core.hpp>
#include <wif_algo/wif_algo.hpp>
#include <wif_viz/wif_viz.hpp>



int main()
{

	// CP NUMERIEK VS THEORETISCH.
	// Closed body check circle als functie van #panelen.
	// tijdsduur als functie van #panelen.
	// fout op cp als functie van #panelen.
	{
		double radius = 1;
		bool Kutta = 0;
		wif_core::vector_2d_c midpoint(0, 0);
		std::shared_ptr<wif_core::uniform_flow_c> uni_flow = std::make_shared<wif_core::uniform_flow_c>(0, 1);

		std::vector<double> cp_error;
		std::vector<double> num_lines_x_axis;
		std::vector<double> closed_body_check;
		std::vector<double> durations;

		std::clock_t start;

		for(int num_lines = 10; num_lines <= 200; num_lines += 10)
		{
			num_lines_x_axis.push_back(num_lines);

			wif_core::airfoil_c circle_airfoil(midpoint, radius, num_lines);

			start = std::clock();
			wif_algo::calculation_results_c circle_results = wif_algo::calculate_flow(circle_airfoil, uni_flow, Kutta);
			durations.push_back((std::clock() - start) / (double) CLOCKS_PER_SEC);

			closed_body_check.push_back(circle_results.closed_body_check * 1000);

			std::vector<wif_core::line_2d_c> circle_lines = circle_airfoil.get_lines();
			std::vector<wif_core::vector_2d_c> centers(num_lines);
			std::vector<double> angles(num_lines);
			std::vector<double> cp_theory(num_lines);
			std::vector<double> cp_x_axis(num_lines);

			double cp_error_accum = 0;

			for(int i = 0; i < num_lines; i++)
			{
				wif_core::line_2d_c temp_line = circle_lines[i];
				centers[i] = temp_line.get_center_point();

				wif_core::vector_2d_c temp_vector((temp_line.end.y - temp_line.begin.y), -(temp_line.end.x - temp_line.begin.x));

				angles[i] = temp_vector.get_angle();
				cp_theory[i] = 1 - 4 * pow((centers[i].y / radius), 2);
				cp_x_axis[i] = i + 1;
				cp_error_accum += pow(1 - 4 * pow((centers[i].y / radius), 2) - circle_results.c_p[i], 2);
			}

			cp_error.push_back(std::log10(cp_error_accum / num_lines));

			// CP NUMERIEK vs THEORETISCH
			std::vector<std::vector<double>> cp_plot(2, std::vector<double>(num_lines));
			cp_plot[0] = cp_theory;
			cp_plot[1] = circle_results.c_p;

			std::vector<std::string> cp_legend(2);
			cp_legend[0] = "Theoretisch";
			cp_legend[1] = "Numeriek";

			std::string cp_title = "Cp: " + std::to_string(num_lines) + " panelen";
			std::string cp_filename = "plot_cp " + std::to_string(num_lines) + ".png";

			std::shared_ptr<wif_viz::visualization_c> cp_plotter = wif_viz::create_visualization_root(uni_flow, midpoint, midpoint);
			cp_plotter->plotVectors(cp_plot, cp_x_axis, cp_legend, cp_filename, "paneel", "Cp", cp_title);
		}

		//Closed body check circle als functie van #panelen
		std::vector<std::vector<double>> closed_body_plot(1);

		for(int i = 0; i < closed_body_check.size(); i++)
		{
			closed_body_check[i] = closed_body_check[i] * 1000;
		}

		closed_body_plot[0] = closed_body_check;

		std::vector<std::string> closed_body_legend(1);
		closed_body_legend[0] = "Som sigma * l (10^3)";

		std::string closed_body_title = "Som sigma * l als functie van het aantal panelen";
		std::string closed_body_filename = "som sigma l cp.png";

		std::shared_ptr<wif_viz::visualization_c> closed_body_plotter = wif_viz::create_visualization_root(uni_flow, midpoint, midpoint);
		closed_body_plotter->plotVectors(closed_body_plot, num_lines_x_axis, closed_body_legend, closed_body_filename, "# panelen", "Som sigma *l (10^3)", closed_body_title);


		// fout op cp als functie van #panelen
		std::vector<std::vector<double>> cp_error_plot(1);
		cp_error_plot[0] = cp_error;

		std::vector<std::string> cp_error_legend(1);
		cp_error_legend[0] = "Fout";

		std::string cp_error_title = "Fout op cp als functie van het aantal panelen";
		std::string cp_error_filename = "fout op cp.png";

		std::shared_ptr<wif_viz::visualization_c> cp_error_plotter = wif_viz::create_visualization_root(uni_flow, midpoint, midpoint);
		cp_error_plotter->plotVectors(cp_error_plot, num_lines_x_axis, cp_error_legend, cp_error_filename, "# panelen", "log10(Fout op Cp)", cp_error_title);

		// tijdsduur als functie van #panelen
		std::vector<std::vector<double>> durations_plot(1);
		durations_plot[0] = durations;

		std::vector<std::string> durations_legend(1);
		durations_legend[0] = "Tijdsduur";

		std::string durations_title = "Tijdsduur als functie van het aantal panelen";
		std::string durations_filename = "tijd cp.png";

		std::shared_ptr<wif_viz::visualization_c> durations_plotter = wif_viz::create_visualization_root(uni_flow, midpoint, midpoint);
		durations_plotter->plotVectors(durations_plot, num_lines_x_axis, durations_legend, durations_filename, "# panelen", "Tijd (s)", durations_title);

	}

	// NACA PLOTS zonder Kutta.


	// CP met verschillende vorticiteit.
	{
		bool Kutta = 0;
		int num_lines = 20;
		std::shared_ptr<wif_core::uniform_flow_c> uni_flow = std::make_shared<wif_core::uniform_flow_c>(0, 1);
		wif_core::airfoil_c naca_airfoil("wif_core/airfoils/selig.dat");
		naca_airfoil = naca_airfoil.closed_merge(0.001);
		naca_airfoil = naca_airfoil.get_circle_projection(num_lines);

		int gamma_start = 0;
		int gamma_step = 5;
		int gamma_end = 15;

		std::vector<std::vector<double>> cp_gamma_plot;
		std::vector<std::string> cp_gamma_legend;

		std::vector<double> cp_gamma_x_axis(2 * num_lines);
		std::iota(std::begin(cp_gamma_x_axis), std::end(cp_gamma_x_axis), 0);

		std::string cp_gamma_title = "cp (gamma)";
		std::string cp_gamma_filename = "cp_gamma.png";

		for(int gamma = gamma_start; gamma <= gamma_end; gamma += gamma_step)
		{

			wif_algo::calculation_results_c naca_airfoil_results = wif_algo::calculate_flow(naca_airfoil, uni_flow, Kutta, gamma);
			cp_gamma_plot.push_back(naca_airfoil_results.c_p);
			cp_gamma_legend.push_back(std::to_string(gamma));

		}

		wif_core::vector_2d_c midpoint(0, 0);
		std::shared_ptr<wif_viz::visualization_c> cp_gamma_plotter = wif_viz::create_visualization_root(uni_flow, midpoint, midpoint);
		cp_gamma_plotter->plotVectors(cp_gamma_plot, cp_gamma_x_axis, cp_gamma_legend, cp_gamma_filename, "paneel", "cp", cp_gamma_title);

	}


	// CP Intrados en Extrados verschillende AoA.
	{
		bool Kutta = 0;
		int num_lines = 20;
		wif_core::airfoil_c naca_airfoil("wif_core/airfoils/selig.dat");
		naca_airfoil = naca_airfoil.closed_merge(0.001);
		naca_airfoil = naca_airfoil.get_circle_projection(num_lines);

		double angle_start = 0.0;
		double angle_step = 0.5;
		double angle_end = 3.0;

		std::vector<std::vector<double>> cp_angle_plot(3);

		std::vector<std::string> cp_angle_legend(3);
		cp_angle_legend[0] = "Bovenkant";
		cp_angle_legend[1] = "Onderkant";
		cp_angle_legend[2] = "Verschil";

		std::vector<double> cp_angle_x_axis = naca_airfoil.get_lower_x(); // I checked, lower and upper size is same.


		for(double angle = angle_start; angle <= angle_end; angle += angle_step)
		{

			std::shared_ptr<wif_core::uniform_flow_c> uni_flow = std::make_shared<wif_core::uniform_flow_c>((angle / 180) * M_PI, 1);

			wif_algo::calculation_results_c cp_angle_results = wif_algo::calculate_flow(naca_airfoil, uni_flow, Kutta);

			std::vector<double> c_p_upper(cp_angle_results.c_p.begin(), cp_angle_results.c_p.begin() + num_lines);
			std::vector<double> c_p_lower(cp_angle_results.c_p.begin() + num_lines, cp_angle_results.c_p.end());

			std::reverse(c_p_upper.begin(), c_p_upper.end());

			cp_angle_plot[0] = c_p_upper;
			cp_angle_plot[1] = c_p_lower;

			std::vector<double> c_p_angle_differences;

			for(int i = 0; i < num_lines; i++)
			{
				c_p_angle_differences.push_back(c_p_upper[i] - c_p_lower[i]);
			}

			cp_angle_plot[2] = c_p_angle_differences;

			std::string cp_angle_title = "# panelen = " + std::to_string(num_lines) + " AoA = " + std::to_string(angle);
			std::string cp_angle_filename = "CpAoA" + std::to_string(angle) + ".png";

			wif_core::vector_2d_c midpoint(0, 0);
			std::shared_ptr<wif_viz::visualization_c> cp_angle_plotter = wif_viz::create_visualization_root(uni_flow, midpoint, midpoint);
			cp_angle_plotter->plotVectors(cp_angle_plot, cp_angle_x_axis, cp_angle_legend, cp_angle_filename, "x", "cp", cp_angle_title);

		}

	}


	// NACA closed body check als functie van aantal panelen.
	{
		bool Kutta = 0;

		std::shared_ptr<wif_core::uniform_flow_c> uni_flow = std::make_shared<wif_core::uniform_flow_c>(0, 1);

		std::vector<double> naca_closed_body_checks;
		std::vector<double> naca_closed_body_checks_x_axis;

		for(int num_lines = 10; num_lines <= 200; num_lines += 10)
		{
			wif_core::airfoil_c naca_airfoil("wif_core/airfoils/selig.dat");
			naca_airfoil = naca_airfoil.closed_merge(0.001);
			naca_airfoil = naca_airfoil.get_circle_projection(num_lines);
			wif_algo::calculation_results_c naca_closed_body_checks_results = wif_algo::calculate_flow(naca_airfoil, uni_flow, Kutta);

			naca_closed_body_checks.push_back(naca_closed_body_checks_results.closed_body_check);
			naca_closed_body_checks_x_axis.push_back(num_lines);

		}

		std::vector<std::vector<double>> naca_closed_body_checks_plot(1);

		for(int i = 0; i < naca_closed_body_checks.size(); i++)
		{
			naca_closed_body_checks[i] = naca_closed_body_checks[i] * 1000;
		}

		naca_closed_body_checks_plot[0] = naca_closed_body_checks;

		std::vector<std::string> naca_closed_body_checks_legend(1);
		naca_closed_body_checks_legend[0] = "Som sigma * l (10^3)";

		std::string naca_closed_body_checks_title = "Som sigma * l als functie van het aantal panelen";
		std::string naca_closed_body_checks_filename = "closed_body_airfoil.png";

		wif_core::vector_2d_c midpoint(0, 0);
		std::shared_ptr<wif_viz::visualization_c> naca_closed_body_checks_plotter = wif_viz::create_visualization_root(uni_flow, midpoint, midpoint);
		naca_closed_body_checks_plotter->plotVectors(naca_closed_body_checks_plot, naca_closed_body_checks_x_axis, naca_closed_body_checks_legend, naca_closed_body_checks_filename, "# panelen", "Som sigma *l (10^3)", naca_closed_body_checks_title);

	}


	// NACA PLOTS met Kutta.


	// CP Intrados en Extrados verschillende AoA.
	{
		bool Kutta = 1;
		int num_lines = 20;
		wif_core::airfoil_c naca_airfoil("wif_core/airfoils/selig.dat");
		naca_airfoil = naca_airfoil.closed_merge(0.001);
		naca_airfoil = naca_airfoil.get_circle_projection(num_lines);

		double angle_start = 0.0;
		double angle_step = 0.5;
		double angle_end = 3.0;

		std::vector<std::vector<double>> cp_angle_plot(3);

		std::vector<std::string> cp_angle_legend(3);
		cp_angle_legend[0] = "Bovenkant";
		cp_angle_legend[1] = "Onderkant";
		cp_angle_legend[2] = "Verschil";

		std::vector<double> cp_angle_x_axis = naca_airfoil.get_lower_x(); // I checked, lower and upper size is same.


		for(double angle = angle_start; angle <= angle_end; angle += angle_step)
		{

			std::shared_ptr<wif_core::uniform_flow_c> uni_flow = std::make_shared<wif_core::uniform_flow_c>((angle / 180) * M_PI, 1);

			wif_algo::calculation_results_c cp_angle_results = wif_algo::calculate_flow(naca_airfoil, uni_flow, Kutta);

			std::vector<double> c_p_upper(cp_angle_results.c_p.begin(), cp_angle_results.c_p.begin() + num_lines);
			std::vector<double> c_p_lower(cp_angle_results.c_p.begin() + num_lines, cp_angle_results.c_p.end());

			std::reverse(c_p_upper.begin(), c_p_upper.end());

			cp_angle_plot[0] = c_p_upper;
			cp_angle_plot[1] = c_p_lower;

			std::vector<double> c_p_angle_differences;

			for(int i = 0; i < num_lines; i++)
			{
				c_p_angle_differences.push_back(c_p_upper[i] - c_p_lower[i]);
			}

			cp_angle_plot[2] = c_p_angle_differences;

			std::string cp_angle_title = "# panelen = " + std::to_string(num_lines) + " AoA = " + std::to_string(angle) + " Met Kutta";
			std::string cp_angle_filename = "CpAoA" + std::to_string(angle) + "Kutta" + ".png";

			wif_core::vector_2d_c midpoint(0, 0);
			std::shared_ptr<wif_viz::visualization_c> cp_angle_plotter = wif_viz::create_visualization_root(uni_flow, midpoint, midpoint);
			cp_angle_plotter->plotVectors(cp_angle_plot, cp_angle_x_axis, cp_angle_legend, cp_angle_filename, "x", "cp", cp_angle_title);

		}
	}

	// CL voor verschillende airfoils bij verschillende AoA
	{



	}

	return 0;
}
