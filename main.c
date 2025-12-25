#include <stdio.h>
#include <stdlib.h>
#include "models.h"
#include "utils.h"

// Define global heads
User *users_head = NULL;
Club *clubs_head = NULL;
Transaction *tx_head = NULL;
Activity *activities_head = NULL;
FundRequest *requests_head = NULL;

int main() {
    load_data();
    // TODO: Main menu loop
    save_data();
    return 0;
}