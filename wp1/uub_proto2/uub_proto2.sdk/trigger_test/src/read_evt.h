#include <shwr_evt_defs.h>

int read_evt_init();
int read_evt_end();
int read_evt_read(struct shwr_evt_raw *shwr);
int read_evt_read2(struct shwr_evt_raw *shwr1,struct shwr_evt_raw *shwr2);
