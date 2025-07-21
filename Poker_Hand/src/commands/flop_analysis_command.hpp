#pragma once

#include "command_base.hpp"
#include "board_analysis_command.hpp"
#include "../analysis/poker_analyzer.hpp"
#include "../engine/monte_carlo_engine.hpp"
#include "../core/types.hpp"
#include <vector>
#include <map>
#include <string>
#include <array>

namespace poker {

/**
 * @brief Comprehensive Flop Analysis Command (Phases 2-4)
 * 
 * This command implements comprehensive flop analysis that builds on the board texture
 * classifier to analyze c-bet frequencies, range evolution, and multi-dimensional outputs.
 * 
 * Phase 2: Board texture integration with hand analysis
 * Phase 3: Range evolution tracking through action sequences  
 * Phase 4: Multi-dimensional CSV output structure
 */
class FlopAnalysisCommand : public CommandBase {
public:
    FlopAnalysisCommand();
    ~FlopAnalysisCommand() override = default;
    
    void execute() override;
    std::string get_name() const override;
    std::string get_description() const override;

private:
    /**
     * @brief Range Evolution Tracker
     * 
     * Tracks how ranges evolve through preflop and flop action sequences
     */
    struct RangeEvolution {
        std::string sequence_name;           // e.g., "UTG_open_BTN_3bet_call"
        std::vector<std::string> initial_range;  // Starting range
        std::vector<std::string> evolved_range;  // Range after action sequence
        double range_tightness;              // How tight the range became (0-1)
        std::string board_texture;           // Board texture for postflop
        double equity_shift;                 // Change in equity vs opponent
    };

    /**
     * @brief Comprehensive Flop Result
     * 
     * Multi-dimensional result structure for CSV export
     */
    struct ComprehensiveFlopResult {
        std::string hand;                    // e.g., "AA", "KQs"
        std::string position;                // UTG, HJ, CO, BTN, SB, BB
        std::string preflop_action;          // open, 3bet, 4bet, call
        BoardAnalysis board_analysis;        // Board texture classification
        std::string flop_action;             // cbet, check, check_raise, call, fold
        double action_frequency;             // How often this action is taken (0-1)
        double win_rate_after_action;        // Win rate if taking this action
        double expected_value;               // EV of the action
        uint64_t simulations_run;            // Monte Carlo simulations
        
        // Range evolution data
        std::string range_type;              // "tight", "loose", "polarized"
        double range_advantage;              // Advantage on this board texture
        
        // Turn/River projections (simplified for now)
        double turn_barrel_frequency;        // Estimated turn barrel frequency
        double river_value_frequency;        // Estimated river value frequency
        double expected_total_ev;            // EV through all streets
    };

    // Core analysis components
    BoardAnalysisCommand::BoardTextureClassifier board_classifier_;
    std::unique_ptr<PokerAnalyzer> poker_analyzer_;
    std::unique_ptr<MonteCarloEngine> monte_carlo_engine_;

    // Command implementation
    void print_help() const;
    void show_progress(int completed, int total, const std::string& current_item);
    
    // Phase 2: Board texture integration
    void run_phase2_board_integration();
    std::vector<ComprehensiveFlopResult> analyze_hands_on_board_textures(
        const std::vector<std::string>& hands,
        const std::vector<std::string>& boards,
        const std::string& position,
        const std::string& preflop_action
    );
    
    // Phase 3: Range evolution analysis
    void run_phase3_range_evolution();
    std::vector<RangeEvolution> analyze_range_evolution_sequences();
    RangeEvolution analyze_specific_sequence(
        const std::string& sequence_name,
        const std::vector<std::string>& initial_range,
        const std::string& board
    );
    
    // Phase 4: Multi-dimensional output
    void run_phase4_comprehensive_output();
    void export_comprehensive_csv(
        const std::vector<ComprehensiveFlopResult>& results,
        const std::string& filename
    );
    
    // Range evolution helpers
    std::vector<std::string> get_preflop_range(const std::string& position, const std::string& action);
    std::vector<std::string> evolve_range_postflop(
        const std::vector<std::string>& preflop_range,
        const BoardAnalysis& board_analysis,
        const std::string& flop_action
    );
    double calculate_range_tightness(const std::vector<std::string>& range);
    
    // Action frequency calculations
    double calculate_cbet_frequency(
        const std::string& hand,
        const BoardAnalysis& board_analysis,
        const std::string& position
    );
    double calculate_check_raise_frequency(
        const std::string& hand,
        const BoardAnalysis& board_analysis,
        const std::string& position
    );
    double calculate_action_ev(
        const std::string& hand,
        const BoardAnalysis& board_analysis,
        const std::string& action,
        const std::string& position
    );
    
    // Representative data sets
    std::vector<std::string> get_representative_hands() const;
    std::vector<std::string> get_representative_boards() const;
    std::vector<std::string> get_action_sequences() const;
    std::vector<std::string> get_positions() const;
    
    // Utility functions
    void create_output_directories();
    std::string format_board_texture(const BoardAnalysis& analysis) const;
    std::string determine_range_type(const std::vector<std::string>& range) const;
    
    // Statistics and validation
    void print_analysis_summary(const std::vector<ComprehensiveFlopResult>& results);
    bool validate_result(const ComprehensiveFlopResult& result);
};

} // namespace poker
