#include "visualization.hpp"
#include <iostream>
namespace wif_viz
{


visualization_c::visualization_c(std::shared_ptr<flow_c> flow, const vector_2d_c & min_range, const vector_2d_c & max_range) :
	flow(flow),
	min_range(min_range),
	max_range(max_range),
	psi_bins(0.0, 0.0),
	phi_bins(0.0, 0.0),
	velocity_bins(0.0, 0.0),
	stagnation_tolerance(0.001),
	airfoil(nullptr),
	streamline_seeds(0.0, 0.0, 0.0, 0.0),
	streamline_resolution(0),
	arrow_scale(0.0)
{
	//
}


visualization_c::~visualization_c()
{
	//
}


void visualization_c::set_range(const vector_2d_c & new_min_range, const vector_2d_c & new_max_range)
{
	this->min_range = new_min_range;
	this->max_range = new_max_range;
}


void visualization_c::set_psi_bins(const vector_2d_c & bins)
{
	this->psi_bins = bins;
}


void visualization_c::set_phi_bins(const vector_2d_c & bins)
{
	this->phi_bins = bins;
}


void visualization_c::set_velocity_bins(const vector_2d_c & bins)
{
	this->velocity_bins = bins;
}


void visualization_c::set_contours(const std::vector<double> & contours)
{
	this->contour_locations = contours;
}


void visualization_c::set_airfoil(wif_core::airfoil_c * new_airfoil)
{
	this->airfoil = new_airfoil;
}

void visualization_c::set_contours(uint32_t contours)
{
	contour_locations.clear();

	for(uint32_t i = 1 ; i <= contours ; i++)
	{
		this->contour_locations.push_back(0.0);
	}
}


void visualization_c::set_stagnation_tolerance(double epsilon)
{
	this->stagnation_tolerance = epsilon;
}

void visualization_c::set_output_to_file(bool file_output)
{
	this->output_to_file = file_output;
}

void visualization_c::plotVectors(std::vector<std::vector<double>>, std::vector<double>, std::vector<std::string>, std::string, std::string, std::string, std::string)
{
}


} // namespace wif_viz
