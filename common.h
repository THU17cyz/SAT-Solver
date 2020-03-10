//
// Common data structures.
//

#include <vector>
#include <unordered_map>
#include <stack>
#include <set>
#include <queue>
#include <string>
#include <sstream>

#ifndef DPLL_COMMON_H
#define DPLL_COMMON_H

// A literal is a atomic formula (that contains a variable). Like in dimacs,
// + positive numbers denote the corresponding variables;
// - negative numbers denote the negations of the corresponding variables.
// Variables are numbered from 1.
typedef int literal;
#define POSITIVE(x) ((x) > 0)
#define NEGATIVE(x) ((x) < 0)
#define VAR(x) (((x) > 0) ? (x) : (-(x)))

// A clause is a list of literals (meaning their disjunction).
typedef std::vector<literal> clause;

// A formula is a list of clauses (meaning their conjunction).
// We also specify the total number of variables, as some of them may not occur in any clause!
struct formula {
    int num_variable;
    std::vector<clause> clauses;

    formula(int n, const std::vector<clause>& clauses): num_variable(n), clauses(clauses) {}
};

// A satisfying model (interpretation).
// e.g. `model[i] = false` means variable `i` is assigned to false.
typedef std::unordered_map<int, bool> model;

struct node {
	std::vector<int> from;
	std::set<int> to;
	bool fixed;
	bool end;
	node(bool fixed_ = false, bool end_ = false) : fixed(fixed_), end(end_) {  }
	void add_to(int to_) { to.insert(to_); }
};

struct graph {
	//std::vector<node> nodes;
	int  from_mat[1024][64];
	int from_size[1024];
	//std::vector<int> desicion_nodes;
	int node_number;
	void initialize_graph(int node_num) 
	{
		node_number = node_num;
		/*
		for (int i = 0; i <= node_num; i++)
		{
			nodes.push_back(node());
			nodes[i].from.reserve(100);
		}*/
	};
	void add_node(std::vector<int> from, int to)
	{
		//nodes[to].from.clear();
		//nodes[to].to.clear();
		if (!from.empty())
		{
			from_size[to] = 0;
		}
		
		bool all_fixed = true;
		for (const auto &n : from)
		{
			if (VAR(n) == to)
			{
				continue;
			}
			from_mat[to][from_size[to]++] = VAR(n);

		}
	}
	std::vector<int> trace_conflict()
	{
		std::vector<int> result;
		std::stack<int> node_stack;
		std::vector<bool> visited(node_number+1, false);
		node_stack.push(0);
		while (node_stack.size() > 0)
		{
			int node = node_stack.top();
			node_stack.pop();
			/*if (nodes[node].fixed)
			{
				continue;
			}*/
			if (from_size[node] == 0)
			{
				result.push_back(node);
				continue;
			}
			for (int i = 0; i < from_size[node]; i++)
			{
				int n = from_mat[node][i];
				if (!visited[n])
				{
					visited[n] = true;
					node_stack.push(n);				
				}
			}
		}
		//std::cout << result.size() << std::endl;

		return result;
	}
};

/*
struct graph_
{
	std::vector<std::set<int>> nodes;
	std::unordered_map<int, std::vector<int>> decision_nodes;
	int node_number;
	void initialize_graph(int node_num)
	{
		node_number = node_num;
		for (int i = 0; i <= node_num; i++)
		{
			nodes.push_back(std::set<int>());
		}
	};
	std::vector<int> trace_conflict()
	{
		std::vector<int> result;
		std::stack<int> node_stack;
		std::vector<bool> visited(node_number + 1, false);
		node_stack.push(0);
		while (node_stack.size() > 0)
		{
			int node = node_stack.top();
			node_stack.pop();
			if (nodes[node].size() == 0)
			{
				result.push_back(node);
				continue;
			}
			for (const auto &n : nodes[node])
			{
				if (!visited[n])
				{
					node_stack.push(n);
					visited[n] = true;
				}
			}
		}
		// std::cout << result.size() << std::endl;
		return result;
		// return nodes[0];
	}
	void add_node(std::vector<int> from, int to)
	{
		to = VAR(to);
		if (from.empty())
		{
			//std::cout << to << " ";
			// nodes[to].insert(to); // decision_node
			decision_nodes[to] = std::vector<int>();
		}

		for (const auto &n : from)
		{
			for (const auto &m : nodes[VAR(n)])
			{
				nodes[to].insert(m);
				decision_nodes[m].push_back(to);
			}	
		}
	}
	void change_graph(int root)
	{
		for (const auto &n : decision_nodes[root])
		{
			nodes[n].clear();
		}
		decision_nodes.erase(root);
	}
	void fix_node(int root)
	{
		nodes[root].clear();
	}
};*/


struct propagation
{
	int var;
	int cls;
	propagation(int var_, int cls_): var(var_), cls(cls_) { }
};
#endif //DPLL_COMMON_H
