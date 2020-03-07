#include "dfa/framework.h"
#include <unordered_set>

// @TODO 

namespace {
class Variable
{
private:
        const Value * _val;
public:
        Variable(const Value & val):
        _val(&val)
        {
        }

        bool operator==(const Variable & other) const
        {
                return (this->getVal() == other.getVal());
        }

        const Value * getVal() const { return _val; }


        friend raw_ostream & operator<<(raw_ostream & outs, const Variable & var);
}; 

raw_ostream & operator<<(raw_ostream & outs, const Variable & var)
{
        outs << "[";
        var.getVal()->printAsOperand(outs, false);
        outs << "]";
        return outs;
}

}

namespace std {

// Construct a hash code for 'Expression'.
template <>
struct hash < Variable >
{
        std::size_t operator()(const Variable & var) const
        {
                std::hash < const Value * > pvalue_hasher;
                std::size_t val_hash = pvalue_hasher((var.getVal()));

                return (val_hash << 1) ^ (1934773);
        }
};
}

namespace {

class Liveness final : public dfa::Framework < Variable,
                                               dfa::Direction::Backward > 
{
public:
        static char ID;

        Liveness() : dfa::Framework<domain_element_t, direction_c>(ID) {}
        virtual ~Liveness() override {}

        virtual void getAnalysisUsage(AnalysisUsage &AU) const override
        {
                AU.setPreservesAll();
        }

        virtual BitVector IC() const override
        {
                return BitVector(_domain.size(), false);
        }
        virtual BitVector BC() const override
        {
                return BitVector(_domain.size(), false);
        }
        virtual BitVector MeetOp(const meetop_const_range & meet_operands) const override
        {
                //return BitVector(_domain.size());
                BitVector ret = BitVector(_domain.size(), false); 
                
                for (auto succ : meet_operands) {
                        Instruction* first_inst = LLVMGetFirstInstruction(succ);
                        auto first_bv = _inst_bv_map.at(first_inst);
                        ret |= first_bv;
                }
                return ret;
        }
        virtual bool TransferFunc(const Instruction & inst,
                                  const BitVector & ibv,
                                  BitVector & obv) override
        {
                return false;
        }
        virtual void InitializeDomainFromInstruction(const Instruction & inst) override
        {
                // Add each operand
                for (auto iter = inst.op_begin(); iter != inst.op_end(); ++iter)
                {
                        Value * val = *iter;
                        if (isa<Instruction>(val) || isa<Argument>(val))
                        {
                                try
                                {
                                        _domain.emplace(*val);
                                }
                                catch (const std::invalid_argument & ia_except) {}        
                        }
                }
        }
protected:
};

char Liveness::ID = 1; 
RegisterPass < Liveness > Y ("liveness", "Liveness");

} // namespace anonymous
