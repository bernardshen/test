#include "bitmap.hpp"
#include "table.hpp"

void testBitmap() {
    char *buf = (char *)malloc(4);
    memset(buf, 0, 4);
    uint64_t count = 32;
    Bitmap bmap(count, buf);

    printf("sizeof buf %d\n", sizeof(buf));
    printf("sizeof *buf %d\n", sizeof(*buf));

    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());

    printf("\nbmap.set(1)\n");
    bmap.set(1);
    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());

    printf("\nbmap.set(10)\n");
    bmap.set(10);
    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());

    printf("\nbmap.set(1)\n");
    bmap.set(1);
    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());

    uint64_t pos;
    printf("\nbmap.findFree()\n");
    bmap.findFree(&pos);
    printf("pos: %d\n", pos);
    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());

    printf("\nbmap.set(pos)\n");
    bmap.set(pos);
    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());

    printf("\nbmap.findFree()\n");
    bmap.findFree(&pos);
    printf("pos: %d\n", pos);
    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());

    printf("\nbmap.clear(1)\n");
    bmap.clear(1);
    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());

    printf("\nbmap.findFree()\n");
    bmap.findFree(&pos);
    printf("pos: %d\n", pos);
    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());

    bool out;
    printf("\nbmap.get(1)\n");
    bmap.get(1, &out);
    printf("out: %d\n", out);
    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());

    printf("\nbmap.get(10)\n");
    bmap.get(10, &out);
    printf("out: %d\n", out);
    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());

    printf("\nbmap.clear(10)\n");
    bmap.clear(10);
    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());

    printf("\nbmap.get(10)\n");
    bmap.get(10, &out);
    printf("out: %d\n", out);
    printf("buf: 0x%llx\n", *(int*)(buf));
    printf("countFree: %lld\n", bmap.countFree());
    printf("countTotal: %lld\n", bmap.countTotal());
}

void testTable() {
    char *buffer = (char *)malloc(100);
    Table<int> *table = new Table<int>(buffer, 8);
    bool status = false;
    uint64_t index = -1;
    int item = 1001;

    printf("create index\n");
    status = table->create(&index);
    printf("create: %d\n", status);
    printf("created index: %d\n", index);

    printf("\ncreate index item %d\n", item);
    status = table->create(&index, &item);
    printf("create: %d\n", status);
    printf("create index: %d\n", index);
    printf("get index %d\n", index);
    status = table->get(index, &item);
    printf("get: %d\n", status);
    printf("item: %d\n", item);

    item = 10001;
    printf("\nput index %d item %d\n", index, item);
    status = table->put(index, &item);
    printf("put: %d\n", status);
    item = -1;
    printf("get index %d\n", index);
    status = table->get(index, &item);
    printf("get: %d\n", status);
    printf("item: %d\n", item);

    printf("\nremove index %d\n", index);
    status = table->remove(index);
    printf("remove: %d\n", status);
    printf("put removed index %d\n", index);
    status = table->put(index, &item);
    printf("put: %d\n", status);
    printf("get removed index %d\n", index);
    status = table->get(index, &item);
    printf("get: %d\n", status);
    
    item = 11111;
    printf("\ncreate index item %d\n", item);
    status = table->create(&index, &item);
    printf("create: %d\n", status);
    printf("create index: %d\n", index);
    printf("get index %d\n", index);
    status = table->get(index, &item);
    printf("get: %d\n", status);
    printf("item: %d\n", item);
}

int main() {
    // testBitmap();
    testTable();
    return 0;
}