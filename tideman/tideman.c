#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool lock_checking_helper(int a, int b);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}





// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i=0;i<candidate_count;i++)
    {
        if (!strcmp(name, candidates[i]))
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i=0;i<candidate_count;i++)
    {
        for (int j=i+1;j<candidate_count;j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i=0;i<candidate_count;i++)
    {
        for (int j=i+1;j<candidate_count;j++)
        {
            if (preferences[i][j]<preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
            else if (preferences[i][j]>preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        for (int j = 0; j < pair_count - i - 1; j++)
        {
            int arr[4] = {
                pairs[j].winner,
                pairs[j].loser,
                pairs[j+1].winner,
                pairs[j+1].loser
            };
            int str1 = preferences[arr[0]][arr[1]] - preferences[arr[1]][arr[0]];
            int str2 = preferences[arr[2]][arr[3]] - preferences[arr[3]][arr[2]];
            if (str1 < str2)
            {
                pair swap = pairs[j];
                pairs[j] = pairs[j + 1];
                pairs[j + 1] = swap;
            }
        }
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i=0;i<pair_count;i++)
    {
        int arr[2];
        arr[0] = pairs[i].winner;
        arr[1] = pairs[i].loser;
        locked[arr[0]][arr[1]] = true;
        if(lock_checking_helper(arr[0],arr[1]))
        {
            locked[arr[0]][arr[1]] = false;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    int n;
    for(int i=0;i<candidate_count;i++)
    {
        n = 0;
        for(int j=0;j<candidate_count;j++)
        {
            if (!locked[j][i])
            {
                n++;
            }
        }
        if (n==candidate_count)
        {
            printf("%s\n", candidates[i]);
            return;
        }
    }
    return;
}

bool lock_checking_helper(int a, int b)
{
    if (candidate_count<=1)
    {
        return false;
    }
    if (a==b)
    {
        return true;
    }
    for (int i=0;i<candidate_count;i++)
    {
        if(locked[b][i] && lock_checking_helper(a,i))
        {
            return true;
        }
    }
    return false;
}
