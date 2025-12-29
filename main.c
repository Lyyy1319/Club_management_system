#include <stdio.h>
#include <stdlib.h>
#include "models.h"
#include "utils.h"
#include "club.h"
#include "finance.h"

// Define global heads
User *users_head = NULL;
Club *clubs_head = NULL;
Transaction *tx_head = NULL;
Activity *activities_head = NULL;
FundRequest *requests_head = NULL;

void print_menu() {
    printf("\nClub Management System\n");
    printf("1. Add member to club\n");
    printf("2. Remove member from club\n");
    printf("3. Change member role\n");
    printf("4. Generate overall financial report\n");
    printf("5. Generate club financial report\n");
    printf("6. Generate fund requests report\n");
    printf("7. Validate data associations\n");
    printf("8. Exit\n");
    printf("Choose an option: ");
}

int main() {
    load_data();
    int choice;
    do {
        print_menu();
        scanf("%d", &choice);
        switch (choice) {
            case 1: {
                int club_id, user_id, role;
                printf("Enter club ID: ");
                scanf("%d", &club_id);
                printf("Enter user ID: ");
                scanf("%d", &user_id);
                printf("Enter role (0=Member, 1=President, 2=Admin, 3=Officer): ");
                scanf("%d", &role);
                add_member_to_club(club_id, user_id, (Role)role);
                break;
            }
            case 2: {
                int club_id, user_id;
                printf("Enter club ID: ");
                scanf("%d", &club_id);
                printf("Enter user ID: ");
                scanf("%d", &user_id);
                remove_member_from_club(club_id, user_id);
                break;
            }
            case 3: {
                int club_id, user_id, new_role;
                printf("Enter club ID: ");
                scanf("%d", &club_id);
                printf("Enter user ID: ");
                scanf("%d", &user_id);
                printf("Enter new role (0=Member, 1=President, 2=Admin, 3=Officer): ");
                scanf("%d", &new_role);
                change_member_role(club_id, user_id, (Role)new_role);
                break;
            }
            case 4:
                generate_financial_report();
                break;
            case 5: {
                int club_id;
                printf("Enter club ID: ");
                scanf("%d", &club_id);
                generate_club_financial_report(club_id);
                break;
            }
            case 6:
                generate_fund_requests_report();
                break;
            case 7:
                validate_data_associations();
                break;
            case 8:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 8);
    save_data();
    return 0;
}