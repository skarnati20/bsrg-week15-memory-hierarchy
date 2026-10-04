#include "../06_indices_vs_pointers/exercise.h"

void flatten_tree(const PointerNode *root, CompactNode *out_arena, size_t *out_count) {
    if (!root) return;
    flatten_tree(root->left, out_arena, out_count);
    flatten_tree(root->right, out_arena, out_count);
    size_t i = (*out_count)++;
    out_arena[i].op = (uint8_t)root->op;
    out_arena[i].val = root->val;
}

int64_t eval_compact_ast(const CompactNode *nodes, size_t count) {
    int64_t stack[64];
    int sp = 0;
    for (size_t i = 0; i < count; i++) {
        uint8_t op = nodes[i].op;
        if (op == OP_CONST) {
            stack[sp++] = nodes[i].val;
        } else {
            int64_t r = stack[--sp];
            int64_t l = stack[--sp];
            if (op == OP_ADD) stack[sp++] = l + r;
            else if (op == OP_SUB) stack[sp++] = l - r;
            else stack[sp++] = (l * r) & 0xFF;
        }
    }
    return stack[0];
}
