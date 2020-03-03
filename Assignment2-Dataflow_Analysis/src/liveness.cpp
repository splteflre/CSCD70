#include "dfa/framework.h"
#include <unordered_set>

// @TODO 

namespace {
class Variable
{
private:
        const Value * _val;
public:
        bool operator==(const Variable & other) const
        {
                // @TODO
                return false;
        }

        const Value * getVal() const { return _val; }

        //friend raw_ostream & operator<<(raw_ostream & outs, const Expression & expr);

}; 
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

        Liveness() : FunctionPass(ID) {}
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
};

char Liveness::ID = 1; 
RegisterPass < Liveness > Y ("liveness", "Liveness");

} // namespace anonymous
