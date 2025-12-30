#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    printf("4. Add Transaction (Income/Expense)\n");
    printf("5. List Transactions\n");
    printf("6. Verify Club Balance\n");
    int main() {
        load_data();
        int choice = 0;
        printf("7. Generate overall financial report\n");
        printf("8. Generate club financial report\n");
        printf("9. Generate fund requests report\n");
        printf("10. Validate data associations\n");
        printf("11. Search Clubs (Fuzzy)\n");
        printf("12. Sort Clubs\n");
        printf("13. Create backup\n");
        printf("14. Restore backup\n");
        printf("15. Exit\n");
        char buf[512];
        while (1) {
            print_menu();
            if (scanf("%d", &choice) != 1) { int c; while ((c = getchar()) != '\n' && c != EOF); continue; }
            int club_id, user_id, role, new_role, criteria;
            double amount;
            int c; while ((c = getchar()) != '\n' && c != EOF);
            switch (choice) {
                case 1:
                    printf("Enter club ID: "); scanf("%d", &club_id);
                    printf("Enter user ID: "); scanf("%d", &user_id);
                    printf("Enter role (0=Member,1=President,2=Admin,3=Officer): "); scanf("%d", &role);
                    add_member_to_club(club_id, user_id, (Role)role);
                    break;
                case 2:
                    printf("Enter club ID: "); scanf("%d", &club_id);
                    printf("Enter user ID: "); scanf("%d", &user_id);
                    remove_member_from_club(club_id, user_id);
                    break;
                case 3:
                    printf("Enter club ID: "); scanf("%d", &club_id);
                    printf("Enter user ID: "); scanf("%d", &user_id);
                    printf("Enter new role (0=Member,1=President,2=Admin,3=Officer): "); scanf("%d", &new_role);
                    add_member_to_club(club_id, user_id, (Role)new_role);
                    break;
                case 4:
                    printf("Enter club ID: "); scanf("%d", &club_id);
                    printf("Enter amount (positive income, negative expense): "); scanf("%lf", &amount);
                    printf("Enter description: "); if (!fgets(buf, sizeof(buf), stdin)) buf[0]='\0'; else { size_t l=strlen(buf); if(l && buf[l-1]=='\n') buf[l-1]='\0'; }
                    add_transaction(club_id, amount, buf);
                    break;
                case 5:
                    printf("Enter club ID: "); scanf("%d", &club_id);
                    list_transactions(club_id);
                    break;
                case 6:
                    printf("Enter club ID: "); scanf("%d", &club_id);
                    verify_balance(club_id);
                    break;
                case 7:
                    generate_financial_report();
                    break;
                case 8:
                    printf("Enter club ID: "); scanf("%d", &club_id);
                    generate_club_financial_report(club_id);
                    break;
                case 9:
                    generate_fund_requests_report();
                    break;
                case 10:
                    validate_data_associations();
                    break;
                case 11:
                    printf("Enter search keyword: "); if (!fgets(buf, sizeof(buf), stdin)) buf[0]='\0'; else { size_t l=strlen(buf); if(l && buf[l-1]=='\n') buf[l-1]='\0'; }
                    search_clubs_fuzzy(buf);
                    break;
                case 12:
                    printf("Sort by: 1. ID, 2. Name, 3. Balance: "); scanf("%d", &criteria);
                    sort_clubs(criteria);
                    break;
                case 13:
                    create_backup();
                    printf("Backup created. See backups/ folder.\n");
                    break;
                case 14:
                    printf("Enter backup directory (e.g. backups/backup_YYYYMMDD_HHMMSS): "); if (!fgets(buf, sizeof(buf), stdin)) buf[0]='\0'; else { size_t l=strlen(buf); if(l && buf[l-1]=='\n') buf[l-1]='\0'; }
                    restore_backup(buf);
                    printf("Restore attempted. Restart program to reload data.\n");
                    break;
                case 15:
                    printf("Exiting...\n");
                    save_data();
                    return 0;
                default:
                    printf("Invalid choice.\n");
            }
        }
        return 0;
    }
", dir);
                restore_backup(dir);
                printf("Restore attempted. Restart program to reload data.\n");
                break;
            }
            case 15:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 15);
    save_data();
    return 0;
}
                    printf("Choose an option: ");
                }

                int main() {
                    load_data();
                    int choice;
                    do {
                        print_menu();
                        if (scanf("%d", &choice) != 1) {
                            int c; while ((c = getchar()) != '\n' && c != EOF);
                            choice = -1;
                        }
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
                                scanf(" %[^
                ]", desc);
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
                                scanf(" %[^
                ]", keyword);
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
                                create_backup();
                                printf("Backup created. See backups/ folder.\n");
                                break;
                            }
                            case 14: {
                                char dir[256];
                                printf("Enter backup directory (e.g. backups/backup_YYYYMMDD_HHMMSS): ");
                                scanf(" %[^
                ]", dir);
                                restore_backup(dir);
                                printf("Restore attempted. Restart program to reload data.\n");
                                break;
                            }
                            case 15:
                                printf("Exiting...\n");
                                break;
                            default:
                                printf("Invalid choice.\n");
                        }
                    } while (choice != 15);
                    save_data();
                    return 0;
                }
