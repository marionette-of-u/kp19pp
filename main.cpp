#include <string>
#include "kp19pp.hpp"

namespace kp19pp{
	struct eos_functor{
		std::string operator ()() const{
			return "$";
		}
	};

	struct eps_functor{
		std::string operator ()() const{
			return "";
		}
	};
}

int main(){
	typedef kp19pp::lalr1_type<int, std::string, std::string, kp19pp::eos_functor, kp19pp::eps_functor> lalr1_type;
	lalr1_type lalr1;
	std::string dummy_term = "#";

	// 文法を定義する --------
	// S' = S
	lalr1_type::expression_type expression_start_prime;
	{
		lalr1_type::expression_type e;
		e.lhs = "S'";

		{
			lalr1_type::expression_type::rhs_type rhs;
			rhs.push_back("S");
			e.rhs.insert(rhs);
		}

		lalr1.add_expression(e);

		expression_start_prime = e;
	}

	// S
	{
		lalr1_type::expression_type e;
		e.lhs = "S";

		{
			lalr1_type::expression_type::rhs_type rhs;
			rhs.push_back("L");
			rhs.push_back("=");
			rhs.push_back("R");
			e.rhs.insert(rhs);
		}

		{
			lalr1_type::expression_type::rhs_type rhs;
			rhs.push_back("R");
			e.rhs.insert(rhs);
		}

		lalr1.add_expression(e);
	}

	// L
	{
		lalr1_type::expression_type e;
		e.lhs = "L";

		{
			lalr1_type::expression_type::rhs_type rhs;
			rhs.push_back("*");
			rhs.push_back("R");
			e.rhs.insert(rhs);
		}

		{
			lalr1_type::expression_type::rhs_type rhs;
			rhs.push_back("identifier");
			e.rhs.insert(rhs);
		}

		lalr1.add_expression(e);
	}

	// R
	{
		lalr1_type::expression_type e;
		e.lhs = "R";

		{
			lalr1_type::expression_type::rhs_type rhs;
			rhs.push_back("L");
			e.rhs.insert(rhs);
		}

		lalr1.add_expression(e);
	}

	// 終端記号のデータを収集する --------
	{
		// !! dummy
		lalr1_type::symbol_data_type symbol_data;
		symbol_data.array_index = 0;
		symbol_data.linkdir = lalr1_type::nonassoc;
		symbol_data.priority = 0;

		lalr1.add_terminal_symbol("=", symbol_data);
		lalr1.add_terminal_symbol("*", symbol_data);
		lalr1.add_terminal_symbol("identifier", symbol_data);
	}

	// エンジンのオプション --------
	lalr1_type::make_parsing_table_option option;
	option.avoid_conflict = true;
	option.disambiguating = true;
	option.put_log = true;
	option.put_alltime = true;
	option.put_time = true;

	lalr1.make_lalr1_parsing_table(
		expression_start_prime,
		dummy_term,
		option,
		kp19pp::default_is_not_terminal<lalr1_type::expression_set_type>(),
		kp19pp::default_term_to_str<std::string>()
	);

	return 0;
}

