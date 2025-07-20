"""Simple integration test for the enhanced Poker Trainer."""

import sys
sys.path.append('.')

def test_excel_data_loader():
    """Test the Excel data loader functionality."""
    print("Testing Excel Data Loader...")
    
    try:
        from data.excel_data_loader import ExcelDataLoader
        
        loader = ExcelDataLoader()
        
        # Test available table sizes
        table_sizes = loader.get_available_table_sizes()
        print(f"  ✓ Available table sizes: {table_sizes}")
        
        # Test positions for each table size
        for table_size in table_sizes:
            positions = loader.get_positions_for_table_size(table_size)
            print(f"  ✓ {table_size} positions: {positions}")
        
        # Test actions
        actions = loader.get_available_actions()
        print(f"  ✓ Available actions: {actions}")
        
        # Test getting hands for a specific scenario
        if table_sizes:
            table_size = table_sizes[0]
            positions = loader.get_positions_for_table_size(table_size)
            if positions:
                position = positions[0]
                action = actions[0]
                
                hands = loader.get_training_hands_for_scenario(table_size, position, action)
                print(f"  ✓ Training hands for {table_size} {position} {action}: {len(hands)} hands")
                
                if hands:
                    print(f"  ✓ Sample hands: {hands[:5]}")
        
        # Test data integrity
        validation = loader.validate_data_integrity()
        print(f"  ✓ Data validation: {validation}")
        
        return True
        
    except Exception as e:
        print(f"  ❌ Excel Data Loader test failed: {e}")
        return False

def test_position_config():
    """Test the position configuration."""
    print("\nTesting Position Config...")
    
    try:
        from config.position_options import PositionConfig
        
        # Test table size options
        table_sizes = PositionConfig.get_available_table_sizes()
        print(f"  ✓ Available table sizes: {table_sizes}")
        
        # Test positions for each table size
        for table_size in table_sizes:
            positions = PositionConfig.get_positions_for_table_size(table_size)
            print(f"  ✓ {table_size} positions: {positions}")
        
        # Test actions
        actions = PositionConfig.get_available_actions()
        print(f"  ✓ Available actions: {actions}")
        
        # Test scenario formatting
        if table_sizes:
            scenario = PositionConfig.format_scenario_display("6-max", "UTG", "Opening-Raise")
            print(f"  ✓ Sample scenario display: {scenario}")
        
        return True
        
    except Exception as e:
        print(f"  ❌ Position Config test failed: {e}")
        return False

def test_imports():
    """Test that all modules can be imported successfully."""
    print("\nTesting Module Imports...")
    
    modules_to_test = [
        ("data.excel_data_loader", "ExcelDataLoader"),
        ("config.position_options", "PositionConfig"),
        ("core.session_manager", "SessionManager"),
        ("ui.components", "UIComponents"),
        ("training.training_engine", "TrainingEngine"),
        ("training.answer_processor", "AnswerProcessor"),
    ]
    
    all_passed = True
    
    for module_name, class_name in modules_to_test:
        try:
            module = __import__(module_name, fromlist=[class_name])
            getattr(module, class_name)
            print(f"  ✓ {module_name}.{class_name}")
        except Exception as e:
            print(f"  ❌ {module_name}.{class_name}: {e}")
            all_passed = False
    
    return all_passed

def main():
    """Run all integration tests."""
    print("=" * 50)
    print("POKER TRAINER INTEGRATION TESTS")
    print("=" * 50)
    
    tests = [
        test_imports,
        test_position_config,
        test_excel_data_loader,
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        if test():
            passed += 1
    
    print("\n" + "=" * 50)
    print(f"RESULTS: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed! The enhanced trainer should work correctly.")
        print("\nTo run the trainer:")
        print("  cd Trainer")
        print("  streamlit run app.py")
    else:
        print("❌ Some tests failed. Please check the configuration.")
    
    print("=" * 50)

if __name__ == "__main__":
    main()
