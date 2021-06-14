#include "lanox2d/lanox2d.h"

typedef struct lx_demo_entry_t_ {
    lx_list_entry_t     entry;
    lx_size_t           data;
}lx_demo_entry_t;

int main(int argc, char** argv) {

    lx_demo_entry_t entries[12] = {
        {{0}, 0}
    ,   {{0}, 1}
    ,   {{0}, 2}
    ,   {{0}, 3}
    ,   {{0}, 4}
    ,   {{0}, 5}
    ,   {{0}, 6}
    ,   {{0}, 7}
    ,   {{0}, 8}
    ,   {{0}, 9}
    ,   {{0}, 10}
    ,   {{0}, 11}
    };

    // init the list
    lx_list_entry_head_t list;
    lx_list_entry_init(&list, lx_demo_entry_t, entry);

    // insert entries
    lx_list_entry_insert_tail(&list, &entries[5].entry);
    lx_list_entry_insert_tail(&list, &entries[6].entry);
    lx_list_entry_insert_tail(&list, &entries[7].entry);
    lx_list_entry_insert_tail(&list, &entries[8].entry);
    lx_list_entry_insert_tail(&list, &entries[9].entry);
    lx_list_entry_insert_head(&list, &entries[4].entry);
    lx_list_entry_insert_head(&list, &entries[3].entry);
    lx_list_entry_insert_head(&list, &entries[2].entry);
    lx_list_entry_insert_head(&list, &entries[1].entry);
    lx_list_entry_insert_head(&list, &entries[0].entry);
    lx_demo_entry_t* entry = (lx_demo_entry_t*)lx_list_entry(&list, &entries[5].entry);
    lx_trace_i("entry: %lu", entry->data);
    lx_trace_i("");
    lx_trace_i("insert: %lu", lx_list_entry_size(&list));
    lx_for_all(lx_demo_entry_t*, item0, lx_list_entry_itor(&list)) {
        lx_trace_i("%lu", item0->data);
    }
    lx_trace_i("");

    // replace entries
    lx_list_entry_replace_head(&list, &entries[10].entry);
    lx_list_entry_replace_last(&list, &entries[11].entry);
    lx_trace_i("replace: %lu", lx_list_entry_size(&list));
    lx_for_all(lx_demo_entry_t*, item1, lx_list_entry_itor(&list)) {
        lx_trace_i("%lu", item1->data);
    }
    lx_trace_i("");

    // remove entries
    lx_list_entry_remove_head(&list);
    lx_list_entry_remove_last(&list);
    lx_trace_i("remove: %lu", lx_list_entry_size(&list));
    lx_for_all(lx_demo_entry_t*, item2, lx_list_entry_itor(&list)) {
        lx_trace_i("%lu", item2->data);
    }
    lx_trace_i("");

    // moveto entries
    lx_list_entry_ref_t head = lx_list_entry_head(&list);
    lx_list_entry_moveto_head(&list, lx_list_entry_last(&list));
    lx_list_entry_moveto_tail(&list, head);
    lx_trace_i("moveto: %lu", lx_list_entry_size(&list));
    lx_for_all(lx_demo_entry_t*, item3, lx_list_entry_itor(&list)) {
        lx_trace_i("%lu", item3->data);
    }
    lx_trace_i("");

    // clear entries
    lx_list_entry_clear(&list);
    lx_trace_i("clear: %lu", lx_list_entry_size(&list));
    lx_for_all(lx_demo_entry_t*, item5, lx_list_entry_itor(&list)) {
        lx_trace_i("%lu", item5->data);
    }

    lx_list_entry_exit(&list);
    lx_trace_i("");
    return 0;
}

