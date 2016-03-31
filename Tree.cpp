#include "Tree.h"

Tree::~Tree() {
    for (Tree *ct : freeList) delete ct;
}

void Tree::AddChild(Tree *ct) {
    kids.push_back(ct);
}

Tree* Tree::GetChild(int n) {
    assert (n < kids.size() && "GetChild index must be smaller than kids size");
    return kids[n];
}

void Tree::CastInt(llvm::ConstantInt *cnst) {
    freeList.push_back(new Tree(IMM));
    Tree *ct = freeList.back();
    const llvm::APInt integer = cnst->getValue();
    unsigned bitWidth = cnst->getBitWidth();
    if (integer.isSignedIntN(bitWidth)) {
        switch (bitWidth) {
            case 1:
                ct->SetValue((bool) cnst->getSExtValue());
                break;
            case 8:
                ct->SetValue((int8_t)cnst->getSExtValue());
                break;
            case 16:
                ct->SetValue((int16_t)cnst->getSExtValue());
                break;
            case 32:
                ct->SetValue((int32_t)cnst->getSExtValue());
                break;
            case 64:
                ct->SetValue((int64_t)cnst->getSExtValue());
                break;
            default:
                std::cerr << "CAST CONSTANT INT FAILURE" << std::endl;
                exit(EXIT_FAILURE);
        }
    }
    else {
        switch (bitWidth) {
            case 1:
                ct->SetValue((bool) cnst->getZExtValue());
                break;
            case 8:
                ct->SetValue((int8_t)cnst->getZExtValue());
                break;
            case 16:
                ct->SetValue((int16_t)cnst->getZExtValue());
                break;
            case 32:
                ct->SetValue((int32_t)cnst->getZExtValue());
                break;
            case 64:
                ct->SetValue((int64_t)cnst->getZExtValue());
                break;
            default:
                std::cerr << "CAST CONSTANT INT FAILURE" << std::endl;
                exit(EXIT_FAILURE);
        }
    }
    this->AddChild(ct);
}

void Tree::CastFP(llvm::ConstantFP *cnst) {
    freeList.push_back(new Tree(IMM));
    Tree *ct = freeList.back();
    // for now, assume all floating point uses 64 bit double
    ct->SetValue((double) cnst->getValueAPF().convertToDouble());
    this->AddChild(ct);
}

void Tree::DisplayTree(Tree *t, int indent) {
    for (int i = 0; i < 2 * indent; i++)
        std::cerr << " ";
    switch (t->op) {
        case DUMMY:
            std::cerr << "op: " << "dummy" << std::endl;
            break;
        case LABEL:
            std::cerr << "op: " << "label" << std::endl;
            break;
        case REG:
            std::cerr << "op: " << "reg" << std::endl;
            break;
        case IMM:
            std::cerr << "op: " << "imm" << std::endl;
            break;
        case MEM:
            std::cerr << "op: " << "mem" << std::endl;
            break;
        default:
            std::cerr << "op: " << llvm::Instruction::getOpcodeName(t->op) << "(" << t->op << ")" << "\tval: " << t->val.val.i32s << std::endl;
    }
    std::cerr << "child num: " << kids.size() << std::endl;
    // for (int i = 0; i < kids.size(); i++)
    //     DisplayTree(kids[i], indent + 1);
}

