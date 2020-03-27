#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>
#include <set>
#include <unordered_map>

using namespace llvm;


namespace {

class LoopInvariantCodeMotion final : public LoopPass
{
private:
    DominatorTree * dom_tree;  // owned by `DominatorTreeWrapperPass`
    std::set<BasicBlock *> bb_set;
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
        auto header_block = L->getLoopPreheader();
        if (!header_block)
        {
            // If we cannot insert a preheader, don't bother.
            return false;
        }
        else
        {
            // Initialize a set for all basic blocks in the loop
            bb_set.clear();

            // Instructions that we can yeet to the preheader
            SmallVector<Instruction*, 24> yeetable_instructions;

            for (auto bb : L->getBlocks())
            {
                bb_set.insert(bb); 
            }

            std::unordered_map<Instruction *, bool> invariant_instructions;
            for (BasicBlock * bb_ptr : L->getBlocks())
            {
                for (auto iter = bb_ptr->begin(); iter != bb_ptr->end(); ++iter)
                {
                    Instruction * I = &(*iter);
                    bool is_invariant = isInstructionLoopInvariant(I, invariant_instructions);
                    if (is_invariant)
                    {
                        I->print(outs()); outs() << " is invariant by our algorithm\n";
                    }
                    else
                    {
                        I->print(outs()); outs() << " is not invariant by our algorithm\n";
                    }
                    bool dominates_uses = dominateUsesWithinLoop(I);
                    bool dominates_exits = dominateExits(I, L);
                    if (is_invaraint && dominates_uses && dominates_exits)
                    {
                        yeetable_instructions.push_back(I);
                    }
                }
            }
        }
        // Clear set of basic blocks so function can be run on other loops
        bb_set.clear();
        return false;
    }

    /*
     * Check if Instruction at inst is loop invariant. Use memoization to 
     * speed up process.
     */
    bool isInstructionLoopInvariant(Instruction * inst, std::unordered_map<Instruction *, bool> & invar_map);

    /*
     * Check if an instruction dominates all of its uses within a loop
     */
    bool dominateUsesWithinLoop(Instruction * I);

    /*
     * Check if an instruction dominates all exit blocks
     */
    bool dominateExits(Instruction *I, Loop * L)
    {
        SmallVector<BasicBlock*, 8> ExitingBlocks;
        L->getExitingBlocks(ExitingBlocks);

        for(auto exit : ExitingBlocks)
        {
            if (!dom_tree->dominates(I, exit))
            {
                return false;
            }
        }
        return true;
    }

    /*
     * Return true if instruction is a branch, return, or phi(?)
     * Such instructions should NEVER be hoisted.
     */
    static inline bool doesInstructionAlterFlow(Instruction * inst);
};


bool LoopInvariantCodeMotion::isInstructionLoopInvariant(Instruction * inst, std::unordered_map<Instruction *, bool> & invar_map)
{
    if (doesInstructionAlterFlow(inst))
    {
        invar_map.insert({inst, false});
        return false;
    }

    // Check if all operands are defined outsidse the loop
    bool all_ops_outside = true;
    bool all_ops_invariant_and_defined_once = true;
    for (auto it = inst->op_begin(); it != inst->op_end(); ++it)
    {
        Value * operand_val = *it;
        Instruction * operand_inst = dyn_cast<Instruction>(operand_val);
        if (operand_inst != NULL)
        {
            BasicBlock * parent_bblock = operand_inst->getParent();
            if (bb_set.find(parent_bblock) != bb_set.end()) // If parent is in loop
            {
                all_ops_outside = false;
            }
        }
    }

    for (auto it = inst->op_begin(); it != inst->op_end(); ++it)
    {
        Value * operand_val = *it;
        Instruction * operand_inst = dyn_cast<Instruction>(operand_val);
        if (operand_inst != NULL)
        {
            /* Instructions may also be invariant if operands themselves are loop invariant with one definition inside the loop.
             * In SSA form, everything only has one definition, period. So instead we check if it is a PHI Instruction that is
             * inside the loop.
             */
            bool is_curr_operand_invariant = false;
            if (invar_map.find(operand_inst) != invar_map.end())
            {
                is_curr_operand_invariant = invar_map[operand_inst];
            }
            else
            {
                is_curr_operand_invariant = this->isInstructionLoopInvariant(operand_inst, invar_map); 
            }

            if (is_curr_operand_invariant)
            {
                if (auto * phi_inst = dyn_cast<PHINode>(operand_inst))
                {
                    // If it is a phi instruction, it's still fine as long as the phi instruction is defined outside the loop
                    BasicBlock * phi_parent = phi_inst->getParent(); 
                    if (bb_set.find(phi_parent) != bb_set.end())
                    {
                        all_ops_invariant_and_defined_once = false;
                    }
                }
            }
            else
            {
                all_ops_invariant_and_defined_once = false;
            }
        }
    }

    invar_map.insert({inst, (all_ops_outside || all_ops_invariant_and_defined_once)});
    return all_ops_outside || all_ops_invariant_and_defined_once;
}

bool LoopInvariantCodeMotion::dominateUsesWithinLoop(Instruction * I)
{
    for (User * U : I->users())
    {
        if (auto used_inst = dyn_cast<Instruction>(U))
        {
            if (this->bb_set.find(used_inst->getParent()) != this->bb_set.end()) // We are only interested in uses within the loop
            {
                if (!dom_tree->dominates(I, used_inst))
                {
                    outs() << "        Instruction does not dominate "; used_inst->print(outs()); outs() << "\n";
                    return false;
                }
            }
        }
    }
    return true;
}

inline bool LoopInvariantCodeMotion::doesInstructionAlterFlow(Instruction * inst)
{
    return (isa<BranchInst>(inst) || isa<IndirectBrInst>(inst) || isa<ReturnInst>(inst)
            || isa<PHINode>(inst));
}

char LoopInvariantCodeMotion::ID = 0;

RegisterPass < LoopInvariantCodeMotion > X (
    "loop-invariant-code-motion",
    "Loop Invariant Code Motion");

}  // namespace anonymous
