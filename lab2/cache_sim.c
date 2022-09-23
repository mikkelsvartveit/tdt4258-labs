#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef enum
{
    dm,
    fa
} cache_map_t;
typedef enum
{
    uc,
    sc
} cache_org_t;
typedef enum
{
    instruction,
    data
} access_t;

typedef struct
{
    uint32_t address;
    access_t accesstype;
} mem_access_t;

typedef struct
{
    uint64_t accesses;
    uint64_t hits;
    // You can declare additional statistics if
    // you like, however you are now allowed to
    // remove the accesses or hits
} cache_stat_t;

// DECLARE CACHES AND COUNTERS FOR THE STATS HERE

uint32_t cache_size;
uint32_t block_size = 64;
cache_map_t cache_mapping;
cache_org_t cache_org;

// USE THIS FOR YOUR CACHE STATISTICS
cache_stat_t cache_statistics;

/* Reads a memory access from the trace file and returns
 * 1) access type (instruction or data access
 * 2) memory address
 */
mem_access_t read_transaction(FILE *ptr_file)
{
    char buf[1000];
    char *token;
    char *string = buf;
    mem_access_t access;

    if (fgets(buf, 1000, ptr_file) != NULL)
    {
        /* Get the access type */
        token = strsep(&string, " \n");
        if (strcmp(token, "I") == 0)
        {
            access.accesstype = instruction;
        }
        else if (strcmp(token, "D") == 0)
        {
            access.accesstype = data;
        }
        else
        {
            printf("Unkown access type\n");
            exit(0);
        }

        /* Get the access type */
        token = strsep(&string, " \n");
        access.address = (uint32_t)strtol(token, NULL, 16);

        return access;
    }

    /* If there are no more entries in the file,
     * return an address 0 that will terminate the infinite loop in main
     */
    access.address = 0;
    return access;
}

int main(int argc, char **argv)
{
    printf("Cache Simulator\n");

    // Reset statistics:
    memset(&cache_statistics, 0, sizeof(cache_stat_t));

    /* Read command-line parameters and initialize:
     * cache_size, cache_mapping and cache_org variables
     */
    /* IMPORTANT: *IF* YOU ADD COMMAND LINE PARAMETERS (you really don't need to),
     * MAKE SURE TO ADD THEM IN THE END AND CHOOSE SENSIBLE DEFAULTS SUCH THAT WE
     * CAN RUN THE RESULTING BINARY WITHOUT HAVING TO SUPPLY MORE PARAMETERS THAN
     * SPECIFIED IN THE UNMODIFIED FILE (cache_size, cache_mapping and cache_org)
     */
    if (argc != 4)
    { /* argc should be 2 for correct execution */
        printf(
            "Usage: ./cache_sim [cache size: 128-4096] [cache mapping: dm|fa] "
            "[cache organization: uc|sc]\n");
        exit(0);
    }
    else
    {
        /* argv[0] is program name, parameters start with argv[1] */

        /* Set cache size */
        cache_size = atoi(argv[1]);

        /* Set Cache Mapping */
        if (strcmp(argv[2], "dm") == 0)
        {
            cache_mapping = dm;
        }
        else if (strcmp(argv[2], "fa") == 0)
        {
            cache_mapping = fa;
        }
        else
        {
            printf("Unknown cache mapping\n");
            exit(0);
        }

        /* Set Cache Organization */
        if (strcmp(argv[3], "uc") == 0)
        {
            cache_org = uc;
        }
        else if (strcmp(argv[3], "sc") == 0)
        {
            cache_org = sc;
        }
        else
        {
            printf("Unknown cache organization\n");
            exit(0);
        }
    }

    /* Open the file mem_trace.txt to read memory accesses */
    FILE *ptr_file;
    ptr_file = fopen("mem_trace_long.txt", "r");
    if (!ptr_file)
    {
        printf("Unable to open the trace file\n");
        exit(1);
    }

    // Initialize cache
    int number_of_blocks = cache_size / block_size;
    // Array of uint32_t
    uint32_t cache[number_of_blocks];
    memset(cache, 0, number_of_blocks * sizeof(u_int32_t));

    // Counter(s) used for FIFO queue in case of accosiative mapping
    int counter = 0;
    int instruction_counter = 0;
    int data_counter = 0;

    /* Loop until whole trace file has been read */
    mem_access_t access;
    while (1)
    {
        access = read_transaction(ptr_file);
        // If no transactions left, break out of loop
        if (access.address == 0)
            break;
        // printf("%d %x\n", access.accesstype, access.address);
        /* Do a cache access */

        // Increment accesses no matter what
        cache_statistics.accesses++;

        uint32_t address = access.address;

        if (cache_mapping == fa)
        {
            // Since the cache block size is 64 bytes, 6 bits of the address
            // will be used for the offset inside the cache block.
            // We remove the 6 least significant bits to get only the tag part of the address
            int tag = access.address >> 6;

            bool in_cache = false;
            for (int i = 0; i < number_of_blocks; i++)
            {
                if (cache[i] == tag)
                {
                    in_cache = true;
                    break;
                }
            }

            if (in_cache)
            {
                cache_statistics.hits++;
            }
            else
            {
                if (cache_org == sc)
                {
                    if (access.accesstype == instruction)
                    {
                        cache[instruction_counter] = tag;
                        instruction_counter += (instruction_counter + 1) % (number_of_blocks / 2);
                    }
                    else if (access.accesstype == data)
                    {
                        cache[number_of_blocks / 2 + data_counter] = tag;
                        instruction_counter += (instruction_counter + 1) % (number_of_blocks / 2);
                    }
                }
                else if (cache_org == uc)
                {
                    cache[counter] = tag;
                    counter = (counter + 1) % number_of_blocks;
                }
            }
        }

        else if (cache_mapping == dm)
        {
            int number_of_index_bits;
            if (cache_org == sc)
            {
                number_of_index_bits = log2(number_of_blocks);
            }
            else if (cache_org == uc)
            {
                number_of_index_bits = log2(number_of_blocks / 2);
            }

            uint32_t index = (access.address >> 6) & ((1 << number_of_index_bits) - 1);
            uint32_t tag = access.address >> (6 + number_of_index_bits);

            if (cache[index] == tag)
            {
                cache_statistics.hits++;
            }
            else
            {
                cache[index] = tag;
            }
        }
    }

    /* Print the statistics */
    // DO NOT CHANGE THE FOLLOWING LINES!
    printf("\nCache Statistics\n");
    printf("-----------------\n\n");
    printf("Accesses: %ld\n", (long)cache_statistics.accesses);
    printf("Hits:     %ld\n", (long)cache_statistics.hits);
    printf("Hit Rate: %.4f\n",
           (double)cache_statistics.hits / cache_statistics.accesses);
    // DO NOT CHANGE UNTIL HERE
    // You can extend the memory statistic printing if you like!

    /* Close the trace file */
    fclose(ptr_file);

    return 0;
}
