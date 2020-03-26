#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>
#include <set>

using namespace llvm;


namespace {

class LoopInvariantCodeMotion final : public LoopPass
{
private:
    DominatorTree * dom_tree;  // owned by `DominatorTreeWrapperPass`
    std::set<BlockT> bb_set;
public:
    static char ID;
    
    LoopInvariantCodeMotion() : LoopPass(ID) {}
    virtual ~LoopInvariantCodeMotion() override {}

    virtual void getAnalysisUsage(AnalysisUsage & AU) const
    {
        AU.addRequired < DominatorTreeWrapperPass > ();
        AU.setPreservesCFG();
    }

    /// @todo Finish the implementation of this method.
    virtual bool runOnLoop(Loop * L, LPPassManager & LPM)
    {
        dom_tree = &(getAnalysis < DominatorTreeWrapperPass > ().getDomTree());

        // Initialize a set for all basic blocks in the loop
        for (auto bb : L.getBlocks()){
            bb_set.insert(bb); 
        }
        /*
        for (auto node = GraphTraits<DominatorTree *>::nodes_begin(dom_tree); node != GraphTraits<DominatorTree *>::nodes_end(dom_tree); ++node)
        {
            BasicBlock * BB = node->getBlock();
            outs() << "BASIC BLOCK "  << BB->getName() << "\n";
            for (auto & I : (*BB))
            {
                I.print(outs());
                outs() << "\n";
            }
        }
        */
        auto header_block = L->getLoopPreheader();
        if (!header_block)
        {
            // If we cannot insert a preheader, don't bother.
            return false;
        }
        else
        {
        }
        return false;

        // Clear set of basic blocks so function can be run on other loops
        bb_set.clear();
    }

    bool isInstructionInvariant(Instruction * I, Loop * L)
    {
        return false;
    }

    bool dominateExits(Instruction *I, Loop * L){

        SmallVector<BasicBlock*, 8> ExitingBlocks;
        L->getExitingBlocks(ExitingBlocks);

        for(auto exit : ExitingBlocks){
            if (!dom_tree.dominates(I, exit)){
                return false;
            }
        }

        return true;
    }

    bool dominateUses(Instruction * I){
        return false:
    }


    
};

char LoopInvariantCodeMotion::ID = 0;

RegisterPass < LoopInvariantCodeMotion > X (
    "loop-invariant-code-motion",
    "Loop Invariant Code Motion");

}  // namespace anonymous
