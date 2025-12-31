#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    printf("16. Create backup\n");
    printf("17. Restore backup\n");
    printf("18. Exit\n");
    printf("Choose an option: ");
}

int main() {
    load_data();
    int choice = 0;
    char buf[512];
    
    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) { 
            int c; 
            while ((c = getchar()) != '\n' && c != EOF); 
            continue; 
        }
        
        // Clear input buffer after reading choice
        int c; 
        while ((c = getchar()) != '\n' && c != EOF);

        int club_id, user_id, role, new_role, criteria, activity_id;
        double amount;
        char name[256], start[64], end[64];

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
                // Consume newline left by scanf
                while ((c = getchar()) != '\n' && c != EOF);
                printf("Enter description: "); 
                if (!fgets(buf, sizeof(buf), stdin)) buf[0]='\0'; 
                else { size_t l=strlen(buf); if(l && buf[l-1]=='\n') buf[l-1]='\0'; }
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
                printf("Enter search keyword: "); 
                if (!fgets(buf, sizeof(buf), stdin)) buf[0]='\0'; 
                else { size_t l=strlen(buf); if(l && buf[l-1]=='\n') buf[l-1]='\0'; }
                search_clubs_fuzzy(buf);
                break;
            case 12:
                printf("Sort by: 1. ID, 2. Name, 3. Balance: "); scanf("%d", &criteria);
                sort_clubs(criteria);
                break;
            case 13:
                printf("Enter club ID: "); scanf("%d", &club_id);
                while ((c = getchar()) != '\n' && c != EOF); // Clear buffer
                printf("Enter activity name: ");
                if (!fgets(name, sizeof(name), stdin)) name[0]='\0';
                else { size_t l=strlen(name); if(l && name[l-1]=='\n') name[l-1]='\0'; }
                
                printf("Enter start time (YYYY-MM-DD HH:MM): ");
                if (!fgets(start, sizeof(start), stdin)) start[0]='\0';
                else { size_t l=strlen(start); if(l && start[l-1]=='\n') start[l-1]='\0'; }
                
                printf("Enter end time (YYYY-MM-DD HH:MM): ");
                if (!fgets(end, sizeof(end), stdin)) end[0]='\0';
                else { size_t l=strlen(end); if(l && end[l-1]=='\n') end[l-1]='\0'; }
                
                publish_activity(club_id, name, start, end);
                break;
            case 14:
                printf("Enter activity ID: "); scanf("%d", &activity_id);
                printf("Enter user ID: "); scanf("%d", &user_id);
                register_for_activity(activity_id, user_id);
                break;
            case 15:
                printf("Enter club ID (0 for all): "); scanf("%d", &club_id);
                list_activities(club_id);
                break;
            case 16:
                create_backup();
                printf("Backup created. See backups/ folder.\n");
                break;
            case 17:
                printf("Enter backup directory (e.g. backups/backup_YYYYMMDD_HHMMSS): "); 
                if (!fgets(buf, sizeof(buf), stdin)) buf[0]='\0'; 
                else { size_t l=strlen(buf); if(l && buf[l-1]=='\n') buf[l-1]='\0'; }
                restore_backup(buf);
                printf("Restore attempted. Restart program to reload data.\n");
                break;
            case 18:
                printf("Exiting...\n");
                save_data();
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}
