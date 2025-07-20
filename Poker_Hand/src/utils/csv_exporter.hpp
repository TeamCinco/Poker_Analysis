#pragma once

#include "types.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

namespace poker {

class CSVExporter {
public:
    // Export results for a specific position, action, and table size
    static void export_position_action_results(
        const std::vector<ActionResult>& results,
        const std::string& position,
        const std::string& action,
        TableSize table_size,
        const std::string& output_base_path = "output"
    );
    
    // Create the complete folder structure
    static void create_folder_structure(
        const std::string& output_base_path = "output"
    );
    
    // Export all results for a table size
    static void export_all_results(
        const std::vector<ActionResult>& all_results,
        TableSize table_size,
        const std::string& output_base_path = "output"
    );

private:
    // Split results by win rate threshold (60%)
    static std::pair<std::vector<ActionResult>, std::vector<ActionResult>> 
    split_by_winrate(const std::vector<ActionResult>& results, double threshold = 0.60);
    
    // Write CSV file with header
    static void write_csv_file(
        const std::vector<ActionResult>& results,
        const std::string& file_path
    );
    
    // Get folder path for table size
    static std::string get_table_size_folder(TableSize table_size);
    
    // Ensure directory exists
    static void ensure_directory_exists(const std::string& directory_path);
    
    // Get CSV header
    static std::string get_csv_header();
    
    // Convert ActionResult to CSV row
    static std::string action_result_to_csv_row(const ActionResult& result);
};

} // namespace poker
