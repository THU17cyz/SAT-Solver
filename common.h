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
	formula() {}
};

// A satisfying model (interpretation).
// e.g. `model[i] = false` means variable `i` is assigned to false.
typedef std::unordered_map<int, bool> model;

struct graph {
	int  from_mat[1024][64];
	int from_size[1024];
	bool fixed[1024];
	int node_number;
	void initialize_graph(int node_num) 
	{
		node_number = node_num;
		for (int i = 0; i < 1024; i++)
		{
			fixed[i] = false;
			from_size[i] = 0;
		}
	};

	// add a node (actually covering)
	void add_node(std::vector<int> from, int to)
	{
		/*if (!from.empty())
		{
			from_size[to] = 0;
		}*/
		int count = 0;
		for (const auto &n : from)
		{
			int abs_n = VAR(n);
			if (abs_n == to || fixed[abs_n])
			{
				continue;
			}
			from_mat[to][count] = abs_n;
			count++;
		}
		from_size[to] = count;
		if (from_size[to] == 0 && !from.empty())
		{
			fixed[to] = true;
		}
	}

	// generate the conflict clause
	std::vector<int> trace_conflict(int max)
	{
		std::vector<int> result;
		std::stack<int> node_stack;
		std::vector<bool> visited(node_number+1, false);
		node_stack.push(0);
		while (node_stack.size() > 0)
		{
			int node = node_stack.top();
			node_stack.pop();
			for (int i = 0; i < from_size[node]; i++)
			{
				int n = from_mat[node][i];
				if (!visited[n])
				{
					visited[n] = true;
					if (from_size[n] == 0)
					{
						result.push_back(n);
						continue;
					}
					node_stack.push(n);				
				}
			}
		}
		return result;
	}
};

struct propagation
{
	int var; // propagated variable
	int cls; // clause that caused the propagation
	propagation(int var_, int cls_): var(var_), cls(cls_) { }
};

struct step
{
	int delta_true_clause_num;
	int delta_interp;
	step(int n, int interp) : delta_true_clause_num(n), delta_interp(interp) {}
};

#endif //DPLL_COMMON_H
