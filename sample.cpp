#ifndef SAMPLE_CPP_
#define SAMPLE_CPP_

#include <cstdlib>
#include <cassert>
#include <vector>

namespace Parser{

enum Token{
    Token_UMns,
    Token_Ast,
    Token_Sla,
    Token_Pls,
    Token_Mns,
    Token_LParen,
    Token_RParen,
    Token_Id,
    Token_0
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
    typedef token token_type;
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

    bool call_0_makeUMns(int nonterminal_index, int base, int arg_index0)
    {
        MyType arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        MyType r = sa_.makeUMns(arg0);
        value_type v = sa_.defaultValue();
        sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_0_makeMlt(int nonterminal_index, int base, int arg_index0, int arg_index1)
    {
        MyType arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        MyType arg1; sa_.downcast(arg1, get_arg(base, arg_index1));
        MyType r = sa_.makeMlt(arg0, arg1);
        value_type v = sa_.defaultValue();
        sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_0_makeDiv(int nonterminal_index, int base, int arg_index0, int arg_index1)
    {
        MyType arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        MyType arg1; sa_.downcast(arg1, get_arg(base, arg_index1));
        MyType r = sa_.makeDiv(arg0, arg1);
        value_type v = sa_.defaultValue();
        sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_0_makeAdd(int nonterminal_index, int base, int arg_index0, int arg_index1)
    {
        MyType arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        MyType arg1; sa_.downcast(arg1, get_arg(base, arg_index1));
        MyType r = sa_.makeAdd(arg0, arg1);
        value_type v = sa_.defaultValue();
        sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_0_makeSub(int nonterminal_index, int base, int arg_index0, int arg_index1)
    {
        MyType arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        MyType arg1; sa_.downcast(arg1, get_arg(base, arg_index1));
        MyType r = sa_.makeSub(arg0, arg1);
        value_type v = sa_.defaultValue();
        sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_0_makeId(int nonterminal_index, int base)
    {
        MyType r = sa_.makeId();
        value_type v = sa_.defaultValue();
        sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_1_makeId(int nonterminal_index, int base, int arg_index0)
    {
        MyType arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        MyType r = sa_.makeId(arg0);
        value_type v = sa_.defaultValue();
        sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool gotof_0(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 0: return push_stack(&parser::state_13, &parser::gotof_13, v);
        default: assert(0); return false;
        }
    }

    bool state_0(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Mns:
            // shift
            push_stack(&parser::state_1, &parser::gotof_1, value);
            return false;
        case Token_LParen:
            // shift
            push_stack(&parser::state_2, &parser::gotof_2, value);
            return false;
        case Token_Id:
            // shift
            push_stack(&parser::state_14, &parser::gotof_14, value);
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
        case 0: return push_stack(&parser::state_7, &parser::gotof_7, v);
        default: assert(0); return false;
        }
    }

    bool state_1(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Mns:
            // shift
            push_stack(&parser::state_1, &parser::gotof_1, value);
            return false;
        case Token_LParen:
            // shift
            push_stack(&parser::state_2, &parser::gotof_2, value);
            return false;
        case Token_Id:
            // shift
            push_stack(&parser::state_14, &parser::gotof_14, value);
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
        case 0: return push_stack(&parser::state_8, &parser::gotof_8, v);
        default: assert(0); return false;
        }
    }

    bool state_2(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Mns:
            // shift
            push_stack(&parser::state_1, &parser::gotof_1, value);
            return false;
        case Token_LParen:
            // shift
            push_stack(&parser::state_2, &parser::gotof_2, value);
            return false;
        case Token_Id:
            // shift
            push_stack(&parser::state_14, &parser::gotof_14, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_3(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 0: return push_stack(&parser::state_9, &parser::gotof_9, v);
        default: assert(0); return false;
        }
    }

    bool state_3(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Mns:
            // shift
            push_stack(&parser::state_1, &parser::gotof_1, value);
            return false;
        case Token_LParen:
            // shift
            push_stack(&parser::state_2, &parser::gotof_2, value);
            return false;
        case Token_Id:
            // shift
            push_stack(&parser::state_14, &parser::gotof_14, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_4(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 0: return push_stack(&parser::state_10, &parser::gotof_10, v);
        default: assert(0); return false;
        }
    }

    bool state_4(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Mns:
            // shift
            push_stack(&parser::state_1, &parser::gotof_1, value);
            return false;
        case Token_LParen:
            // shift
            push_stack(&parser::state_2, &parser::gotof_2, value);
            return false;
        case Token_Id:
            // shift
            push_stack(&parser::state_14, &parser::gotof_14, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_5(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 0: return push_stack(&parser::state_11, &parser::gotof_11, v);
        default: assert(0); return false;
        }
    }

    bool state_5(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Mns:
            // shift
            push_stack(&parser::state_1, &parser::gotof_1, value);
            return false;
        case Token_LParen:
            // shift
            push_stack(&parser::state_2, &parser::gotof_2, value);
            return false;
        case Token_Id:
            // shift
            push_stack(&parser::state_14, &parser::gotof_14, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_6(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 0: return push_stack(&parser::state_12, &parser::gotof_12, v);
        default: assert(0); return false;
        }
    }

    bool state_6(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Mns:
            // shift
            push_stack(&parser::state_1, &parser::gotof_1, value);
            return false;
        case Token_LParen:
            // shift
            push_stack(&parser::state_2, &parser::gotof_2, value);
            return false;
        case Token_Id:
            // shift
            push_stack(&parser::state_14, &parser::gotof_14, value);
            return false;
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
        case Token_Ast:
        case Token_Sla:
        case Token_Pls:
        case Token_Mns:
        case Token_RParen:
        case Token_0:
            return call_0_makeUMns(0, 2, 1);
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
        case Token_Ast:
            // shift
            push_stack(&parser::state_3, &parser::gotof_3, value);
            return false;
        case Token_Sla:
            // shift
            push_stack(&parser::state_4, &parser::gotof_4, value);
            return false;
        case Token_Pls:
            // shift
            push_stack(&parser::state_5, &parser::gotof_5, value);
            return false;
        case Token_Mns:
            // shift
            push_stack(&parser::state_6, &parser::gotof_6, value);
            return false;
        case Token_RParen:
            // shift
            push_stack(&parser::state_15, &parser::gotof_15, value);
            return false;
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
        case Token_Ast:
        case Token_Sla:
        case Token_Pls:
        case Token_Mns:
        case Token_RParen:
        case Token_0:
            return call_0_makeMlt(0, 3, 0, 2);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_10(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_10(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Ast:
        case Token_Sla:
        case Token_Pls:
        case Token_Mns:
        case Token_RParen:
        case Token_0:
            return call_0_makeDiv(0, 3, 0, 2);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_11(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_11(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Ast:
            // shift
            push_stack(&parser::state_3, &parser::gotof_3, value);
            return false;
        case Token_Sla:
            // shift
            push_stack(&parser::state_4, &parser::gotof_4, value);
            return false;
        case Token_Pls:
        case Token_Mns:
        case Token_RParen:
        case Token_0:
            return call_0_makeAdd(0, 3, 0, 2);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_12(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_12(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Ast:
            // shift
            push_stack(&parser::state_3, &parser::gotof_3, value);
            return false;
        case Token_Sla:
            // shift
            push_stack(&parser::state_4, &parser::gotof_4, value);
            return false;
        case Token_Pls:
        case Token_Mns:
        case Token_RParen:
        case Token_0:
            return call_0_makeSub(0, 3, 0, 2);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_13(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_13(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Ast:
            // shift
            push_stack(&parser::state_3, &parser::gotof_3, value);
            return false;
        case Token_Sla:
            // shift
            push_stack(&parser::state_4, &parser::gotof_4, value);
            return false;
        case Token_Pls:
            // shift
            push_stack(&parser::state_5, &parser::gotof_5, value);
            return false;
        case Token_Mns:
            // shift
            push_stack(&parser::state_6, &parser::gotof_6, value);
            return false;
        case Token_0:
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

    bool gotof_14(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_14(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Ast:
        case Token_Sla:
        case Token_Pls:
        case Token_Mns:
        case Token_RParen:
        case Token_0:
            return call_0_makeId(0, 1);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_15(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_15(token_type token, const value_type& value)
    {
        switch(token){
        case Token_Ast:
        case Token_Sla:
        case Token_Pls:
        case Token_Mns:
        case Token_RParen:
        case Token_0:
            return call_1_makeId(0, 3, 1);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

};

} // namespace Parser

#endif // SAMPLE_CPP_
