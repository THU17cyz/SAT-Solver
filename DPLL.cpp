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
    for (const auto &c : phi.clauses)
    {
        if (left_undef_literals[count] == 1)
        {
            int var = c.back();
            singles.push_back(var);
		    //implication_graph.nodes[VAR(var)].fixed = true;
        }
        count++;
    }
    // std::cout << singles.size() << std::endl;
    if(!propagate(singles)) 
    {
        return false; 
    }
	std::cout << prev_steps.size() << std::endl;
    while(!prev_steps.empty())
    {
        prev_steps.pop_back();
        undef_var_num--; // fix these variables
    }

	/*
	int cc = 0;
	for (const auto &c : phi.clauses)
	{
		if (clause_is_true[cc++])
		{
			//std::cout << cc << std::endl;
			//continue;
		}
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
	}*/

	for (int i = 1; i <= phi.num_variable; i++)
	{
		for (int j = 1; j <= phi.num_variable - i; j++)
		{
			if (interpretation[ordered_vars[j]]!=0 || var_eval[ordered_vars[j]] < var_eval[ordered_vars[j+1]]) // (satisfied_clauses[ordered_vars[j]].size() + not_satisfied_clauses[ordered_vars[j]].size() < satisfied_clauses[ordered_vars[j + 1]].size() + not_satisfied_clauses[ordered_vars[j + 1]].size()))
			{
				int tmp = ordered_vars[j];
				ordered_vars[j] = ordered_vars[j + 1];
				ordered_vars[j + 1] = tmp;
			}
		}
	}

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
			// decision_path.push_back(next_decision);
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
				std::cout << jump_c << " " << update_count <<  std::endl;
                return false;
            }
        }
        else 
        {
            if (reach_sat()) 
            {
				std::cout << jump_c << " " << update_count << std::endl;
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
		if (interpretation[i] == 1)
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
			if (POSITIVE(l * interpretation[VAR(l)])) flag = true;
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
    undef_var_num = 0;
    interpretation.push_back(0); // start from index 1
    // left_undef_literals.push_back(0); // start from index 1
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
			
            
            // filter the variables that never appeared
            if (interpretation[abs_var] == 1)
            {
                interpretation[abs_var] = 0;
                undef_var_num++;
            }
            if (POSITIVE(var))
            {
                satisfied_clauses[abs_var].push_back(count);
            }
            else
            {
                not_satisfied_clauses[abs_var].push_back(count);
            }
        }
        left_undef_literals.push_back(c.size());
        count++;
    }
    clause_num = count;
	
	implication_graph.initialize_graph(phi.num_variable);

	for (int i = 0; i <= phi.num_variable; i++)
	{
		ordered_vars.push_back(i);
	}
	
	//std::sort(ordered_vars.begin() + 1, ordered_vars.end() - 1, bound_cmp);
	for (int i = 1; i <= phi.num_variable; i++)
	{
		//std::cout << ordered_vars[i] << std::endl;
		//std::cout << satisfied_clauses[ordered_vars[i]].size() + not_satisfied_clauses[ordered_vars[i]].size() << std::endl;
	}
	next_decision = 1;
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
		//i = decision_path.back() + 1;
	}
	/*
	if (interpretation[VAR(next_decision)] == 0)
	{
		decision_path.push_back(next_decision);
		implication_graph.nodes[VAR(next_decision)].from.clear();
		return;
	}*/
    // choose the next undefined variable 
    for(; i <= phi.num_variable; i++)
    {
        if (interpretation[ordered_vars[i]] == 0)
        {
            decision_path.push_back(ordered_vars[i]);
			//implication_graph.nodes[ordered_vars[i]].from.clear();
			implication_graph.from_size[ordered_vars[i]] = 0;
			break;
        }
    }
}

int DPLL::backtrace()
{
	// std::vector<int> conflict_clause = implication_graph.trace_conflict();
	

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
		//implication_graph.change_graph(VAR(last_decision));
        if (last_decision > 0)
        {
			// decision_path.push_back(-last_decision);
            break;
        }
    }
    if (decision_path.size() == 1) // the root variable is fixed now
    {
        undef_var_num--;
    }
	// trace till the last decision, recover the modification
	while (true)
	{
		step last_step = prev_steps.back();
		prev_steps.pop_back();
		rollback(last_step.delta_interp, last_step.delta_true_clause_num);
		// std::cout << last_decision << " " <<  last_step.delta_interp << std::endl;
		if (last_step.delta_interp == last_decision)
		{
			break;
		}

	}
	return -last_decision;
}

int DPLL::backjump()
{
	if (decision_path.size() == 0)
	{
		return 0;
	}
	std::vector<int> conflict_clause = implication_graph.trace_conflict();

	/*phi.clauses.push_back(conflict_clause);
	clause_is_true.push_back(false);
	left_undef_literals.push_back(conflict_clause.size() - 1);
	std::vector<int> from;
	for (const auto v : conflict_clause)
	{
		int abs_var = VAR(v);
		if (interpretation[abs_var] == 1)
		{
			not_satisfied_clauses[abs_var].push_back(clause_num);
		}
		else
		{
			satisfied_clauses[abs_var].push_back(clause_num);
		}
		if (abs_var != VAR(decision_path.back()))
		{
			from.push_back(abs_var);
		}
	}
	clause_num++;*/
	
	
	std::vector<int> from;
	for (const auto v : conflict_clause)
	{
		int abs_var = VAR(v);
		if (abs_var != VAR(decision_path.back()))
		{
			from.push_back(abs_var);
		}
		//std::cout << v << " ";
	}
	//std::cout << std::endl;
	
	
	int result = -decision_path.back();
	int last_decision = decision_path.back();
	decision_path.pop_back();
	//implication_graph.change_graph(VAR(last_decision));
	
	if (decision_path.size() == 0)
	{
		std::cout << last_decision << std::endl;
		// implication_graph.nodes[VAR(last_decision)].from.clear();
		implication_graph.from_size[VAR(last_decision)] = 0;
		//implication_graph.nodes[VAR(last_decision)].fixed = true;
	}
	implication_graph.add_node(from, VAR(last_decision));

	int count = 0;
	// change path
	while (decision_path.size() > 0)
	{
		//count++;
		int second_last = decision_path.back(); // second last dicision
		if (std::find(conflict_clause.begin(), conflict_clause.end(), second_last) != conflict_clause.end()) // not in conflict_clause
		{
			// std::cout << second_last << " " << *std::find(conflict_clause.begin(), (conflict_clause.end()), second_last) << std::endl;
			// decision_path.push_back(-last_decision);
			break;
		}
		last_decision = decision_path.back();
		decision_path.pop_back();
		//implication_graph.change_graph(VAR(last_decision));
	}

	//std::cout << count << " ";

	// trace till the last decision, recover the modification
	while (true)
	{
		step last_step = prev_steps.back();
		prev_steps.pop_back();
		rollback(last_step.delta_interp, last_step.delta_true_clause_num);
		// std::cout << last_decision << " " <<  last_step.delta_interp << std::endl;
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
    for (const auto &v : phi.clauses[clause_id])
    {
        if (interpretation[VAR(v)] == 0) 
        {           
            return v;
        }
    }
}

bool DPLL::update(int var, int cls, std::queue<propagation>& prop)
{
	update_count++;
	if (interpretation[VAR(var)] * var == VAR(var))
	{
		return true;
	}
	// 可能重复建图

    // check if contradiction occurs
    if (check_collision(var))
    {
		implication_graph.add_node(phi.clauses[cls], 0);
        return false;
    }
	if (cls == -1)
	{
		implication_graph.add_node(clause(), VAR(var));
		/*if (decision_path.empty() && var < 0)
		{
			implication_graph.nodes[VAR(var)].clear();
		}*/
	}
	else
	{
		implication_graph.add_node(phi.clauses[cls], VAR(var));
	}
	/*if (implication_graph.nodes[VAR(var)].fixed)
	{
		// var must be negative
		for (const auto &c : not_satisfied_clauses[-var])
		{
			std::cout << c << " ";
			for (const auto &v : phi.clauses[c])
			{
				if (v == var)
				{
					continue;
				}
				if (v > 0)
				{
					std::vector<int> &tmp = satisfied_clauses[v];
					std::vector<int>::iterator iter = std::find(tmp.begin(), tmp.end(), c);
					tmp.erase(iter);
				}
				else
				{
					std::vector<int> &tmp = not_satisfied_clauses[-v];
					std::vector<int>::iterator iter = std::find(tmp.begin(), tmp.end(), c);
					tmp.erase(iter);
				}
			}
		}
		
	}*/
	
	

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
			/*
			if (left_undef_literals[c] == 2)
			{
				for (const auto &v : phi.clauses[c])
				{
					if (v < 0 && interpretation[VAR(v)] == 0)
					{
						next_decision = -v;
						continue;
					}
				}
				continue;
			}*/
			int res = find_unit(c);
			if (res)
			{
				if (interpretation[VAR(res)] * res == VAR(res))
				{
					continue;
				}
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
			/*if (left_undef_literals[c] == 2)
			{
				for (const auto &v : phi.clauses[c])
				{
					if (v < 0 && interpretation[VAR(v)] == 0)
					{
						next_decision = -v;
						continue;
					}
				}
				continue;
			}*/
			int res = find_unit(c);
			if (res)
			{
				if (interpretation[VAR(res)] * res == VAR(res))
				{
					continue;
				}
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
	// std::cout << abs_var << std::endl;
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
    if ((value == 1 && var < 0) || (value == -1 && var > 0))
    {
        return true;
    }
    // interpretation[VAR(var)] = var > 0 ? 1 : -1;
    return false;
}




bool DPLL::propagate(std::vector<int> last_decisions)
{
    std::queue<propagation> props; // a queue
	for (int i = 0; i < last_decisions.size(); i++)
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
		/*
        if (POSITIVE(var))
        {
            // push all deduced variables into the stack, but not update them at once
            for (const auto &s : not_satisfied_clauses[var])
            {
                int res = find_unit(s);
                if (res) 
                {
					if (interpretation[VAR(res)] * res == VAR(res))
					{
						continue;
					}
                    if (check_collision(res))
                    {
                        return false;
                    }
                    props.push(propagation(res, s));        
                }
            }
        }
        else
        {
            for (const auto &s : satisfied_clauses[-var])
            {
                int res = find_unit(s);
                if (res)
                {
					if (interpretation[VAR(res)] * res == VAR(res))
					{
						continue;
					}
                    if (check_collision(res))
                    {
                        return false;
                    }
                    props.push(propagation(res, s));
                }
            }
        }
		*/
    }
    return true;
}