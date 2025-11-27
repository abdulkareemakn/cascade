#pragma once
#include "database.h"

void initStorage() { getStorage().sync_schema(); }
