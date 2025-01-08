# Storage Carving Tools

Storage Carving Tools is a project developed as part of **Smart India Hackathon 2024**, focused on efficient data recovery from filesystems. Written in **C++** with **CUDA** support, it currently supports **XFS** and will soon add **BTRFS** support. This project is designed to handle data carving with enhanced performance and accuracy.

## Overview

### Features
- **Filesystem-Aware Carving**: Parses filesystem structures (currently XFS) to focus on unallocated locations.
- **Fragmentation Handling**: Appropriately manages fragmented data to improve recovery precision.
- **CUDA-Accelerated Processing**: Uses CUDA to speed up the carving process for large datasets.

### Design Highlights
- **XFS Structure Parsing**: Extracts metadata and filesystem layout information to identify unallocated blocks.
- **Unallocated Space Scanning**: Targets only unused storage areas, reducing time and unnecessary reads.
- **Parallel Data Processing**: Leverages GPU parallelism to enhance performance and efficiency.

### Learning Objectives
This project aims to:
- Deepen understanding of filesystem internals (XFS and BTRFS).
- Apply CUDA programming to real-world data processing challenges.
- Improve data carving techniques with a focus on performance and correctness.

### Limitations
- Currently limited to XFS; BTRFS support is under development.
- Limited recovery capabilities compared to full-featured file recovery suites.

## Future Improvements
- Add support for BTRFS filesystem parsing and carving.
- Implement advanced file-type-specific carving strategies.
- Optimize CUDA kernel designs for further performance gains.

