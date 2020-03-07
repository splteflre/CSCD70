#include "dfa/framework.h"

namespace {

class Expression
{
private:
        unsigned _opcode; const Value * _lhs, * _rhs;
public:
        Expression(const Instruction & inst)
        {
                this->_opcode = inst.getOpcode();
                this->_lhs = inst.getOperand(0);
                this->_rhs = inst.getOperand(1);
        }

        bool operator==(const Expression & Expr) const
        {
                if (this->operationIsCommutative())
                {
                        bool isExactSame = (this->_opcode == Expr._opcode && this->_lhs == Expr._lhs && this->_rhs == Expr._rhs);
                        bool switchedOperands = (this->getOpcode() == Expr.getOpcode() && this->getLHSOperand() == Expr.getRHSOperand() 
                                                 && this->getRHSOperand() == Expr.getLHSOperand());
                        return isExactSame || switchedOperands;
                }
                else
                {
                        return (this->_opcode == Expr._opcode && this->_lhs == Expr._lhs && this->_rhs == Expr._rhs);
                }
        }

        unsigned getOpcode() const { return _opcode; }
        const Value * getLHSOperand() const { return _lhs; }
        const Value * getRHSOperand() const { return _rhs; }

        bool operationIsCommutative() const;
        
        friend raw_ostream & operator<<(raw_ostream & outs, const Expression & expr);
}; 

raw_ostream & operator<<(raw_ostream & outs, const Expression & expr)
{
        outs << "[" << Instruction::getOpcodeName(expr._opcode) << " ";
        expr._lhs->printAsOperand(outs, false); outs << ", ";
        expr._rhs->printAsOperand(outs, false); outs << "]";

        return outs;
}

bool Expression::operationIsCommutative() const
{
        bool out;
        unsigned operation = this->getOpcode();
        switch(operation)
        {
                case Instruction::Add:
                case Instruction::FAdd:
                case Instruction::Mul:
                case Instruction::FMul:
                case Instruction::And:
                case Instruction::Or:
                case Instruction::Xor:
                        out = true;
                        break;
                default:
                        out = false;
                        break;
        }
        return out;
}

}  // namespace anonymous

namespace std {

// Construct a hash code for 'Expression'.
template <>
struct hash < Expression >
{
        std::size_t operator()(const Expression & expr) const
        {
                std::hash < unsigned > unsigned_hasher; std::hash < const Value * > pvalue_hasher;

                std::size_t opcode_hash = unsigned_hasher(expr.getOpcode());
                std::size_t lhs_operand_hash = pvalue_hasher((expr.getLHSOperand()));
                std::size_t rhs_operand_hash = pvalue_hasher((expr.getRHSOperand()));

                return opcode_hash ^ (lhs_operand_hash << 1) ^ (rhs_operand_hash << 1);
        }
};

}  // namespace std

namespace {

class AvailExpr final : public dfa::Framework < Expression, 
                                                dfa::Direction::Forward >
{
protected:
        virtual BitVector IC() const override
        {
                return BitVector(_domain.size(), true);
        }
        virtual BitVector BC() const override
        {
                return BitVector(_domain.size());
        }
        virtual BitVector MeetOp(const meetop_const_range & meet_operands) const override
        {
                // @TODO
                
                BitVector ret = BitVector(_domain.size(), true);

                for(auto pred : meet_operands) {
                        Instruction* last_inst = LLVMGetLastInstruction(pred);
                        auto last_bv = _inst_bv_map.at(last_inst);
                        ret &= last_bv;
                }
                return ret;
                // return BitVector(_domain.size());
        }
        virtual bool TransferFunc(const Instruction & inst,
                                  const BitVector & ibv,
                                  BitVector & obv) override
        {
                /*
                BitVector bv_prime = ibv;
                //const Value inst_v = inst;

                for (auto prev_exp : _domain){
                    if (prev_exp.getLHSOperand() == &(cast<Value>(inst)) || prev_exp.getRHSOperand() == &(cast<Value>(inst))){
                        int idx = std::distance(_domain.begin(), _domain.find(inst));
                        bv_prime.set(idx, 0); 
                    }
                }
                Expression cur_exp = Expression(inst);
                auto found = _domain.find(cur_exp);
                if(found != _domain.end()){
                    int idx = std::distance(_domain.begin(), found);
                    bv_prime.set(idx, 1);
                }
                bool hasChange = (ibv == bv_prime);
                obv = bv_prime;

                // @TODO
                return hasChange;
                */
                return false;
        }
        virtual void InitializeDomainFromInstruction(const Instruction & inst) override
        {
                // try to insert the instruction into the domain, and throw an
                // `invalid_argument` exception if failed
                try {
                        if (isa<BinaryOperator>(inst))
                        {
                                _domain.emplace(inst);
                        }
                } catch (const std::invalid_argument & ia_except) {}
        }
public:
        static char ID;

        AvailExpr() : dfa::Framework < domain_element_t, 
                                       direction_c > (ID) {}
        virtual ~AvailExpr() override {}
};

char AvailExpr::ID = 1; 
RegisterPass < AvailExpr > Y ("avail_expr", "Available Expression");

} // namespace anonymous
