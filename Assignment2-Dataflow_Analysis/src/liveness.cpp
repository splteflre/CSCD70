#include "dfa/framework.h"
#include <unordered_set>

// @TODO 

namespace {
class Variable
{
private:
        const Value * _val;
public:
        Variable(const Instruction & inst)
        {
                // @TODO
        }

        bool operator==(const Variable & other) const
        {
                // @TODO
                return false;
        }

        const Value * getVal() const { return _val; }


        friend raw_ostream & operator<<(raw_ostream & outs, const Variable & var);
}; 

raw_ostream & operator<<(raw_ostream & outs, const Variable & expr)
{
        outs << "[";
        expr._val->printAsOperand(outs, false);
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

                return (val_hash << 1) ^ (13);
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

        // @TODO Add or remove method definitions if necessary.

        virtual void getAnalysisUsage(AnalysisUsage &AU) const override
        {
                AU.setPreservesAll();
        }

        /*
        virtual bool runOnFunction(Function &func) override
        {
                return false;
        }
        */
        virtual BitVector IC() const override
        {
                // @TODO
                return BitVector(_domain.size());
        }
        virtual BitVector BC() const override
        {
                // @TODO
                return BitVector(_domain.size());
        }
        virtual BitVector MeetOp(const meetop_const_range & meet_operands) const override
        {
                // @TODO
                return BitVector(_domain.size());
        }
        virtual bool TransferFunc(const Instruction & inst,
                                  const BitVector & ibv,
                                  BitVector & obv) override
        {
                // @TODO
                return false;
        }
        virtual void InitializeDomainFromInstruction(const Instruction & inst) override
        {
        }
protected:
};

char Liveness::ID = 1; 
RegisterPass < Liveness > Y ("liveness", "Liveness");

} // namespace anonymous
