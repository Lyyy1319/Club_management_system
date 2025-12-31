#include <stdio.h>
#include <stdlib.h>
#include "models.h"
#include "utils.h"
#include "club.h"
#include "finance.h"
#include "activity.h"

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
    printf("4. Add Transaction (Income/Expense)\n");
    printf("5. List Transactions\n");
    printf("6. Verify Club Balance\n");
    printf("7. Generate overall financial report\n");
    printf("8. Generate club financial report\n");
    printf("9. Generate fund requests report\n");
    printf("10. Validate data associations\n");
    printf("11. Search Clubs (Fuzzy)\n");
    printf("12. Sort Clubs\n");
    printf("13. Publish Activity\n");
    printf("14. Register for Activity\n");
    printf("15. List Activities\n");
    printf("16. Exit\n");
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
                // Use add_member_to_club to update role as per club.c implementation
                add_member_to_club(club_id, user_id, (Role)new_role);
                break;
            }
            case 4: {
                int club_id;
                double amount;
                char desc[256];
                printf("Enter club ID: ");
                scanf("%d", &club_id);
                printf("Enter amount (positive for income, negative for expense): ");
                scanf("%lf", &amount);
                printf("Enter description: ");
                scanf(" %[^\n]", desc); // Read string with spaces
                add_transaction(club_id, amount, desc);
                break;
            }
            case 5: {
                int club_id;
                printf("Enter club ID: ");
                scanf("%d", &club_id);
                list_transactions(club_id);
                break;
            }
            case 6: {
                int club_id;
                printf("Enter club ID: ");
                scanf("%d", &club_id);
                verify_balance(club_id);
                break;
            }
            case 7:
                generate_financial_report();
                break;
            case 8: {
                int club_id;
                printf("Enter club ID: ");
                scanf("%d", &club_id);
                generate_club_financial_report(club_id);
                break;
            }
            case 9:
                generate_fund_requests_report();
                break;
            case 10:
                validate_data_associations();
                break;
            case 11: {
                char keyword[100];
                printf("Enter search keyword: ");
                scanf(" %[^\n]", keyword);
                search_clubs_fuzzy(keyword);
                break;
            }
            case 12: {
                int criteria;
                printf("Sort by: 1. ID, 2. Name, 3. Balance: ");
                scanf("%d", &criteria);
                sort_clubs(criteria);
                break;
            }
            case 13: {
                int club_id;
                char name[256], start[64], end[64];
                printf("Enter club ID: ");
                scanf("%d", &club_id);
                printf("Enter activity name: ");
                scanf(" %[^\n]", name);
                printf("Enter start time (YYYY-MM-DD HH:MM): ");
                scanf(" %[^\n]", start);
                printf("Enter end time (YYYY-MM-DD HH:MM): ");
                scanf(" %[^\n]", end);
                publish_activity(club_id, name, start, end);
                break;
            }
            case 14: {
                int activity_id, user_id;
                printf("Enter activity ID: ");
                scanf("%d", &activity_id);
                printf("Enter user ID: ");
                scanf("%d", &user_id);
                register_for_activity(activity_id, user_id);
                break;
            }
            case 15: {
                int club_id;
                printf("Enter club ID (0 for all): ");
                scanf("%d", &club_id);
                list_activities(club_id);
                break;
            }
            case 16:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 16);
    save_data();
    return 0;
}
