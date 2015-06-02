/*************************************************************************
    > File Name: BacktrackColoring.h
    > Author: Yibo Lin
    > Mail: yibolin@utexas.edu
    > Created Time: Mon Jun  1 22:22:14 2015
 ************************************************************************/

#ifndef LIMBO_ALGORITHMS_COLORING_BACKTRACKCOLORING
#define LIMBO_ALGORITHMS_COLORING_BACKTRACKCOLORING

#include <limbo/algorithms/coloring/Coloring.h>
#include <limbo/algorithms/coloring/GreedyColoring.h>

namespace limbo { namespace algorithms { namespace coloring {

template <typename GraphType>
class BacktrackColoring : public Coloring<GraphType>
{
	public:
		typedef Coloring<GraphType> base_type;
		using typename base_type::graph_type;
		using typename base_type::graph_vertex_type;
		using typename base_type::graph_edge_type;
		using typename base_type::vertex_iterator_type;
		using typename base_type::edge_iterator_type;
		using typename base_type::ColorNumType;
		typedef typename boost::graph_traits<graph_type>::adjacency_iterator adjacency_iterator_type;

		/// constructor
		BacktrackColoring(graph_type const& g) 
			: base_type(g)
		{}
		/// destructor
		virtual ~BacktrackColoring() {}
	protected:
		/// \return objective value 
		virtual double coloring();
		/// initial color assignment by greedy approach 
		/// \return cost 
		double init_coloring(vector<int8_t>& vColor) const;
		/// kernel function for recursive backtracking 
		void coloring_kernel(vector<int8_t>& vColor, double& best_cost, double& cur_cost, graph_vertex_type v) const;
};

template <typename GraphType>
double BacktrackColoring<GraphType>::coloring()
{
	vector<int8_t> vColor (this->m_vColor.begin(), this->m_vColor.end());
	double best_cost = this->init_coloring(vColor);
	double cur_cost = 0;

	// solve coloring problem 
	this->coloring_kernel(vColor, best_cost, cur_cost, 0);

	// apply coloring solution 
	this->m_vColor.swap(vColor);

	// verify solution  
	double cost = this->calc_cost(this->m_vColor);
	assert_msg(best_cost == cost, "best_cost = " << best_cost << ", actual cost = " << cost);

	return best_cost;
}

template <typename GraphType>
double BacktrackColoring<GraphType>::init_coloring(vector<int8_t>& vColor) const 
{
	DsatColoring<graph_type> dc (this->m_graph);
	dc();

	vertex_iterator_type vi, vie;
	for (boost::tie(vi, vie) = boost::vertices(this->m_graph); vi != vie; ++vi)
	{
		graph_vertex_type v = *vi;
		int8_t color = dc.color(v);
		if (vColor[v] >= 0 && vColor[v] < this->m_color_num) // precolored 
			continue;
		else if (color >= this->m_color_num) 
			vColor[v] = this->m_color_num;
		else vColor[v] = color;
	}
	// calculate cost 
	double cost = this->calc_cost(vColor);
	return cost;
}

template <typename GraphType>
void BacktrackColoring<GraphType>::coloring_kernel(vector<int8_t>& vColor, double& best_cost, double& cur_cost, BacktrackColoring<GraphType>::graph_vertex_type v) const 
{
	if (best_cost == 0) // no conflict 
		return;
	if (cur_cost >= best_cost) // branch and bound 
		return; 
	if (v == boost::num_vertices(this->m_graph)) // leaf node in the recursion tree 
	{
		if (cur_cost < best_cost) 
			best_cost = cur_cost;
		return;
	}

	int8_t color_begin = 0;
	int8_t color_end = this->m_color_num;
	if (this->m_vColor[v] >= 0 && this->m_vColor[v] < this->m_color_num) // precolored vertex 
	{
		color_begin = this->m_vColor[v];
		color_end = color_begin+1;
	}
	for (int8_t c = color_begin; c < color_end; ++c)
	{
		vColor[v] = c;
		double delta_cost = 0;
		adjacency_iterator_type vi, vie;
		for (boost::tie(vi, vie) = boost::adjacent_vertices(v, this->m_graph); vi != vie; ++vi)
		{
			graph_vertex_type u = *vi;
			if (u < v) // only check parent node in the recursion tree 
			{
				if (vColor[u] == c) // conflict 
				{
					pair<graph_edge_type, bool> e = boost::edge(u, v, this->m_graph);
					assert_msg(e.second, "failed to find edge with " << u << "--" << v);
					int32_t w = boost::get(boost::edge_weight, this->m_graph, e.first);
					assert_msg(w > 0, "only support conflict edges with positive cost"); // only support conflict edges 
					delta_cost += w;
				}
			}
		}
		cur_cost += delta_cost;
		this->coloring_kernel(vColor, best_cost, cur_cost, v+1); // recursion 
		cur_cost -= delta_cost;
	}
}

}}} // namespace limbo // namespace algorithms // namespace coloring

#endif