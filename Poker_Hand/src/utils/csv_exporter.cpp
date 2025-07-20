#include "csv_exporter.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>

namespace poker {

void CSVExporter::export_position_action_results(
    const std::vector<ActionResult>& results,
    const std::string& position,
    const std::string& action,
    TableSize table_size,
    const std::string& output_base_path) {
    
    // Split results by win rate threshold
    auto [high_winrate, low_winrate] = split_by_winrate(results, 0.60);
    
    // Get base directory path
    std::string table_folder = get_table_size_folder(table_size);
    std::string base_path = output_base_path + "/" + table_folder + "/" + position + "/" + action;
    
    // Ensure directories exist
    ensure_directory_exists(base_path);
    
    // Export high win rate hands
    std::string high_path = base_path + "/high_winrate_hands.csv";
    write_csv_file(high_winrate, high_path);
    
    // Export low win rate hands
    std::string low_path = base_path + "/low_winrate_hands.csv";
    write_csv_file(low_winrate, low_path);
    
    std::cout << "Exported " << high_winrate.size() << " high win rate hands to " << high_path << std::endl;
    std::cout << "Exported " << low_winrate.size() << " low win rate hands to " << low_path << std::endl;
}

void CSVExporter::create_folder_structure(const std::string& output_base_path) {
    // Create 6-max folder structure
    std::vector<std::string> positions_6max = {"UTG", "HJ", "CO", "BTN", "SB", "BB"};
    std::vector<std::string> actions = {"opening_raise", "3_bet", "4_bet", "call"};
    
    for (const auto& position : positions_6max) {
        for (const auto& action : actions) {
            std::string path = output_base_path + "/6_player/" + position + "/" + action;
            ensure_directory_exists(path);
        }
    }
    
    // Create 9-max folder structure
    std::vector<std::string> positions_9max = {"UTG", "UTG1", "MP1", "MP2", "HJ", "CO", "BTN", "SB", "BB"};
    
    for (const auto& position : positions_9max) {
        for (const auto& action : actions) {
            std::string path = output_base_path + "/9_player/" + position + "/" + action;
            ensure_directory_exists(path);
        }
    }
    
    std::cout << "Created complete folder structure in " << output_base_path << std::endl;
}

void CSVExporter::export_all_results(
    const std::vector<ActionResult>& all_results,
    TableSize table_size,
    const std::string& output_base_path) {
    
    // Group results by position and action
    std::map<std::pair<std::string, std::string>, std::vector<ActionResult>> grouped_results;
    
    for (const auto& result : all_results) {
        if (result.player_count == static_cast<int>(table_size)) {
            std::pair<std::string, std::string> key = {result.position, result.action};
            grouped_results[key].push_back(result);
        }
    }
    
    // Export each group
    for (const auto& [key, results] : grouped_results) {
        const auto& [position, action] = key;
        export_position_action_results(results, position, action, table_size, output_base_path);
    }
}

std::pair<std::vector<ActionResult>, std::vector<ActionResult>> 
CSVExporter::split_by_winrate(const std::vector<ActionResult>& results, double threshold) {
    std::vector<ActionResult> high_winrate, low_winrate;
    
    for (const auto& result : results) {
        if (result.win_rate > threshold) {
            high_winrate.push_back(result);
        } else {
            low_winrate.push_back(result);
        }
    }
    
    // Sort by win rate (high to low)
    std::sort(high_winrate.begin(), high_winrate.end(), 
              [](const ActionResult& a, const ActionResult& b) {
                  return a.win_rate > b.win_rate;
              });
    
    std::sort(low_winrate.begin(), low_winrate.end(), 
              [](const ActionResult& a, const ActionResult& b) {
                  return a.win_rate > b.win_rate;
              });
    
    return {high_winrate, low_winrate};
}

void CSVExporter::write_csv_file(
    const std::vector<ActionResult>& results,
    const std::string& file_path) {
    
    std::ofstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << file_path << std::endl;
        return;
    }
    
    // Write header
    file << get_csv_header() << std::endl;
    
    // Write data rows
    for (const auto& result : results) {
        file << action_result_to_csv_row(result) << std::endl;
    }
    
    file.close();
}

std::string CSVExporter::get_table_size_folder(TableSize table_size) {
    return (table_size == TableSize::SIX_MAX) ? "6_player" : "9_player";
}

void CSVExporter::ensure_directory_exists(const std::string& directory_path) {
    try {
        std::filesystem::create_directories(directory_path);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating directory " << directory_path << ": " << e.what() << std::endl;
    }
}

std::string CSVExporter::get_csv_header() {
    return "hand,position,action,win_rate,player_count,simulations_run,expected_value,confidence_interval_low,confidence_interval_high,in_range";
}

std::string CSVExporter::action_result_to_csv_row(const ActionResult& result) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    
    oss << result.hand << ","
        << result.position << ","
        << result.action << ","
        << result.win_rate << ","
        << result.player_count << ","
        << result.simulations_run << ","
        << result.expected_value << ","
        << result.confidence_interval_low << ","
        << result.confidence_interval_high << ","
        << (result.in_range ? "true" : "false");
    
    return oss.str();
}

} // namespace poker
