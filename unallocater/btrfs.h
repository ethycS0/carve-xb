#ifndef BTRFS
#define BTRFS

#include "common.h"

// Device information structure
struct btrfs_dev_item {
        uint64_t devid;                     // Device ID
        uint64_t total_bytes;               // Total bytes
        uint64_t bytes_used;                // Bytes used
        uint32_t io_align;                  // IO alignment
        uint32_t io_width;                  // IO width
        uint32_t sector_size;               // Sector size
        uint64_t type;                      // Device type
        uint64_t generation;                // Generation
        uint64_t start_offset;              // Start offset
        uint32_t dev_group;                 // Device group
        uint8_t  seek_speed;                // Seek speed
        uint8_t  bandwidth;                 // Bandwidth
        uint8_t  uuid[16];                  // Device UUID
        uint8_t  fsid[16];                  // Filesystem UUID
};

struct btrfs_super_block {
        uint8_t  csum[32];                  // Checksum of superblock
        uint8_t  fsid[16];                  // Filesystem UUID
        uint64_t bytenr;                    // Physical address of this block
        uint64_t flags;                     // Superblock flags
        uint64_t magic;                     // BTRFS_MAGIC ('_BHRfS_M')
        uint64_t generation;                // Generation number
        uint64_t root;                      // Root tree root
        uint64_t chunk_root;                // Chunk tree root
        uint64_t log_root;                  // Log tree root
        uint64_t total_bytes;               // Total bytes used
        uint64_t bytes_used;                // Bytes used
        uint64_t root_dir_objectid;         // Root dir objectid
        uint64_t num_devices;               // Number of devices
        uint32_t sectorsize;                // Sector size in bytes
        uint32_t nodesize;                  // Node size in bytes
        uint32_t leafsize;                  // Leaf size in bytes
        uint32_t stripesize;                // Stripe size in bytes
        uint32_t sys_chunk_array_size;      // System chunk array size
        uint64_t chunk_root_generation;     // Chunk root generation
        uint64_t compat_flags;              // Compatible feature flags
        uint64_t compat_ro_flags;           // Compatible read-only feature flags
        uint64_t incompat_flags;            // Incompatible feature flags
        uint16_t csum_type;                 // Checksum type
        uint8_t  root_level;                // Root level
        uint8_t  chunk_root_level;          // Chunk root level
        uint8_t  log_root_level;            // Log root level
        struct btrfs_dev_item dev_item;     // Device information
        char label[256];                    // Filesystem label
        uint64_t cache_generation;          // Cache generation
        uint64_t uuid_tree_generation;      // UUID tree generation
        uint8_t  metadata_uuid[16];         // Metadata UUID
        uint8_t  reserved[176];             // Reserved for future use
};

struct btrfs_extent_item {
        uint64_t refs;              // Reference count
        uint64_t generation;        // Generation number
        uint64_t flags;            // Extent flags
};

struct btrfs_extent_inline_ref {
        uint64_t offset;           // Offset for the reference
        uint8_t type;              // Reference type
        uint8_t pad[7];            // Padding for alignment
};

struct btrfs_tree_block_info {
        uint64_t key;              // Key of the block
        uint8_t level;             // Level in the tree
};

struct btrfs_extent_data_ref {
        uint64_t root;             // Root objectid
        uint64_t objectid;         // Owner objectid
        uint64_t offset;           // Offset within the file
        uint32_t count;            // Reference count
};

struct btrfs_shared_data_ref {
        uint32_t count;            // Reference count
};

// Extent tree key types
enum btrfs_extent_key_type {
        BTRFS_EXTENT_ITEM_KEY       = 168,  // Regular extent
        BTRFS_METADATA_ITEM_KEY     = 169,  // Metadata extent
        BTRFS_EXTENT_DATA_REF_KEY   = 178,  // Data reference
        BTRFS_SHARED_BLOCK_REF_KEY  = 182,  // Shared block reference
        BTRFS_SHARED_DATA_REF_KEY   = 184   // Shared data reference
};

// Extent flags
enum btrfs_extent_flags {
        BTRFS_EXTENT_FLAG_DATA      = 0x001,  // Data extent
        BTRFS_EXTENT_FLAG_TREE_BLOCK = 0x002, // Tree block extent
        BTRFS_BLOCK_FLAG_FULL_BACKREF = 0x004 // Full backref
};

// Header for extent tree nodes
struct btrfs_extent_header {
        uint8_t  csum[32];          // Checksum of the node
        uint8_t  fsid[16];          // Filesystem UUID
        uint64_t bytenr;            // Block number
        uint64_t flags;             // Node flags
        uint8_t  chunk_tree_uuid[16]; // Chunk tree UUID
        uint64_t generation;         // Generation number
        uint64_t owner;             // Owner of the node
        uint32_t nritems;           // Number of items
        uint8_t  level;             // Level in the tree
};

// Key structure for extent entries
struct btrfs_disk_key {
        uint64_t objectid;          // Object ID (usually starting block number)
        uint8_t  type;              // Type of the key
        uint64_t offset;            // Offset or size
};

// Complete extent entry
struct btrfs_extent_entry {
        struct btrfs_disk_key key;
        uint32_t offset;            // Offset in the node
        uint32_t size;              // Size of the item
        union {
                struct btrfs_extent_item extent;
                struct btrfs_extent_inline_ref inline_ref;
                struct btrfs_extent_data_ref data_ref;
                struct btrfs_shared_data_ref shared_ref;
        } data;
};


#endif // !BTRFS
