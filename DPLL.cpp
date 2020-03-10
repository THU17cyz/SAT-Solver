//
// DPLL algorithm.
//
#include <iostream>
#include "DPLL.h"



bool DPLL::check_sat()
{
    initialize();
    std::vector<int> singles;
    // single-literal clauses
    int count = 0;
    for (const auto &c : new_phi.clauses)
    {
        if (left_undef_literals[count] == 1)
        {
            int var = c.back();
            singles.push_back(var);
		    implication_graph.fixed[VAR(var)] = true;
        }
        count++;
    }
    if(!propagate(singles)) 
    {
        return false; 
    }
	prev_steps.clear();

    bool after_backjump = false;
	int next_decision;
	int jump_c = 0;
    while (true)
    {
		std::vector<int> var;
        // after backjump, skip decide
        if (!after_backjump)
        {
            decide();	
			if (decision_path.empty())
			{
				return true;
			}
			var.push_back(decision_path.back());
        }
		else
		{
			var.push_back(next_decision);
		}  
		
        bool has_conflict = !propagate(var);
        if (has_conflict)
        {
			jump_c++;
			next_decision = backjump();
            if (next_decision != 0)
            {   
                after_backjump = true;
            }
            else
            {
				//std::cout << jump_c << " " << update_count <<  std::endl;
                return false;
            }
        }
        else 
        {
            if (reach_sat()) 
            {
				//std::cout << jump_c << " " << update_count << std::endl;
                return true;
            }
            after_backjump = false;
        }
    }
}

model DPLL::get_model()
{
    // TODO: your code here, or in the header file
	model res;
	for (int i = 1; i <= phi.num_variable; i++)
	{
		if (interpretation[origin_to_sorted[i]] == 1)
		{
			res[i] = true;
		}
		else
		{
			res[i] = false;
		}
	}
	/*
	test model
	*/
	for (const auto &c : phi.clauses)
	{
		bool flag = false;
		for (const auto &l : c)
		{
			if ((l > 0 && res[l]) || (l < 0 && !res[-l])) flag = true;
		}
		if (!flag)
		{
			std::cout << "wrong!" << std::endl;
			break;
		}
	}
    return res;
}

void DPLL::initialize()
{
    interpretation.push_back(0); // start from index 1
    std::vector<int> tmp;
    satisfied_clauses.push_back(tmp);
    not_satisfied_clauses.push_back(tmp); // start from index 1
	var_eval.push_back(0);
    // initialize empty vectors
    for (int i = 0; i < phi.num_variable; i++) 
    {
		var_eval.push_back(0);
        interpretation.push_back(1);
        std::vector<int> pos;
        std::vector<int> neg;
        satisfied_clauses.push_back(pos);
        not_satisfied_clauses.push_back(neg);
    }
	new_phi.num_variable = phi.num_variable;
    int count = 0;
	max_var_num = 0;
	for (const auto &c : phi.clauses)
	{
		if (c.size() > max_var_num)
		{
			max_var_num = c.size();
		}
	}
	max_var_num++;
    for (const auto &c : phi.clauses)
    {
		clause_is_true.push_back(false);
		new_phi.clauses.push_back(clause());
        for (const auto &var : c)
        {
            int abs_var = VAR(var);
			if (var > 0)
			{
				var_eval[abs_var] += max_var_num - c.size();
			}
			else {
				var_eval[abs_var] += max_var_num - c.size();
			}
        }
		left_undef_literals.push_back(c.size());
        count++;
    }
    clause_num = count;
	
	implication_graph.initialize_graph(phi.num_variable);

	for (int i = 0; i <= phi.num_variable; i++)
	{
		origin_to_sorted.push_back(i);
		sorted_to_origin.push_back(i);
	}
	
	for (int i = 1; i <= phi.num_variable; i++)
	{
		for (int j = 1; j <= phi.num_variable - i; j++)
		{
			if (var_eval[sorted_to_origin[j]] < var_eval[sorted_to_origin[j + 1]]) // (satisfied_clauses[ordered_vars[j]].size() + not_satisfied_clauses[ordered_vars[j]].size() < satisfied_clauses[ordered_vars[j + 1]].size() + not_satisfied_clauses[ordered_vars[j + 1]].size()))
			{
				int tmp = sorted_to_origin[j];
				sorted_to_origin[j] = sorted_to_origin[j + 1];
				sorted_to_origin[j + 1] = tmp;
			}
		}
	}
	for (int i = 1; i <= phi.num_variable; i++)
	{
		origin_to_sorted[sorted_to_origin[i]] = i;
	}

	count = 0;
	for (const auto &c : phi.clauses)
	{
		for (const auto &var : c)
		{
			int abs_var = VAR(var);
			int new_var = origin_to_sorted[abs_var];

			// filter the variables that never appeared
			if (interpretation[new_var] == 1)
			{
				interpretation[new_var] = 0;
			}
			if (POSITIVE(var))
			{
				satisfied_clauses[new_var].push_back(count);
				new_phi.clauses[count].push_back(new_var);
			}
			else
			{
				not_satisfied_clauses[new_var].push_back(count);
				new_phi.clauses[count].push_back(-new_var);
			}
		}
		count++;
	}
}

void DPLL::decide()
{
	int i = 1;
	if (decision_path.empty())
	{
		prev_steps.clear();
	}
	else
	{
		i = VAR(decision_path.back()) + 1;
		//i = decision_path.back() + 1;
	}

    // choose the next undefined variable 
    for(; i <= phi.num_variable; i++)
    {
        if (interpretation[i] == 0)
        {
            decision_path.push_back(i);
			//implication_graph.nodes[ordered_vars[i]].from.clear();
			implication_graph.from_size[i] = 0;
			break;
        }
    }
}

int DPLL::backtrace()
{
	int last_decision;
    // change path
    while (true)
    {
		if (decision_path.empty())
		{
			return 0;
		}
        last_decision = decision_path.back();
        decision_path.pop_back();
        if (last_decision > 0)
        {
			// decision_path.push_back(-last_decision);
            break;
        }
    }
	// trace till the last decision, recover the modification
	while (true)
	{
		step last_step = prev_steps.back();
		prev_steps.pop_back();
		rollback(last_step.delta_interp, last_step.delta_true_clause_num);
		if (last_step.delta_interp == last_decision)
		{
			break;
		}

	}
	decision_path.push_back(-last_decision);
	return -last_decision;
}

int DPLL::backjump()
{
	int decision_size = decision_path.size();
	if (decision_size == 0)
	{
		return 0;
	}
	int end = decision_path.back();
	int result = -end;
	int last_decision = decision_path.back();
	decision_path.pop_back();

	int second_max = 0;
	std::vector<int> from;
	std::vector<int> conflict_clause;
	if (decision_size == 1)
	{
		//conflict_clause.push_back(0);
	}
	else
	{
		implication_graph.from_mat[end][0] = end;
		implication_graph.from_size[end] = 1;
		conflict_clause = implication_graph.trace_conflict(end);
		
		for (const auto v : conflict_clause)
		{
			//from.push_back(abs_var);
			if (v > second_max)
			{
				second_max = v;
			}
			//std::cout << v << " ";
		}
		//std::cout << std::endl;*/
	}
	


	if (second_max)
	{
		// change path
		while (decision_path.size() > 0)
		{
			//count++;
			if (decision_path.back() == second_max)
			{
				break;
			}
			last_decision = decision_path.back();
			decision_path.pop_back();
		}
		if (decision_path.size() == 0)
		{
			//std::cout << result << " ";
			implication_graph.from_size[end] = 0;
			implication_graph.fixed[end] = true;
		}
	}
	else
	{
		//std::cout << result << " ";
		implication_graph.from_size[end] = 0;
		implication_graph.fixed[end] = true;
	}

	
	implication_graph.add_node(conflict_clause, end);
	

	// trace till the last decision, recover the modification
	while (true)
	{
		step last_step = prev_steps.back();
		prev_steps.pop_back();
		rollback(last_step.delta_interp, last_step.delta_true_clause_num);
		if (last_step.delta_interp == last_decision)
		{
			break;
		}
	}
	return result;
}

int DPLL::find_unit(int clause_id) 
{
    if (left_undef_literals[clause_id] != 1 || clause_is_true[clause_id])
    {
        return 0;
    }
    for (const auto &v : new_phi.clauses[clause_id])
    {
        if (interpretation[VAR(v)] == 0) 
        {           
            return v;
        }
    }
	return 0;
}

bool DPLL::update(int var, int cls, std::queue<propagation>& prop)
{
	update_count++;
	int value = interpretation[VAR(var)];
	if (value > 0)
	{
		if (var > 0)
		{
			return true;
		}
		else
		{
			if (cls != -1)
			{
				implication_graph.add_node(new_phi.clauses[cls], 0);
			}
			return false;
		}
	}
	else if (value < 0)
	{
		if (var < 0)
		{
			return true;
		}
		else
		{
			if (cls != -1)
			{
				implication_graph.add_node(new_phi.clauses[cls], 0);
			}
			return false;
		}
	}
	// 可能重复建图

	if (cls != -1)
	{
		implication_graph.add_node(new_phi.clauses[cls], VAR(var));
	}	

    int delta_true_clause_num = 0;
    if (POSITIVE(var))
    {
        interpretation[VAR(var)] = 1;
        for (const auto &c : satisfied_clauses[var])
        {
            if (!clause_is_true[c])
            {
                delta_true_clause_num++;
                clause_is_true[c] = true;
                true_clauses.push_back(c);
            }
            left_undef_literals[c]--;
        }
        for (const auto &c : not_satisfied_clauses[var])
        {
            left_undef_literals[c]--;
			int res = find_unit(c);
			if (res)
			{
				if (check_collision(res))
				{
					return false;
				}
				prop.push(propagation(res, c));
			}
        }
    }
    else
    {
        interpretation[VAR(var)] = -1;
        for (const auto &c : not_satisfied_clauses[-var])
        {
            if (!clause_is_true[c])
            {
                delta_true_clause_num++;
                clause_is_true[c] = true;
                true_clauses.push_back(c);
            }
            left_undef_literals[c]--;
        }
        for (const auto &c : satisfied_clauses[-var])
        {
            left_undef_literals[c]--;
			int res = find_unit(c);
			if (res)
			{
				if (check_collision(res))
				{
					return false;
				}
				prop.push(propagation(res, c));
			}
        }
    }
    prev_steps.push_back(step(delta_true_clause_num, var));
    return true;
}

void DPLL::rollback(int var, int delta_true_clause_num)
{
    int abs_var = VAR(var);
    interpretation[abs_var] = 0;
    for (const auto &c : satisfied_clauses[abs_var])
    {
        left_undef_literals[c]++;
    }
    for (const auto &c : not_satisfied_clauses[abs_var])
    {
        left_undef_literals[c]++;
    }
    for (int i = 0; i < delta_true_clause_num; i++)
    {
        clause_is_true[true_clauses.back()] = false;
        true_clauses.pop_back();
    }
}

bool DPLL::check_collision(int var)
{
    int value = interpretation[VAR(var)];
    if ((value > 0 && var < 0) || (value < 0 && var > 0))
    {
        return true;
    }
    return false;
}


bool DPLL::propagate(std::vector<int> last_decisions)
{
	std::queue<propagation> props;
	for (unsigned int i = 0; i < last_decisions.size(); i++)
	{
		props.push(propagation(last_decisions[i], -1));
	}
    
    while (props.size() > 0)
    {
        // update the stack top
        int var = props.front().var;
		int cls = props.front().cls;
        props.pop();
        if (!update(var, cls, props))
        {
            return false;
        }
    }
    return true;
}