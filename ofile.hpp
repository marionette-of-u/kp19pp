#ifndef IFILE_TXT_
#define IFILE_TXT_

#include <cstdlib>
#include <cassert>
#include <vector>

namespace grammar{

enum token{
    token_eq,
    token_ast,
    token_id,
    token_0
};

template<class T, int StackSize>
class stack{
public:
    stack(){ gap_ = 0; }
    ~stack(){}
    
    void reset_tmp()
    {
        gap_ = stack_.size();
        tmp_.clear();
    }

    void commit_tmp()
    {
        // may throw
        stack_.reserve(gap_ + tmp_.size());

        // expect not to throw
        stack_.erase(stack_.begin() + gap_, stack_.end());
        stack_.insert(stack_.end(), tmp_.begin(), tmp_.end());
    }

    bool push(const T& f)
    {
        if(StackSize != 0 && StackSize <= stack_.size() + tmp_.size()){
            return false;
        }
        tmp_.push_back(f);
        return true;
    }

    void pop(std::size_t n)
    {
        if(tmp_.size() < n){
            n -= tmp_.size();
            tmp_.clear();
            gap_ -= n;
        }else{
            tmp_.erase(tmp_.end() - n, tmp_.end());
        }
    }

    const T& top()
    {
        if(!tmp_.empty()){
            return tmp_.back();
        } else {
            return stack_[gap_ - 1];
        }
    }

    const T& get_arg(std::size_t base, std::size_t index)
    {
        std::size_t n = tmp_.size();
        if(base - index <= n){
            return tmp_[n - (base - index)];
        }else{
            return stack_[gap_ - (base - n) + index];
        }
    }

    void clear()
    {
        stack_.clear();
    }

private:
    std::vector<T> stack_;
    std::vector<T> tmp_;
    std::size_t gap_;

};

template<class Value, class SemanticAction, int StackSize = 0>
class parser {
public:
    typedef Token token_type;
    typedef Value value_type;

public:
    parser(SemanticAction& sa) : sa_(sa){ reset(); }

    void reset()
    {
        error_ = false;
        accepted_ = false;
        clear_stack();
        reset_tmp_stack();
        if(push_stack(&parser::state_0, &parser::gotof_0, value_type())){
            commit_tmp_stack();
        }else{
            sa_.stack_overflow();
            error_ = true;
        }
    }

    bool post(token_type token, const value_type& value)
    {
        assert(!error_);
        reset_tmp_stack();
        while((this->*(stack_top()->state))(token, value)); // may throw
        if(!error_){
            commit_tmp_stack();
        }
        return accepted_ || error_;
    }

    bool accept(value_type& v)
    {
        assert(accepted_);
        if(error_){ return false; }
        v = accepted_value_;
        return true;
    }

    bool error(){ return error_; }

private:
    typedef parser<Value, SemanticAction, StackSize> self_type;
    typedef bool (self_type::*state_type)(token_type, const value_type&);
    typedef bool ( self_type::*gotof_type )(int, const value_type&);

    bool            accepted_;
    bool            error_;
    value_type      accepted_value_;
    SemanticAction& sa_;

    struct stack_frame{
        state_type state;
        gotof_type gotof;
        value_type value;

        stack_frame(state_type s, gotof_type g, const value_type& v)
            : state(s), gotof(g), value(v){}
    };

    stack<stack_frame, StackSize> stack_;
    bool push_stack(state_type s, gotof_type g, const value_type& v)
    {
        bool f = stack_.push(stack_frame(s, g, v));
        assert(!error_);
        if(!f){
            error_ = true;
            sa_.stack_overflow();
        }
        return f;
    }

    void pop_stack(std::size_t n)
    {
        stack_.pop(n);
    }

    const stack_frame* stack_top()
    {
        return &stack_.top();
    }

    const value_type& get_arg(std::size_t base, std::size_t index)
    {
        return stack_.get_arg(base, index).value;
    }

    void clear_stack()
    {
        stack_.clear();
    }

    void reset_tmp_stack()
    {
        stack_.reset_tmp();
    }

    void commit_tmp_stack()
    {
        stack_.commit_tmp();
    }

    bool call_0_make_r(int nonterminal_index, int base, int arg_index0)
    {
        type_l arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        type_r r = sa_.make_r(arg0)
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_0_make_l_b(int nonterminal_index, int base, int arg_index0)
    {
        type_id arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        type_l r = sa_.make_l_b(arg0)
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_0_make_l_a(int nonterminal_index, int base, int arg_index0)
    {
        type_r arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        type_l r = sa_.make_l_a(arg0)
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_0_make_s_b(int nonterminal_index, int base, int arg_index0)
    {
        type_r arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        type_s r = sa_.make_s_b(arg0)
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_0_make_s_a(int nonterminal_index, int base, int arg_index0, int arg_index1)
    {
        type_l arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        type_r arg1; sa_.downcast(arg1, get_arg(base, arg_index1));
        type_s r = sa_.make_s_a(arg0, arg1)
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool gotof_0(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 0: return push_stack(&parser::state_4, &parser::gotof_4, v);
        case 1: return push_stack(&parser::state_9, &parser::gotof_9, v);
        case 2: return push_stack(&parser::state_7, &parser::gotof_7, v);
        default: assert(0); return false;
        }
    }

    bool state_0(token_type token, const value_type& value)
    {
        switch(token){
        case token_ast:
            // shift
            push_stack(&parser::state_1, &parser::gotof_1, value);
            return false;
        case token_id:
            // shift
            push_stack(&parser::state_5, &parser::gotof_5, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_1(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 0: return push_stack(&parser::state_3, &parser::gotof_3, v);
        case 2: return push_stack(&parser::state_6, &parser::gotof_6, v);
        default: assert(0); return false;
        }
    }

    bool state_1(token_type token, const value_type& value)
    {
        switch(token){
        case token_ast:
            // shift
            push_stack(&parser::state_1, &parser::gotof_1, value);
            return false;
        case token_id:
            // shift
            push_stack(&parser::state_5, &parser::gotof_5, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_2(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 0: return push_stack(&parser::state_3, &parser::gotof_3, v);
        case 2: return push_stack(&parser::state_8, &parser::gotof_8, v);
        default: assert(0); return false;
        }
    }

    bool state_2(token_type token, const value_type& value)
    {
        switch(token){
        case token_ast:
            // shift
            push_stack(&parser::state_1, &parser::gotof_1, value);
            return false;
        case token_id:
            // shift
            push_stack(&parser::state_5, &parser::gotof_5, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_3(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_3(token_type token, const value_type& value)
    {
        switch(token){
        case token_eq:
        case token_0:
            return call_0_make_r(2, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_4(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_4(token_type token, const value_type& value)
    {
        switch(token){
        case token_eq:
            // shift
            push_stack(&parser::state_2, &parser::gotof_2, value);
            return false;
        case token_0:
            return call_0_make_r(2, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_5(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_5(token_type token, const value_type& value)
    {
        switch(token){
        case token_eq:
        case token_0:
            return call_0_make_l_b(0, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_6(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_6(token_type token, const value_type& value)
    {
        switch(token){
        case token_eq:
        case token_0:
            return call_0_make_l_a(0, 2, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_7(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_7(token_type token, const value_type& value)
    {
        switch(token){
        case token_0:
            return call_0_make_s_b(1, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_8(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_8(token_type token, const value_type& value)
    {
        switch(token){
        case token_0:
            return call_0_make_s_a(1, 3, 0, 1);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_9(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_9(token_type token, const value_type& value)
    {
        switch(token){
        case token_0:
            // accept
            // run_semantic_action();
            accepted_ = true;
            accepted_value_ = get_arg(1, 0);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

};

} // namespace grammar

#endif // IFILE_TXT_
