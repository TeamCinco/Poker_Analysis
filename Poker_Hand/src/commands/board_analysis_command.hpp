#pragma once

#include "command_base.hpp"
#include "../core/types.hpp"
#include <vector>
#include <map>
#include <string>
#include <array>

namespace poker {

/**
 * @brief Board Texture Classifier and Analysis Command
 * 
 * This command implements a comprehensive board texture classification system
 * that forms the foundation for all postflop analysis. It classifies boards
 * by connectivity, flush potential, pairing, and strategic implications.
 */
class BoardAnalysisCommand : public CommandBase {
public:
    BoardAnalysisCommand();
    ~BoardAnalysisCommand() override = default;
    
    void execute() override;
    std::string get_name() const override;
    std::string get_description() const override;

    /**
     * @brief Core board texture classifier
     */
    class BoardTextureClassifier {
    public:
        BoardTextureClassifier();
        
        // Main classification function
        BoardAnalysis classify_board(const std::array<Card, 3>& board);
        BoardAnalysis classify_board_string(const std::string& board_str);
        
        // Utility functions for board analysis
        std::vector<std::string> get_representative_boards() const;
        std::string texture_to_string(BoardTexture texture) const;
        
    private:
        // Core analysis functions
        double calculate_connectivity_index(const std::array<Card, 3>& board);
        double calculate_flush_potential(const std::array<Card, 3>& board);
        double calculate_pair_potential(const std::array<Card, 3>& board);
        double calculate_high_card_bias(const std::array<Card, 3>& board);
        
        // Strategic frequency calculations
        double calculate_expected_cbet_freq(BoardTexture primary_texture);
        double calculate_expected_checkraise_freq(BoardTexture primary_texture);
        double calculate_range_advantage_pfr(BoardTexture primary_texture, double connectivity);
        
        // Board classification helpers
        BoardTexture determine_primary_texture(const std::array<Card, 3>& board);
        BoardTexture determine_secondary_texture(const std::array<Card, 3>& board, BoardTexture primary);
        
        bool is_paired_board(const std::array<Card, 3>& board);
        bool is_monotone_board(const std::array<Card, 3>& board);
        bool is_two_tone_board(const std::array<Card, 3>& board);
        bool is_highly_connected(const std::array<Card, 3>& board);
        bool is_broadway_heavy(const std::array<Card, 3>& board);
        bool is_wheel_texture(const std::array<Card, 3>& board);
        
        // Strategic lookup tables (based on GTO solver data)
        std::map<BoardTexture, double> cbet_frequency_table_;
        std::map<BoardTexture, double> checkraise_frequency_table_;
        std::map<BoardTexture, double> range_advantage_table_;
        
        void initialize_strategic_tables();
    };

private:
    // Command implementation functions
    void print_help() const;
    void run_board_classification_demo();
    void analyze_representative_boards();
    void generate_board_analysis_csv();
    void show_progress(int completed, int total, const std::string& current_item);
    
    // Test different board categories
    void test_dry_boards();
    void test_wet_boards(); 
    void test_paired_boards();
    void test_special_cases();
    
    // CSV export functionality
    void export_board_classifications(const std::vector<BoardAnalysis>& analyses, const std::string& filename);
    void create_output_directories();
    
    // Representative board collections
    std::vector<std::string> get_dry_boards() const;
    std::vector<std::string> get_wet_boards() const;
    std::vector<std::string> get_paired_boards() const;
    std::vector<std::string> get_monotone_boards() const;
    std::vector<std::string> get_broadway_boards() const;
    std::vector<std::string> get_wheel_boards() const;
    
    BoardTextureClassifier classifier_;
};

} // namespace poker
