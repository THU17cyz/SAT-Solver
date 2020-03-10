//
// DPLL algorithm.
//

#ifndef DPLL_DPLL_H
#define DPLL_DPLL_H

#include "common.h"


class DPLL {
public:
    /**
     * Constructor.
     *
     * @param phi the formula to be checked
     * @note Please DON'T CHANGE this signature because the grading script will directly call this function!
     */
    DPLL(const formula &phi) : phi(phi) {}

    /**
     * Check if the formula is satisfiable.
     *
     * @return true if satisfiable, and false if unsatisfiable
     * @note Please DON'T CHANGE this signature because the grading script will directly call this function!
     */
    bool check_sat();

    /**
     * Get a satisfying model (interpretation) of the formula, the model must be *complete*, that is,
     * it must assign every variable a truth value.
     * This function will be called if and only if `check_sat()` returns true.
     *
     * @return an arbitrary (if there exist many) satisfying model
     * @note Please DON'T CHANGE this signature because the grading script will directly call this function!
     */
    model get_model();

private:
    void initialize();
    void decide();
    bool update(int var, int cls, std::queue<propagation>& prop);
    int backtrace();
	int backjump();
    void rollback(int vars, int delta_true_clause_num);
    int find_unit(int clause_id);
    bool propagate(std::vector<int> vars);
    bool reach_sat() { return true_clauses.size() == clause_num; }
    bool check_collision(int var);


    std::vector<std::vector<int>> satisfied_clauses;
    std::vector<std::vector<int>> not_satisfied_clauses;
    std::vector<int> interpretation; // P -> true(1)/false(-1)/undefined(0)
    std::vector<int> left_undef_literals;
    std::vector<bool> clause_is_true;
    std::vector<int> true_clauses;
    std::vector<int> decision_path;

	std::vector<int> origin_to_sorted;
	std::vector<int> sorted_to_origin;


	int update_count;

	unsigned int max_var_num;
	std::vector<long long> var_eval;

public:
    int last_decision;
    unsigned int clause_num;
    std::vector<step> prev_steps;
	graph implication_graph;
    formula phi;
	formula new_phi;
};


#endif //DPLL_DPLL_H
