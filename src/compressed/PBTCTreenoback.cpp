//
// Created by elarielcl on 25/09/18.
//

#include <compressed/PBTCTreenoback.h>
#include <unordered_set>

PBTCTreenoback::PBTCTreenoback(std::string& input_string, int block_tree_version, int r, int mll, int c) {
    std::unordered_set<int> characters;
    for (char c: input_string) {
        characters.insert(c);
    }

    PBlockTree* bt = new PBlockTree(input_string, r, mll);

    switch (block_tree_version) {
        case PAPER:
            bt->process_back_pointers();
            bt->clean_unnecessary_expansions();
            break;
        case PAPER_NO_CLEAN:
            bt->process_back_pointers();
            break;
        case PRUNIG_C:
            bt->process_back_pointers();
            bt->clean_unnecessary_expansions(c);
            break;
        case HEURISTIC:
            bt->process_back_pointers_heuristic();
            break;
        case LIBERAL:
            bt->process_back_pointers_liberal_heuristic();
            break;
        case CONSERVATIVE:
            bt->process_back_pointers_conservative_heuristic();
            break;
    }

    for (int c: characters)
        bt->add_rank_select_support(c);
    bt->add_rank_select_leaf_support();
    bt->add_fwdsearch_support();
    bt->add_max_fields();
    bt->add_preffix_min_count_fields();

    bps_ = new BPSPCBlockTree(bt);
    delete bt;
}

PBTCTreenoback::~PBTCTreenoback() {
    delete bps_;
}

int PBTCTreenoback::root() {
    return root_;
}

int PBTCTreenoback::first_child(int node) {
    // check whether node+1 is (
    int a = bps_->access(node+1);
    return (a == '(') ? node+1 : -1;
}

int PBTCTreenoback::last_child(int node) {
    // check whether node+1 is (
    int c = bps_->my_fwdsearch_nobackmin(node, -1);
    return bps_->my_bwdsearch_nobackmin(c-1 ,0) + 1;
}

int PBTCTreenoback::next_sibling(int node) {
    // check whether close(node)+1 is (
    int a = bps_->my_fwdsearch_nobackmin(node, -1) + 1;
    int b = bps_->access(a);
    return (b == '(')? a : -1;
}

int PBTCTreenoback::previous_sibling(int node) {
    // check whether node != root and node-1 = )
    return bps_->my_bwdsearch_nobackmin(node-1, 0) + 1;
}

int PBTCTreenoback::parent(int node) {
    // check whether node != root
    if (node == root_) return  root_;
    return bps_->my_bwdsearch_nobackmin(node, -2) + 1;
}


bool PBTCTreenoback::is_leaf(int node) {
    return bps_->open_ != bps_->access(node+1);
}


bool PBTCTreenoback::is_leaf_rank(int node, int& leaf_rank) {
    return bps_->is_leaf_rank_alternative_noback(node+1, leaf_rank);
}

int PBTCTreenoback::node_map(int node) {
    return bps_->rank_1_alternative_noback(node);
}


int PBTCTreenoback::node_select(int i) {
    return bps_->select_1_alternative_noback(i);
}

int PBTCTreenoback::postorder(int node) {
    int c = bps_->my_fwdsearch_nobackmin(node, -1);
    return bps_->rank_0_alternative_noback(c);
}

int PBTCTreenoback::postorder_select(int i) {
    int s = bps_->select_0_alternative_noback(i);
    return bps_->my_bwdsearch_nobackmin(s, 0) + 1;
}

int PBTCTreenoback::depth(int node) {
    return bps_->excess_alternative_noback(node);
}

int PBTCTreenoback::subtree_size(int node) {
    int c = bps_->my_fwdsearch_nobackmin(node ,-1);
    return (c-node+1)/2;
}

bool PBTCTreenoback::is_ancestor(int node_a, int node_d) {
    if (node_a > node_d) return false;
    int c = bps_->my_fwdsearch_nobackmin(node_a, -1);
    return node_d <= c;
}

int PBTCTreenoback::level_ancestor(int node, int d) {
    return bps_->my_bwdsearch_nobackmin(node, -d-1) + 1;
}


int PBTCTreenoback::leaf_rank(int node) {
    return bps_->rank_10_alternative_noback(node) + 1;
}

int PBTCTreenoback::number_of_leaves(int node) {
    int c = bps_->my_fwdsearch_nobackmin(node, -1);
    return leaf_rank(c) - leaf_rank(node);
}

int PBTCTreenoback::leaf_select(int node) {
    return bps_->select_10_alternative_noback(node);
}

int PBTCTreenoback::lca(int node1, int node2) {
    if (node1 == node2) return node1;
    if (node1 > node2) {
        int t = node1;
        node1 = node2;
        node2 = t;
    }
    int m = bps_->my_min_excess_nobackmin(node1, node2);
    int im = bps_->my_fwdsearch_nobackmin(node1-1, m);
    return bps_->my_bwdsearch_nobackmin(im + 1, -2) + 1;
}

int PBTCTreenoback::child_rank(int node) {
    // Check whether node != root
    int p = bps_->my_bwdsearch_nobackmin(node, -2) + 1;
    return bps_->my_min_count_nobackmin(p,node);
}

int PBTCTreenoback::child(int node, int t) {
    // Check whether t <= number of children of node
    int c = bps_->my_fwdsearch_nobackmin(node, -1);
    return bps_->my_min_select_nobackmin(node, c-2, t) + 1;
}

int PBTCTreenoback::number_of_children(int node) {
    // Check whether t <= number of children of node
    int c = bps_->my_fwdsearch_nobackmin(node, -1);
    return bps_->my_min_count_nobackmin(node, c-2);
}

int PBTCTreenoback::deepest_node(int node) {
    int c = bps_->my_fwdsearch_nobackmin(node, -1);
    int M = bps_->my_max_excess_nobackmax(node, c);
    return bps_->my_positive_fwdsearch_nobackmax(node-1, M);
}

int PBTCTreenoback::height(int node) {
    return depth(deepest_node(node)) - depth(node);
}

void PBTCTreenoback::print_statistics() {
    bps_->print_statistics();
}

int PBTCTreenoback::get_size() {
    int size = 0;

    // Bit Vectors Size(B)
    size += sizeof(void*);
    for (sdsl::bit_vector* bv : bps_->bt_bv_) {
        size += sdsl::size_in_bytes(*bv);
    }
    // Bit Vectors Size(B)

    // Bit Vectors Rank Size(B)
    size += sizeof(void*);
    for (sdsl::rank_support_v<1>* bvr : bps_->bt_bv_rank_) {
        size += sdsl::size_in_bytes(*bvr);
    }
    // Bit Vectors Rank Size(B)

    // Offsets Size Plain(B)
    size += sizeof(void*);
    for (sdsl::int_vector<>* offsets: bps_->bt_offsets_) {
        size += sdsl::size_in_bytes(*offsets);
    }
    // Offsets Size Plain(B)

    // Leaves String Size(B)
    size += sdsl::size_in_bytes(*(bps_->leaf_string_));
    // Leaves String Size(B)

    // Mapping Size(B)
    size += sizeof(int) * 256;
    // Mapping Size(B)

    // Alphabet Size(B)
    size += sdsl::size_in_bytes(*(bps_->alphabet_));
    // Alphabet Size(B)


    // first_level_prefix_ranks Plain(B)
    for (auto pair: bps_->bt_first_level_prefix_ranks_) {
        size += sdsl::size_in_bytes(*(pair.second));
    }
    // first_level_prefix_ranks Plain(B)


    // ranks Size Plain(B)
    size += (bps_->bt_ranks_.size()+1) * sizeof(void*);
    for (auto pair: bps_->bt_ranks_) {
        for (sdsl::int_vector<>* ranks: pair.second) {
            size += sdsl::size_in_bytes(*ranks);
        }
    }
    // ranks Size Plain(B)

    // first_level_prefix_leaf_ ranks Plain(B)
    size += sdsl::size_in_bytes(*(bps_->bt_first_level_prefix_leaf_ranks_));
    // first_level_prefix_leaf_ ranks Plain(B)

    // leaf_ranks Size Plain(B)
    size += sizeof(void *);
    for (sdsl::int_vector<> *vector : bps_->bt_leaf_ranks_) {
        size += sdsl::size_in_bytes(*vector);
    }
    // leaf_ranks Size Plain(B)

    // starts_with_end_leaf Plain(B)
    size += sizeof(void *);
    for (sdsl::bit_vector *bv : bps_->bt_starts_with_end_leaf_) {
        size += sdsl::size_in_bytes(*bv);
    }
    // starts_with_end_leaf Plain(B)

    // suffix_start_with_end_leaf Plain(B)
    size += sizeof(void *);
    for (sdsl::bit_vector *bv : bps_->bt_suffix_start_with_end_leaf_) {
        size += sdsl::size_in_bytes(*bv);
    }
    // suffix_start_with_end_leaf Plain(B)

    // Top Excess Size(B)
    size += sdsl::size_in_bytes(*(bps_->top_excess_));
    // Top Excess Size(B)

    // Top Min Excess Size(B)
    size += sdsl::size_in_bytes(*(bps_->top_min_excess_));
    // Top Min Excess Size(B)

    // Min Excess Size(B)
    size += sizeof(void*);
    for (sdsl::int_vector<>* vector: bps_->bt_prefix_min_excess_) {
        size += sdsl::size_in_bytes(*vector);
    }
    // Min Excess Size(B)

    return size;
}