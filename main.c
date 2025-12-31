// Cleaned-up main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "models.h"
#include "utils.h"
#include "club.h"
#include "finance.h"
#include "activity.h"

// Global heads
User *users_head = NULL;
Club *clubs_head = NULL;
Transaction *tx_head = NULL;
Activity *activities_head = NULL;
FundRequest *requests_head = NULL;

static void print_menu() {
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
}

static void strip_newline(char *s) {
    if (!s) return;
    size_t l = strlen(s);
    if (l>0 && s[l-1]=='\n') s[l-1]='\0';
}

int main() {
    load_data();
<<<<<<< HEAD
    int choice = -1;
    char buf[512];
    while (1) {
        print_menu();
        printf("Choose an option: ");
        if (scanf("%d", &choice) != 1) { int c; while ((c = getchar()) != '\n' && c != EOF); continue; }
        int club_id, user_id, role, new_role, criteria;
        double amount;
        // consume remainder newline
        int ch = getchar(); (void)ch;
        switch (choice) {
            case 1:
                printf("Enter club ID: "); if (scanf("%d", &club_id)!=1) break; printf("Enter user ID: "); if (scanf("%d", &user_id)!=1) break; printf("Enter role (0=Member,1=President,2=Admin,3=Officer): "); if (scanf("%d", &role)!=1) break; add_member_to_club(club_id, user_id, (Role)role); break;
            case 2:
                printf("Enter club ID: "); if (scanf("%d", &club_id)!=1) break; printf("Enter user ID: "); if (scanf("%d", &user_id)!=1) break; remove_member_from_club(club_id, user_id); break;
            case 3:
                printf("Enter club ID: "); if (scanf("%d", &club_id)!=1) break; printf("Enter user ID: "); if (scanf("%d", &user_id)!=1) break; printf("Enter new role (0=Member,1=President,2=Admin,3=Officer): "); if (scanf("%d", &new_role)!=1) break; change_member_role(club_id, user_id, (Role)new_role); break;
            case 4:
                printf("Enter club ID: "); if (scanf("%d", &club_id)!=1) break; printf("Enter amount (positive income, negative expense): "); if (scanf("%lf", &amount)!=1) break; while ((ch = getchar()) != '\n' && ch != EOF); printf("Enter description: "); if (!fgets(buf, sizeof(buf), stdin)) buf[0]='\0'; strip_newline(buf); add_transaction(club_id, amount, buf); break;
            case 5:
                printf("Enter club ID: "); if (scanf("%d", &club_id)!=1) break; list_transactions(club_id); break;
            case 6:
                printf("Enter club ID: "); if (scanf("%d", &club_id)!=1) break; verify_balance(club_id); break;
            case 7:
                generate_financial_report(); break;
            case 8:
                printf("Enter club ID: "); if (scanf("%d", &club_id)!=1) break; generate_club_financial_report(club_id); break;
            case 9:
                generate_fund_requests_report(); break;
            case 10:
                validate_data_associations(); break;
            case 11:
                while ((ch = getchar()) != '\n' && ch != EOF);
                printf("Enter search keyword: "); if (!fgets(buf, sizeof(buf), stdin)) buf[0]='\0'; strip_newline(buf); search_clubs_fuzzy(buf); break;
            case 12:
                printf("Sort by: 1. ID, 2. Name, 3. Balance: "); if (scanf("%d", &criteria)!=1) break; sort_clubs(criteria); break;
            case 13:
                create_backup(); printf("Backup created. See backups/ folder.\n"); break;
            case 14:
                while ((ch = getchar()) != '\n' && ch != EOF);
                printf("Enter backup directory (e.g. backups/backup_YYYYMMDD_HHMMSS): "); if (!fgets(buf, sizeof(buf), stdin)) buf[0]='\0'; strip_newline(buf); restore_backup(buf); printf("Restore attempted. Restart program to reload data.\n"); break;
            case 15:
                while ((ch = getchar()) != '\n' && ch != EOF);
                printf("Enter club ID for activity: "); if (scanf("%d", &club_id)!=1) break; while ((ch = getchar()) != '\n' && ch != EOF);
                printf("Enter activity name: "); if (!fgets(buf, sizeof(buf), stdin)) buf[0]='\0'; strip_newline(buf); char namebuf[256]; strncpy(namebuf, buf, sizeof(namebuf)); namebuf[sizeof(namebuf)-1]=0;
                printf("Enter start time (YYYY-MM-DD HH:MM): "); if (!fgets(buf, sizeof(buf), stdin)) buf[0]='\0'; strip_newline(buf); char startbuf[64]; strncpy(startbuf, buf, sizeof(startbuf)); startbuf[sizeof(startbuf)-1]=0;
                printf("Enter end time (YYYY-MM-DD HH:MM): "); if (!fgets(buf, sizeof(buf), stdin)) buf[0]='\0'; strip_newline(buf); char endbuf[64]; strncpy(endbuf, buf, sizeof(endbuf)); endbuf[sizeof(endbuf)-1]=0;
                create_activity(club_id, namebuf, startbuf, endbuf);
                break;
            case 16:
                printf("Enter club ID: "); if (scanf("%d", &club_id)!=1) break; list_activities_for_club(club_id); break;
            case 0:
                save_data(); printf("Exiting...\n"); return 0;
=======
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
>>>>>>> origin/main
            default:
                printf("Invalid choice.\n"); break;
        }
    }
    return 0;
}
